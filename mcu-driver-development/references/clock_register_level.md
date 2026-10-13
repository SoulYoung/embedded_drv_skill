# Clock 寄存器级实现

## 资料边界

厂家头文件仅提供寄存器基址、寄存器和位域宏。不得调用 `rcu_*()` 等厂家 SDK 函数；驱动自行读改写寄存器、轮询状态和计算频率。

## 继承接口编码

若平台已有 `TDrvClockIClk` / `TDrvClockRcmIdType` 将 ID 编为：

`组号 * 32 + 位号`

则完整继承枚举数值。门控和复位函数按 ID 选择 AHB、APB1、APB2 或附加寄存器组，再以低 5 位作为位号。不要建立“驱动 ID 到厂家枚举”的转换表。

## 门控与复位模式

```c
TDrvIrqDisableGlobal(&g_drv_irq);
if (id < 32U)
    reg = &RCU_AHBEN;
else if (id < 64U)
{
    reg = &RCU_APB1EN;
    id -= 32U;
}
...
reg_val = *reg;
if (enable)
    reg_val |= (1UL << id);
else
    reg_val &= ~(1UL << id);
*reg = reg_val;
TDrvIrqEnableGlobal(&g_drv_irq);
```

复位脉冲使用对应复位寄存器：先置位、再清位；具体时序以该平台参考实现和手册为准。

## 系统时钟与频率

在 `.c` 内部定义源频率、等待次数、PLL 参数。使用 `RCU_CTL`、`RCU_CFG0`、`RCU_CFG1` 等宏直接完成时钟切换。所有稳定和切换等待都有超时并上报 `Timeout` 或 `Fail`。

频率查询读取当前位域：恢复预分频编码、PLL 倍频编码、AHB/APB 分频编码后逐级计算。对位运算显式加括号，特别避免 `reg & mask == 0` 这种优先级错误。
