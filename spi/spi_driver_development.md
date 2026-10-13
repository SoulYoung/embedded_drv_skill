# SPI 驱动模块开发流程与适配评估

## 1. 结论

GD32E535 的普通 SPI 可以适配现有 SPI 的“一个控制器、多个逻辑片选端口、异步读写、写后读保持片选”对象模型，但不能直接照搬现有 AC78428 的传输实现。生成源码前应先完成本文件第 3 节列出的架构调整。

可直接保留的部分：

- `TDrvSpi` 控制器与 `TDrvSpiPort` 逻辑端口的父子对象关系；
- `Create/Open/Close/Destroy` 生命周期与控制器引用计数；
- 每个逻辑端口独立的 CPOL、CPHA、波特率、填充字节、手动/自动 CS 与收发项；
- `Read`、`Write`、`ReadAndWrite`、`WriteThenRead` 的上层语义；
- 一个控制器同一时刻只执行一个端口传输的 `busy_` 状态机。

## 2. 资料与硬件事实

开发时必须读取：

1. `doc/spi/demo/`（建立后）或当前 SPI 接口与配置头；
2. 芯片 SPI/I2S 用户手册；
3. `Include/gd32e53x_spi.h`、`Source/gd32e53x_spi.c`；
4. Clock、PORT、GPIO、IRQ 与 DMA 驱动接口。

GD32E535 普通 SPI 的关键事实：

- SPI0–SPI3 都可运行 SPI；SPI0、SPI3 额外支持四线 SPI 主机模式；
- 普通 SPI 仅支持 8 位或 16 位帧，发送/接收缓冲区为 16 位；
- 时钟分频仅支持 PCLK / 2、/4、/8、/16、/32、/64、/128、/256；
- 使用 `TBE`、`RBNE`、`TRANS` 与错误状态完成中断状态机；
- 主机硬件 NSS 在 SPI 使能后持续为低；NSS 脉冲模式在每两个数据帧间插入脉冲，且要求 `CKPH=0`；
- 全双工时发送每个帧都会产生一个接收帧，必须读出 RX 数据以避免溢出。

## 3. 现有架构的不适配点与调整建议

| 现有设计 | 不适配原因 | 建议 |
| --- | --- | --- |
| `auto_cs_` 依赖硬件 CS 覆盖一笔完整事务 | 硬件 NSS 从 SPI enable 到 disable 始终有效；NSSP 会在帧间脉冲，不能保持 `WriteThenRead` 的 CS | 所有 `TDrvSpiPort` 的 CS 都用 GPIO 控制；`auto_cs_` 的语义改为“驱动在事务开始/结束自动操作 GPIO CS” |
| `cell_size_` 支持 8/16/32 位 | 普通 SPI 只有 8/16 位；四线模式固定 8 位 | 普通接口只接受 8、16；32 返回 `kHccDrvSocCommonNotSupportLine`。四线模式未来单列特殊接口 |
| `prescale_low_`、`prescale_high_` | 该芯片只有一个 PSC[2:0]，没有高低电平独立分频 | 改为一个私有 `prescale_`；保留公开波特率接口，由驱动向下选择不超过目标值的 PCLK/2^n |
| `DRV_SPI_FRAME_MAX_SIZE = 512` 及 CONT 帧假设 | 这是原平台硬件帧/勘误限制，不是普通 SPI 限制 | 从通用接口与新平台实现中移除该硬件限制；若保留传输分段，仅作为驱动内部缓冲策略 |
| ISR 内直接调用 `OnComplete` | 新代码规范要求 ISR 不执行用户回调 | ISR 只完成搬运、清状态、设置完成事件；由任务/轮询上下文分派 `OnComplete`。若当前上层必须 ISR 回调，需用户明确接受例外 |
| `reverse` 仅交换 MOSI/MISO 配置 | SPI 支持全双工、单工、双向线模式，但引脚与方向必须匹配手册复用 | 作为平台支持项单独验证；普通全双工主机路径优先实现。无法保证引脚复用时返回不支持 |

因此，当前接口需要先确认以下调整后才适合实现：

1. `TDrvSpiPortSetAutoCs` 改为 GPIO 自动片选语义；
2. `TDrvSpiPortSetCellSize` 在本平台只允许 8 或 16；
3. 私有 `prescale_low_`/`prescale_high_` 改为单分频值；
4. 移除 AC 专用 512 字节帧限制及 CONT 依赖；
5. 明确完成回调是否允许 ISR 上下文；默认按规范转到非 ISR 上下文。

## 4. 生成前的接口与配置工作流

1. 以现有 `drv_spi.h/.c/.cfg.h` 整理通用 demo；把上述 AC 专用实现改为 `TODO` 或删除。
2. 保留控制器配置中的 SCK、MOSI、MISO 引脚和实例索引；CS 继续是每个 `TDrvSpiPort` 的 GPIO 配置。
3. 将帧大小约束写入 `TDrvSpiPortSetCellSize` 注释：目标普通 SPI 仅 8/16 位。
4. 在 `TDrvSpiPortSetBaudRate` 中用控制器 PCLK 和 2^n 分频选择实际不超过请求值的频率，并保存实际频率。
5. 在 `.h` 的特殊接口区按需增加四线 SPI、DMA、CRC、TI 模式或 NSSP；用户未明确要求时不实现。

## 5. 寄存器级实现工作流

1. 控制器 Open：开启 SPI 时钟，配置 SCK/MOSI/MISO 复用，关闭 SPI 后配置 `CTL0/CTL1`，最后置 `SPIEN`。
2. 端口 Open：配置 CS 为 GPIO 输出并置为无效电平；计算分频、CPOL/CPHA、帧宽；不使用硬件 NSS 作为多从机事务 CS。
3. 提交传输：检查 `busy_`、缓冲区、长度和帧宽；自动 CS 时先拉低 GPIO CS；初始化 TX/RX 进度和错误状态。
4. 中断传输：`TBE` 写入发送数据或填充字节；`RBNE` 读取数据并保存或丢弃；检查溢出、配置错误与 CRC 错误；最后等待 `TRANS=0`。
5. 完成：关闭 TBE/RBNE 中断，自动 CS 时拉高 GPIO CS，清 `busy_` 并记录完成事件；在非 ISR 上下文调用回调。
6. Close：等待或取消活动传输，禁止 SPI 中断，确认 `TRANS=0` 后关闭 SPI；不要擅自释放其他端口仍在使用的控制器资源。

## 6. 验证清单

- [ ] 4 个 SPI 实例、时钟和复用映射正确；
- [ ] 8/16 位正常，32 位返回不支持；
- [ ] 四种 CPOL/CPHA 组合与目标设备时序正确；
- [ ] 波特率为请求值以下最接近的 PCLK/2^n；
- [ ] 写、读、全双工、写后读均能保持 GPIO CS 时序；
- [ ] 只写时仍读取 RX 寄存器，避免 `RXORERR`；
- [ ] TBE/RBNE/TRANS、溢出和配置错误均有明确收尾；
- [ ] 两个逻辑端口不可并发占用同一控制器；
- [ ] 回调上下文符合最终确定的架构约定。
