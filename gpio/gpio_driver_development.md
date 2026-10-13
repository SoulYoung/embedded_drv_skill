# GPIO 驱动模块开发规范

## 模块职责

GPIO 负责单引脚原子电平读写，并管理 GPIO 外部中断对象。PORT 先完成引脚模式配置；GPIO 不修改复用、上下拉和输出类型。

## 必要接口

- `TDrvGpioCreate/Destroy`、`TDrvGpioOpen/Close`；
- `TDrvGpioSetLevel/SetHighLevel/SetLowLevel/GetLevel`；
- `TDrvGpioIntCreate/Destroy/SetMode/Open/IsOpen/Close/GetFlag`。

## 实现流程

1. 以 demo 的对象、配置、触发枚举和完整接口注释为骨架。
2. 从目标芯片手册确认输入采样、原子置位/清零、输出锁存、方向控制、中断状态和中断配置寄存器；原子写能力不足时明确临界区策略。
3. 确认中断源选择、触发类型、Pending 清除语义、端口/引脚共享关系及其对应 NVIC IRQ；不得假定使用某种外部中断控制器或固定分组方式。
4. 根据共享关系决定资源冲突规则和 NVIC 的启停条件；同一硬件资源不得被不兼容的对象重复配置。
5. ISR 只清除本驱动登记的中断源并记录最小状态；若需要用户回调，按运行环境提供任务上下文分发或延后处理机制。

## 验证清单

- [ ] 电平写操作使用原子置位/清零寄存器；
- [ ] 中断资源冲突规则与芯片共享关系一致；
- [ ] GPIO 中断源与对应 NVIC IRQ 正确；
- [ ] Pending 按目标芯片规定的语义清除，且不影响未登记的中断源；
- [ ] 重复 Open/Close 与无效触发模式返回正确错误码。
