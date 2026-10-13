#include "drv_gpio.h"
#include "drv_common.h"

TDrvGpio g_drv_gpio;    ///< 通用输入输出驱动对象

Int32 TDrvGpioCreate(TDrvGpio *self)
{
    Int32 result;

    if (self == NULL)
        result = kHccDrvSocCommonParamLine;
    else
    {
        self->cross_tag_ = kDrvMemCrossTagCreate;
        self->is_open_ = False;
        result = kHccDrvSocCommonSuccessLine;
    }

    return result;
}

Int32 TDrvGpioDestroy(TDrvGpio *self)
{
    Int32 result;

    if (self == NULL)
        result = kHccDrvSocCommonParamLine;
    else
    {
        if (self->is_open_)
            TDrvGpioClose(self);
        self->cross_tag_ = kDrvMemCrossTagDestroy;
        result = kHccDrvSocCommonSuccessLine;
    }

    return result;
}

Int32 TDrvGpioOpen(TDrvGpio *self)
{
    Int32 result;

    if (self == NULL)
        result = kHccDrvSocCommonParamLine;
    else if (self->is_open_)
        result = kHccDrvSocCommonOpenedLine;
    else
    {
        self->cross_tag_ = kDrvMemCrossTagOpen;
        self->is_open_ = True;
        result = kHccDrvSocCommonSuccessLine;
    }

    return result;
}

Int32 TDrvGpioClose(TDrvGpio *self)
{
    Int32 result;

    if (self == NULL)
        result = kHccDrvSocCommonParamLine;
    else if (!self->is_open_)
        result = kHccDrvSocCommonClosedLine;
    else
    {
        self->cross_tag_ = kDrvMemCrossTagClose;
        self->is_open_ = False;
        result = kHccDrvSocCommonSuccessLine;
    }

    return result;
}

Int32 TDrvGpioSetLevel(TDrvGpio *self, UInt8 group, UInt8 pin, Bool level)
{
    Int32 result;

    if (self == NULL || group >= kDrvPortGroupMax || pin >= kDrvPortPinMax)
        result = kHccDrvSocCommonParamLine;
    else if (!self->is_open_)
        result = kHccDrvSocCommonClosedLine;
    else if (level)
        result = TDrvGpioSetHighLevel(self, group, pin);
    else
        result = TDrvGpioSetLowLevel(self, group, pin);

    return result;
}

Int32 TDrvGpioSetHighLevel(TDrvGpio *self, UInt8 group, UInt8 pin)
{
    Int32 result;

    if (self == NULL || group >= kDrvPortGroupMax || pin >= kDrvPortPinMax)
        result = kHccDrvSocCommonParamLine;
    else if (!self->is_open_)
        result = kHccDrvSocCommonClosedLine;
    else
    {
        /* TODO: 写目标芯片 GPIO 原子置位寄存器。 */
        result = kHccDrvSocCommonNotSupportLine;
    }

    return result;
}

Int32 TDrvGpioSetLowLevel(TDrvGpio *self, UInt8 group, UInt8 pin)
{
    Int32 result;

    if (self == NULL || group >= kDrvPortGroupMax || pin >= kDrvPortPinMax)
        result = kHccDrvSocCommonParamLine;
    else if (!self->is_open_)
        result = kHccDrvSocCommonClosedLine;
    else
    {
        /* TODO: 写目标芯片 GPIO 原子清零寄存器。 */
        result = kHccDrvSocCommonNotSupportLine;
    }

    return result;
}

Int32 TDrvGpioGetLevel(TDrvGpio *self, UInt8 group, UInt8 pin, Bool *level)
{
    Int32 result;

    if (self == NULL || level == NULL || group >= kDrvPortGroupMax || pin >= kDrvPortPinMax)
        result = kHccDrvSocCommonParamLine;
    else if (!self->is_open_)
        result = kHccDrvSocCommonClosedLine;
    else
    {
        /* TODO: 读取目标芯片 GPIO 输入状态寄存器并赋值给 level。 */
        result = kHccDrvSocCommonNotSupportLine;
    }

    return result;
}

