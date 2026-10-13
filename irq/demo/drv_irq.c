#include "drv_irq.h"
#include "drv_common.h"

enum
{
    kDrvIrqGlobalRefMax = 127,
};

TDrvIrq g_drv_irq;

Int32 TDrvIrqCreate(TDrvIrq *self)
{
    Int32 result;

    if (self == NULL)
        result = kHccDrvSocCommonParamLine;
    else
    {
        self->cross_tag_ = kDrvMemCrossTagCreate;
        self->global_ref_ = 0;
        self->is_open_ = False;
        result = kHccDrvSocCommonSuccessLine;
    }

    return result;
}

Int32 TDrvIrqDestroy(TDrvIrq *self)
{
    Int32 result;

    if (self == NULL)
        result = kHccDrvSocCommonParamLine;
    else
    {
        if (self->is_open_)
            TDrvIrqClose(self);
        self->cross_tag_ = kDrvMemCrossTagDestroy;
        result = kHccDrvSocCommonSuccessLine;
    }

    return result;
}

Int32 TDrvIrqOpen(TDrvIrq *self)
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

Int32 TDrvIrqClose(TDrvIrq *self)
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

Int32 TDrvIrqEnableGlobal(TDrvIrq *self)
{
    Int32 result;

    if (self == NULL)
        result = kHccDrvSocCommonParamLine;
    else if (!self->is_open_)
        result = kHccDrvSocCommonClosedLine;
    else if (self->global_ref_ == 0)
        result = kHccDrvSocCommonSuccessLine;
    else
    {
        self->global_ref_--;
        if (self->global_ref_ == 0)
        {
            /* TODO: 调用目标CMSIS的全局中断使能接口。 */
        }
        result = kHccDrvSocCommonSuccessLine;
    }

    return result;
}

Int32 TDrvIrqDisableGlobal(TDrvIrq *self)
{
    Int32 result;

    if (self == NULL)
        result = kHccDrvSocCommonParamLine;
    else if (!self->is_open_)
        result = kHccDrvSocCommonClosedLine;
    else if (self->global_ref_ == kDrvIrqGlobalRefMax)
        result = kHccDrvSocCommonOutOfRangeLine;
    else
    {
        if (self->global_ref_ == 0)
        {
            /* TODO: 调用目标CMSIS的全局中断禁止接口。 */
        }
        self->global_ref_++;
        result = kHccDrvSocCommonSuccessLine;
    }

    return result;
}

Int32 TDrvIrqEnable(TDrvIrq *self, UInt8 num)
{
    Int32 result;

    if (self == NULL)
        result = kHccDrvSocCommonParamLine;
    else if (!self->is_open_)
        result = kHccDrvSocCommonClosedLine;
    else
    {
        /* TODO: 使用目标芯片头文件定义的最大外设 IRQ 编号校验 num。 */
        /* TODO: 调用 NVIC_EnableIRQ((IRQn_Type)num)。 */
        result = kHccDrvSocCommonNotSupportLine;
    }

    return result;
}

Int32 TDrvIrqDisable(TDrvIrq *self, UInt8 num)
{
    Int32 result;

    if (self == NULL)
        result = kHccDrvSocCommonParamLine;
    else if (!self->is_open_)
        result = kHccDrvSocCommonClosedLine;
    else
    {
        /* TODO: 使用目标芯片头文件定义的最大外设 IRQ 编号校验 num。 */
        /* TODO: 调用 NVIC_DisableIRQ((IRQn_Type)num)。 */
        result = kHccDrvSocCommonNotSupportLine;
    }

    return result;
}
