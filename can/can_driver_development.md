# CAN 驱动模块开发规范

## 1. 模块职责

CAN 驱动管理控制器、收发环形缓存、普通与优先报文发送状态、接收通知、错误状态和硬件过滤器。协议骨架包括 `TCanData`、`TCanfdData`、双发送状态、Bus-Off、波特率侦测和 ISR 回调语义；目标平台适配只负责将其映射到控制器、邮箱或 FIFO、时钟、端口和中断资源。

## 2. 必要接口

- `TDrvCanCreate/Destroy/Open/Close`
- `TDrvCanRead/Write`
- `TDrvCanWritePreemptive`
- `TDrvCanGetBaudRate/TDrvCanfdGetBaudRate`
- `TDrvCanSetBaudRate/TDrvCanfdSetBaudRate`
- `TDrvCanSetBaudRateEx/TDrvCanfdSetBaudRateEx`
- `TDrvCanSetMode`
- `TDrvCanAutoRecover/TDrvCanSetAutoRecover/TDrvCanBusOffRecover`
- `TDrvCanDetectMode/TDrvCanSetDetectMode/TDrvCanBusNo/TDrvCanBusOff`
- `TDrvCanAbortSend/TDrvCanClearSendFifo`
- `TDrvCanSetFifoFilter/TDrvCanGetFifoFilterCount/TDrvCanGetErrorFrameCount`

## 3. Demo、参考实现与平台适配

CAN demo 与同模块参考实现共同定义接口外形、数据结构、状态迁移、错误码、回调上下文和注释。移植时必须保留这些协议内容；不能因为目标控制器使用不同的消息 RAM、FIFO 或中断划分而删除核心对象、优先发送状态或 CAN FD 接口。

demo 的 `.c` 只保留通用生命周期、缓存、状态机和错误码处理。外设基址、实例数量、时钟编号、端口复用、IRQ、消息 RAM 布局和 SDK 调用必须移至平台 `.c`；demo 以说明前置条件和完成条件的 `TODO` 表示这些适配点。

## 4. 实现规则

1. 从手册确认控制器时钟、位时序、消息 RAM/FIFO、过滤器、错误状态和 CAN FD 支持范围；SDK 仅用于核对寄存器配置顺序，平台驱动直接访问寄存器。
2. `Create` 绑定调用方提供的收发缓存并初始化所有协议状态；`Open` 依次完成时钟、端口、控制器、过滤器和中断配置。任一关键步骤失败时按反序收口已取得资源。
3. 普通发送 FIFO 与优先发送必须分别维护软件状态和硬件发送资源。发送完成后先以硬件最终状态确定成功或中止，再释放对应软件状态、通知回调并启动下一普通报文。
4. 接收 ISR 必须依目标手册的锁定、读取、确认和解锁顺序服务单个硬件接收项；只有完整报文成功写入软件缓存后才触发 `OnRecv`。不得用读取邮箱状态代替规定的中断标志确认流程。
5. 报文接收、发送完成、发送错误、Bus-Off 和波特率侦测分别由私有处理函数承担。波特率侦测模式只报告侦测结果和接收通知，不得错误调用普通业务的 `OnSent` 或 `OnBusOff`。
6. 错误重试、发送中止和 Bus-Off 处理以硬件最终状态为准。达到项目既有的重试上限时，中止在途报文并由完成路径以失败结果回调；不把中止请求本身当作已经完成。
7. `Close` 先禁止外设中断源和 NVIC，再停止控制器并确认其已静止，之后才恢复端口和关闭时钟。硬件状态轮询必须有超时；公开结果码接口以 `result` 单一出口返回并按日志规范上报错误。
8. CAN FD 数据段位时序必须在打开前有效。数据段速率、帧格式位和速率切换位是不同语义，不能因数据长度或是否请求速率切换而混淆。

## 5. 验证清单

- [ ] 标准帧、扩展帧、远程帧及支持的 CAN FD 帧收发正确。
- [ ] 位时序与实际控制器时钟一致，CAN FD 数据段在打开前已完成配置。
- [ ] 普通与优先发送并发时各自的完成回调、错误计数和中止结果正确。
- [ ] 接收缓存满、接收中断紧邻到达和中止完成时，报文不重复、不误报成功。
- [ ] 过滤器、Bus-Off 手动/自动恢复和波特率侦测不干扰普通业务状态。
- [ ] 重复 Open/Close、打开失败回退、关闭超时和销毁后访问均有确定行为。