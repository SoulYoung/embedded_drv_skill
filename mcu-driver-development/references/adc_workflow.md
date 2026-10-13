# ADC 模块工作流

## 必要接口

控制器：`TDrvAdcCreate`、`TDrvAdcOpen`、`TDrvAdcClose`、`TDrvAdcDestroy`。

通道：`TDrvAdcChCreate`、`TDrvAdcChOpen`、`TDrvAdcChClose`、`TDrvAdcChDestroy`、`TDrvAdcChReadOrigValue`、`TDrvAdcChReadConvValue`。

## 特殊接口

平台具备的校准、扫描、连续转换、外部触发、DMA、模拟看门狗、内部参考或温度通道，直接放入 `drv_adc.h` 的特殊接口分组，并注明适用条件和不支持时的错误码。

## 同步采样时序

检查对象和通道状态，检查通道范围，选择通道，清理旧完成/错误标志，启动转换，带超时等待完成，读取原始码，清理或确认状态，返回结果。

## 工程量转换

```text
voltage = raw * reference_mv / full_scale
input   = voltage * divider_den / divider_num + offset
```

检查满量程和分压参数不为 0，乘法使用宽位宽中间值，明确截断、舍入、负偏移和溢出规则。

## 平台资料重点

确认 ADC 实例和通道数量、分辨率、参考源、采样时间编码、最大时钟、启动和 EOC 时序、错误标志、校准步骤、模拟引脚配置、低功耗行为以及多通道共享控制器的打开关闭关系。

## 模板填写要求

以 `../../adc/demo/drv_adc.h`、`../../adc/demo/drv_adc_cfg.h` 和 `../../adc/demo/drv_adc.c` 为起点。保留 `x/y` 等既有配置字段、父对象 `c_ref_/o_ref_` 引用计数和子对象 `is_open_` 状态，不重写生命周期、参数检查、父子对象管理和工程量计算。只在平台适配区填写 TODO：配置表、实例/通道校验、时钟复位、端口复用、寄存器初始化、校准、EOC/错误处理、原始码读取和关闭恢复。

每个 TODO 必须由用户手册、芯片头文件或 SDK 依据支撑；全部 TODO 完成前，适配函数应保持返回“不支持”或明确失败，不能伪造硬件成功。
