#include "drv_pmc.h"
#include "drv_common.h"

TDrvPmc g_drv_pmc;

Int32 TDrvPmcCreate(TDrvPmc *self)
{
    Int32 result = kHccDrvSocCommonSuccessLine;
    if (self == NULL)
        result = kHccDrvSocCommonParamLine;
    else
    {
        self->cross_tag_ = kDrvMemCrossTagCreate;
        self->is_open_ = False;
    }
    return result;
}

Int32 TDrvPmcDestroy(TDrvPmc *self)
{
    Int32 result = kHccDrvSocCommonSuccessLine;
    if (self == NULL)
        result = kHccDrvSocCommonParamLine;
    else
    {
        if (self->is_open_)
            TDrvPmcClose(self);
        self->cross_tag_ = kDrvMemCrossTagDestroy;
    }
    return result;
}

Int32 TDrvPmcOpen(TDrvPmc *self)
{
    Int32 result = kHccDrvSocCommonSuccessLine;
    if (self == NULL)
        result = kHccDrvSocCommonParamLine;
    else if (self->is_open_)
        result = kHccDrvSocCommonOpenedLine;
    else
    {
        /* TODO: 按手册配置PMC时钟、低功耗入口前置条件和可用唤醒源。 */
        self->cross_tag_ = kDrvMemCrossTagOpen;
        self->is_open_ = True;
    }
    return result;
}

Int32 TDrvPmcClose(TDrvPmc *self)
{
    Int32 result = kHccDrvSocCommonSuccessLine;
    if (self == NULL)
        result = kHccDrvSocCommonParamLine;
    else if (!self->is_open_)
        result = kHccDrvSocCommonClosedLine;
    else
    {
        /* TODO: 清理PMC软件状态；不可擅自关闭仍由其他模块使用的时钟。 */
        self->cross_tag_ = kDrvMemCrossTagClose;
        self->is_open_ = False;
    }
    return result;
}

Int32 TDrvPmcSelectMode(TDrvPmc *self, TDrvPmcMode mode)
{
    Int32 result;
    if (self == NULL)
        result = kHccDrvSocCommonParamLine;
    else if (!self->is_open_)
        result = kHccDrvSocCommonNotReadyLine;
    else if (mode >= kDrvPmcModeMax)
        result = kHccDrvSocCommonParamLine;
    else
    {
        switch (mode)
        {
        case kDrvPmcModeRun:
        case kDrvPmcModeSleep:
        case kDrvPmcModeVlps:
        case kDrvPmcModeStandby:
            /* TODO: 按模式配置保持域、唤醒源、睡眠深度，并执行芯片规定的模式入口。 */
            result = kHccDrvSocCommonNotSupportLine;
            break;

        default:
            result = kHccDrvSocCommonParamLine;
            break;
        }
    }
    return result;
}