Int32 TDrvGpioIntCreate(TDrvGpioInt *self, const TDrvGpioIntCfg *cfg)
{
    Int32 result;

    if (self == NULL || cfg == NULL || cfg->group >= kDrvPortGroupMax
        || cfg->pin >= kDrvPortPinMax || cfg->run_trigger >= kDrvGpioIntTriggerMax
        || cfg->sleep_trigger >= kDrvGpioIntTriggerMax)
        result = kHccDrvSocCommonParamLine;
    else
    {
        self->cross_tag_ = kDrvMemCrossTagCreate;
        self->receiver = NULL;
        self->OnIrq = NULL;
        self->next_ = NULL;
        self->cfg_ = cfg;
        self->permanent_ = False;
        self->is_open_ = False;
        /* TODO: 将对象登记到目标芯片 GPIO 中断资源表。 */
        result = kHccDrvSocCommonSuccessLine;
    }

    return result;
}

Int32 TDrvGpioIntDestroy(TDrvGpioInt *self)
{
    Int32 result;

    if (self == NULL)
        result = kHccDrvSocCommonParamLine;
    else
    {
        if (self->is_open_)
            TDrvGpioIntClose(self);
        self->cross_tag_ = kDrvMemCrossTagDestroy;
        self->next_ = NULL;
        self->receiver = NULL;
        self->OnIrq = NULL;
        self->cfg_ = NULL;
        /* TODO: 从目标芯片 GPIO 中断资源表撤销对象。 */
        result = kHccDrvSocCommonSuccessLine;
    }

    return result;
}

Int32 TDrvGpioIntSetMode(TDrvGpioInt *self, Bool permanent)
{
    Int32 result;

    if (self == NULL)
        result = kHccDrvSocCommonParamLine;
    else
    {
        self->permanent_ = permanent;
        result = kHccDrvSocCommonSuccessLine;
    }

    return result;
}

Int32 TDrvGpioIntOpen(TDrvGpioInt *self)
{
    Int32 result;

    if (self == NULL || self->cfg_ == NULL)
        result = kHccDrvSocCommonParamLine;
    else if (self->is_open_)
        result = kHccDrvSocCommonOpenedLine;
    else
    {
        /* TODO: 配置 GPIO 中断源、触发边沿、Pending 和 NVIC 资源。 */
        result = kHccDrvSocCommonNotSupportLine;
    }

    return result;
}

Int32 TDrvGpioIntIsOpen(TDrvGpioInt *self, Bool *is_open)
{
    Int32 result;

    if (self == NULL || is_open == NULL)
        result = kHccDrvSocCommonParamLine;
    else
    {
        *is_open = self->is_open_;
        result = kHccDrvSocCommonSuccessLine;
    }

    return result;
}

Int32 TDrvGpioIntClose(TDrvGpioInt *self)
{
    Int32 result;

    if (self == NULL)
        result = kHccDrvSocCommonParamLine;
    else if (!self->is_open_)
        result = kHccDrvSocCommonClosedLine;
    else
    {
        /* TODO: 关闭目标 GPIO 中断源，并按共享关系释放 NVIC 资源。 */
        result = kHccDrvSocCommonNotSupportLine;
    }

    return result;
}

Int32 TDrvGpioIntGetFlag(TDrvGpioInt *self, Bool *flag)
{
    Int32 result;

    if (self == NULL || flag == NULL)
        result = kHccDrvSocCommonParamLine;
    else if (!self->is_open_)
        result = kHccDrvSocCommonClosedLine;
    else
    {
        /* TODO: 读取并按目标芯片规则清除该 GPIO 中断 Pending 标志。 */
        result = kHccDrvSocCommonNotSupportLine;
    }

    return result;
}

