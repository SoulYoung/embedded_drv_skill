#include "drv_port.h"
#include "drv_common.h"

TDrvPort g_drv_port;    ///< 端口驱动对象

Int32 TDrvPortCreate(TDrvPort *self)
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

Int32 TDrvPortDestroy(TDrvPort *self)
{
    Int32 result;

    if (self == NULL)
        result = kHccDrvSocCommonParamLine;
    else
    {
        if (self->is_open_)
            TDrvPortClose(self);
        self->cross_tag_ = kDrvMemCrossTagDestroy;

        result = kHccDrvSocCommonSuccessLine;
    }

    return result;
}

Int32 TDrvPortOpen(TDrvPort *self)
{
    Int32 result;

    if (self == NULL)
        result = kHccDrvSocCommonParamLine;
    else if (self->is_open_)
        result = kHccDrvSocCommonOpenedLine;
    else
    {
        /* TODO: 开启目标芯片 PORT/GPIO/复用控制单元需要的时钟。 */
        self->cross_tag_ = kDrvMemCrossTagOpen;
        self->is_open_ = True;

        result = kHccDrvSocCommonSuccessLine;
    }

    return result;
}

Int32 TDrvPortClose(TDrvPort *self)
{
    Int32 result;

    if (self == NULL)
        result = kHccDrvSocCommonParamLine;
    else if (!self->is_open_)
        result = kHccDrvSocCommonClosedLine;
    else
    {
        /* TODO: 仅在硬件要求且不影响其他模块时释放 PORT 共享资源。 */
        self->cross_tag_ = kDrvMemCrossTagClose;
        self->is_open_ = False;

        result = kHccDrvSocCommonSuccessLine;
    }

    return result;
}

Int32 TDrvPortSetMux(TDrvPort *self, UInt8 group, UInt8 pin, UInt8 mux)
{
    Int32 result;

    if (self == NULL || group >= kDrvPortGroupMax || pin >= kDrvPortPinMax || mux >= kDrvPortMuxAltMax)
        result = kHccDrvSocCommonParamLine;
    else if (!self->is_open_)
        result = kHccDrvSocCommonClosedLine;
    else
    {
        /* TODO: 按目标芯片复用/模式寄存器设置 group 的 pin 为 mux。 */
        result = kHccDrvSocCommonNotSupportLine;
    }

    return result;
}

Int32 TDrvPortSetInputEnabled(TDrvPort *self, UInt8 group, UInt8 pin, Bool enable)
{
    Int32 result;

    if (self == NULL || group >= kDrvPortGroupMax || pin >= kDrvPortPinMax)
        result = kHccDrvSocCommonParamLine;
    else if (!self->is_open_)
        result = kHccDrvSocCommonClosedLine;
    else
    {
        /* TODO: 按目标芯片语义设置输入缓冲或输入模式。 */
        result = kHccDrvSocCommonNotSupportLine;
    }

    return result;
}

Int32 TDrvPortSetOutputEnabled(TDrvPort *self, UInt8 group, UInt8 pin, Bool enable)
{
    Int32 result;

    if (self == NULL || group >= kDrvPortGroupMax || pin >= kDrvPortPinMax)
        result = kHccDrvSocCommonParamLine;
    else if (!self->is_open_)
        result = kHccDrvSocCommonClosedLine;
    else
    {
        /* TODO: 按目标芯片语义设置输出缓冲或输出模式。 */
        result = kHccDrvSocCommonNotSupportLine;
    }

    return result;
}

Int32 TDrvPortSetPullMode(TDrvPort *self, UInt8 group, UInt8 pin, UInt8 pull)
{
    Int32 result;

    if (self == NULL || group >= kDrvPortGroupMax || pin >= kDrvPortPinMax || pull >= kDrvPortPullMax)
        result = kHccDrvSocCommonParamLine;
    else if (!self->is_open_)
        result = kHccDrvSocCommonClosedLine;
    else
    {
        /* TODO: 读改写目标引脚的上拉、下拉字段。 */
        result = kHccDrvSocCommonNotSupportLine;
    }

    return result;
}

Int32 TDrvPortSetOpenDrainEnable(TDrvPort *self, UInt8 group, Int32 pin, Bool enable)
{
    Int32 result;

    if (self == NULL || group >= kDrvPortGroupMax || pin < 0 || pin >= kDrvPortPinMax)
        result = kHccDrvSocCommonParamLine;
    else if (!self->is_open_)
        result = kHccDrvSocCommonClosedLine;
    else
    {
        /* TODO: 读改写目标引脚的推挽/开漏字段。 */
        result = kHccDrvSocCommonNotSupportLine;
    }

    return result;
}
