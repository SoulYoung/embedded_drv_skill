# Clock 驱动模块开发规范

## 1. 模块职责

Clock 驱动负责系统时钟源、PLL、AHB/APB 分频、外设接口时钟门控、外设软复位、功能时钟选择和 CLKOUT 输出。其他外设驱动打开前必须先完成 Clock 初始化。

## 2. 必要接口

- `TDrvClockCreate/Destroy`
- `TDrvClockOpen/Close`
- `TDrvClockGetSrcFreq`
- `TDrvClockSetPeriphIClk`
- `TDrvClockSetPeriphFClk`
- `TDrvClockSoftResetPeriph`
- `TDrvClockSetRunOutGate`

用户未明确提出其他能力时，不增加特殊接口。

## 3. 实现规则

1. **先完整复制并逐项核对 `demo/drv_clock.h/.c`。** demo 是 Clock 模块的公开接口、文件头、必要/特殊接口分组、Doxygen 注释、对象模型、生命周期、函数顺序和代码格式的唯一骨架；填写平台实现时不得删改这些非硬件内容。仅在手册确认目标 MCU 与 demo 不能表达的能力时，才在特殊接口分组中补充。
2. 先读取目标平台的同模块参考实现。保留公开接口、注释、对象成员和枚举值，尤其是外设时钟/复位 ID 的数值。
3. 厂家头文件仅作为寄存器、位域和常量来源。芯片平台相关头文件统一由 `drv_common.h` 包含；模块 `.c` 只包含自身 `.h` 和 `drv_common.h`，不得重复包含芯片头文件。禁止调用 `rcu_*()` 或其他厂家 SDK 封装函数；使用宏直接读改写寄存器。
4. 若外设 ID 采用“寄存器组 × 32 + 位号”编码，`TDrvClockSetPeriphIClk` 与 `TDrvClockSoftResetPeriph` 按数值选择 AHB、APB1、APB2、附加 APB 寄存器，再对低 5 位进行置位/清位。不要转换为厂家 SDK 枚举。
5. 系统时钟按平台资料的顺序操作：切换安全源、清理旧配置、启动并等待时钟源、设置 Flash 等待周期和总线分频、配置 PLL、等待锁定、切换系统源、确认状态、执行升频保护。
6. 每个等待过程必须有有限超时并返回通用错误码，不能永久阻塞。
7. 频率查询从当前时钟状态和分频位域还原实际频率；不能调用厂家查询 API，也不能只返回初始化常量。
8. 接口时钟和复位的寄存器读改写置于 IRQ 临界区；复位脉冲先置复位位、再清复位位。

## 4. Demo 骨架核对清单

- [ ] 目标 `drv_clock.h/.c` 由 Clock demo 完整复制开始，而非从其他 MCU 的 Clock 文件复制。
- [ ] 文件头、include guard、对象结构、必要/特殊接口分组和所有既有 Doxygen 注释均被保留。
- [ ] 系统时钟源、外设时钟/复位标识和 CLKOUT 枚举仅按目标芯片手册补齐，不改变既有成员的语义。
- [ ] `.c` 仅包含 `drv_clock.h` 和 `drv_common.h`；SDK 芯片头由 `drv_common.h` 集中提供。
- [ ] 代码格式保持 Clock demo 的局部格式；全局编码规则按当前任务显式导入的规范执行。

## 5. 验证清单

- [ ] 目标板的晶振频率和 PLL 参数符合板级资料。
- [ ] 时钟源、PLL 锁定与系统切换都能超时返回。
- [ ] 升频前已设置 Flash 等待周期。
- [ ] AHB/APB 分频和频率查询结果一致。
- [ ] 门控、复位 ID 与 RCU 寄存器位一一对应。
- [ ] Clock 未打开、非法 ID、重复 Open/Close 均返回确定错误码。
- [ ] 没有厂家 SDK 函数调用；仅使用厂家头文件中的寄存器宏。
