#include "drv_reset.h"
#include "drv_common.h"

TDrvReset g_drv_reset;

Int32 TDrvResetCreate(TDrvReset *self)
{
    Int32 result = kHccDrvSocCommonSuccessLine;
    if (self == NULL)
        result = kHccDrvSocCommonParamLine;
    else
    {
        self->cross_tag_ = kDrvMemCrossTagCreate;
        self->reason = 0;
        self->is_open = False;
    }
    return result;
}

Int32 TDrvResetDestroy(TDrvReset *self)
{
    Int32 result = kHccDrvSocCommonSuccessLine;
    if (self == NULL)
        result = kHccDrvSocCommonParamLine;
    else
    {
        if (self->is_open)
            TDrvResetClose(self);
        self->cross_tag_ = kDrvMemCrossTagDestroy;
    }
    return result;
}

Int32 TDrvResetOpen(TDrvReset *self)
{
    Int32 result = kHccDrvSocCommonSuccessLine;
    if (self == NULL)
        result = kHccDrvSocCommonParamLine;
    else if (self->is_open)
        result = kHccDrvSocCommonOpenedLine;
    else
    {
        /* TODO: 读取目标MCU复位状态寄存器，并映射到TDrvResetChipReason。 */
        self->cross_tag_ = kDrvMemCrossTagOpen;
        self->is_open = True;
    }
    return result;
}

Int32 TDrvResetClose(TDrvReset *self)
{
    Int32 result = kHccDrvSocCommonSuccessLine;
    if (self == NULL)
        result = kHccDrvSocCommonParamLine;
    else if (!self->is_open)
        result = kHccDrvSocCommonClosedLine;
    else
    {
        self->cross_tag_ = kDrvMemCrossTagClose;
        self->is_open = False;
    }
    return result;
}

Int32 TDrvResetGetReason(TDrvReset *self, TDrvResetType type, UInt32 *reason)
{
    Int32 result = kHccDrvSocCommonSuccessLine;
    if (self == NULL || reason == NULL)
        result = kHccDrvSocCommonParamLine;
    else if (!self->is_open)
        result = kHccDrvSocCommonNotReadyLine;
    else if (type >= kDrvResetTypeMax)
        result = kHccDrvSocCommonParamLine;
    else
        *reason = self->reason;
    return result;
}

Int32 TDrvResetClrFlags(TDrvReset *self, TDrvResetType type)
{
    Int32 result = kHccDrvSocCommonSuccessLine;
    if (self == NULL)
        result = kHccDrvSocCommonParamLine;
    else if (!self->is_open)
        result = kHccDrvSocCommonNotReadyLine;
    else if (type >= kDrvResetTypeMax)
        result = kHccDrvSocCommonParamLine;
    else
    {
        /* TODO: 按目标MCU手册规定的写一清零或命令序列清除复位标志。 */
        self->reason = 0;
    }
    return result;
}

Int32 TDrvResetTrigger(TDrvReset *self, TDrvResetTriggerMode mode)
{
    Int32 result = kHccDrvSocCommonSuccessLine;
    if (self == NULL)
        result = kHccDrvSocCommonParamLine;
    else if (!self->is_open)
        result = kHccDrvSocCommonNotReadyLine;
    else if (mode >= TDrvResetTriggerMax)
        result = kHccDrvSocCommonParamLine;
    else
    {
        (void)mode;
        /* TODO: 执行目标MCU系统复位请求，并按内核要求加入必要屏障。 */
        result = kHccDrvSocCommonNotSupportLine;
    }
    return result;
}
