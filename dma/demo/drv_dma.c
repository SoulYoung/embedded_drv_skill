#include "drv_dma.h"
#include "drv_common.h"

TDrvDma g_drv_dma;
static TDrvDmaCh *g_dma_channels_[DMA_CH_MAX] = {NULL};

/** DMA通道中断统一入口。 */
void TDrvDmaIrqHandler(Int32 channel)
{
    if (channel >= 0 && channel < DMA_CH_MAX && g_dma_channels_[channel] != NULL)
    {
        // TODO: 读取并清除完成/错误标志；错误时先停止请求，再调用OnError；完成时调用OnComplete。
    }
}

/** 使能或关闭DMA通道中断。 */
Int32 TDrvDmaChSetIrqEnable(TDrvDmaCh *self, Bool enabled)
{
    Int32 result = kHccDrvSocCommonSuccessLine;
    if (self == NULL)
        result = kHccDrvSocCommonParamLine;
    else if (!self->is_open_)
        result = kHccDrvSocCommonClosedLine;
    else
    {
        (void)enabled;
        // TODO: 开关目标MCU对应DMA通道NVIC中断。
    }
    return result;
}

static void TDrvDmaChSetSource(TDrvDmaCh *self, UInt8 source)
{
    (void)self;
    (void)source;
    // TODO: 写入目标MCU的DMA请求复用器。
}

Int32 TDrvDmaChCreate(TDrvDmaCh *self, TDrvDma *dma, UInt8 channel, UInt8 source)
{
    Int32 result = kHccDrvSocCommonSuccessLine;
    if (self == NULL || dma == NULL || channel >= DMA_CH_MAX)
        result = kHccDrvSocCommonParamLine;
    else if (g_dma_channels_[channel] != NULL)
        result = kHccDrvSocCommonOpenedLine;
    else
    {
        self->cross_tag_ = kDrvMemCrossTagCreate;
        self->dma_ = dma;
        self->channel_ = channel;
        self->source_ = source;
        self->receiver = NULL;
        self->OnComplete = NULL;
        self->OnError = NULL;
        self->is_open_ = False;
        g_dma_channels_[channel] = self;
    }
    return result;
}

Int32 TDrvDmaChDestroy(TDrvDmaCh *self)
{
    Int32 result = kHccDrvSocCommonSuccessLine;
    if (self == NULL)
        result = kHccDrvSocCommonParamLine;
    else
    {
        if (self->is_open_)
            TDrvDmaChClose(self);
        g_dma_channels_[self->channel_] = NULL;
        self->cross_tag_ = kDrvMemCrossTagDestroy;
    }
    return result;
}

Int32 TDrvDmaChOpen(TDrvDmaCh *self)
{
    Int32 result = kHccDrvSocCommonSuccessLine;
    if (self == NULL || self->dma_ == NULL)
        result = kHccDrvSocCommonParamLine;
    else if (!self->dma_->is_open_)
        result = kHccDrvSocCommonNotReadyLine;
    else if (self->is_open_)
        result = kHccDrvSocCommonOpenedLine;
    else
    {
        TDrvDmaChSetSource(self, self->source_);
        // TODO: 禁用触发/循环等默认未使用能力，清标志并使能完成和错误中断。
        self->cross_tag_ = kDrvMemCrossTagOpen;
        self->is_open_ = True;
        TDrvDmaChSetIrqEnable(self, True);
    }
    return result;
}

Int32 TDrvDmaChClose(TDrvDmaCh *self)
{
    Int32 result = kHccDrvSocCommonSuccessLine;
    if (self == NULL)
        result = kHccDrvSocCommonParamLine;
    else if (!self->is_open_)
        result = kHccDrvSocCommonClosedLine;
    else
    {
        TDrvDmaChDisableRequest(self);
        TDrvDmaChSetIrqEnable(self, False);
        TDrvDmaChSetSource(self, 0);
        // TODO: 禁用通道中断，清除Pending。
        self->cross_tag_ = kDrvMemCrossTagClose;
        self->is_open_ = False;
    }
    return result;
}

static Int32 TDrvDmaChSetTransfer(TDrvDmaCh *self, UInt32 periph_addr, UInt8 transfer_size, Bool read)
{
    Int32 result = kHccDrvSocCommonSuccessLine;
    if (self == NULL || periph_addr == 0 || transfer_size >= kDmaTransferSizeMax)
        result = kHccDrvSocCommonParamLine;
    else if (!self->is_open_)
        result = kHccDrvSocCommonClosedLine;
    else
    {
        (void)read;
        // TODO: 配置外设地址固定、内存地址递增及目标MCU对应的传输宽度。
    }
    return result;
}

Int32 TDrvDmaChSetAsReadPeriph(TDrvDmaCh *self, UInt32 periph_addr, UInt8 transfer_size)
{
    return TDrvDmaChSetTransfer(self, periph_addr, transfer_size, True);
}

Int32 TDrvDmaChSetAsWritePeriph(TDrvDmaCh *self, UInt32 periph_addr, UInt8 transfer_size)
{
    return TDrvDmaChSetTransfer(self, periph_addr, transfer_size, False);
}

Int32 TDrvDmaChSetAsCustom(TDrvDmaCh *self, Int32 src_ofs, Int32 dst_ofs, UInt8 src_type, UInt8 dst_type)
{
    Int32 result = kHccDrvSocCommonSuccessLine;
    if (self == NULL || src_type >= kDmaTransferSizeMax || dst_type >= kDmaTransferSizeMax)
        result = kHccDrvSocCommonParamLine;
    else if (!self->is_open_)
        result = kHccDrvSocCommonClosedLine;
    else
    {
        (void)src_ofs;
        (void)dst_ofs;
        // TODO: 配置源/目的偏移与宽度。
    }
    return result;
}

Int32 TDrvDmaChSetSrcAddr(TDrvDmaCh *self, Int32 addr)
{
    Int32 result = kHccDrvSocCommonSuccessLine;
    if (self == NULL || addr == 0)
        result = kHccDrvSocCommonParamLine;
    else if (!self->is_open_)
        result = kHccDrvSocCommonClosedLine;
    else
    {
        // TODO: 写入DMA源地址。
    }
    return result;
}

Int32 TDrvDmaChSetDstAddr(TDrvDmaCh *self, Int32 addr)
{
    Int32 result = kHccDrvSocCommonSuccessLine;
    if (self == NULL || addr == 0)
        result = kHccDrvSocCommonParamLine;
    else if (!self->is_open_)
        result = kHccDrvSocCommonClosedLine;
    else
    {
        // TODO: 写入DMA目的地址。
    }
    return result;
}

Int32 TDrvDmaChSetMajorLoopCount(TDrvDmaCh *self, Int32 count)
{
    Int32 result = kHccDrvSocCommonSuccessLine;
    if (self == NULL || count <= 0)
        result = kHccDrvSocCommonParamLine;
    else if (!self->is_open_)
        result = kHccDrvSocCommonClosedLine;
    else
    {
        // TODO: 按目标MCU的计数单位写入主循环次数和循环地址边界。
    }
    return result;
}

Int32 TDrvDmaChGetMajorLoopCount(TDrvDmaCh *self, UInt32 *count, UInt32 *remain)
{
    Int32 result = kHccDrvSocCommonSuccessLine;
    if (self == NULL || (count == NULL && remain == NULL))
        result = kHccDrvSocCommonParamLine;
    else if (!self->is_open_)
        result = kHccDrvSocCommonClosedLine;
    else
    {
        // TODO: 读取目标MCU总传输量和剩余传输量。
        if (count != NULL)
            *count = 0;
        if (remain != NULL)
            *remain = 0;
        result = kHccDrvSocCommonNotSupportLine;
    }
    return result;
}

Int32 TDrvDmaChEnableRequest(TDrvDmaCh *self)
{
    Int32 result = kHccDrvSocCommonSuccessLine;
    if (self == NULL)
        result = kHccDrvSocCommonParamLine;
    else if (!self->is_open_)
        result = kHccDrvSocCommonClosedLine;
    else
    {
        // TODO: 清状态后使能DMA请求。
    }
    return result;
}

Int32 TDrvDmaChDisableRequest(TDrvDmaCh *self)
{
    Int32 result = kHccDrvSocCommonSuccessLine;
    if (self == NULL)
        result = kHccDrvSocCommonParamLine;
    else if (!self->is_open_)
        result = kHccDrvSocCommonClosedLine;
    else
    {
        // TODO: 禁用DMA请求，并确认停止。
    }
    return result;
}

Int32 TDrvDmaCreate(TDrvDma *self)
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

Int32 TDrvDmaDestroy(TDrvDma *self)
{
    Int32 result = kHccDrvSocCommonSuccessLine;
    if (self == NULL)
        result = kHccDrvSocCommonParamLine;
    else
    {
        if (self->is_open_)
            TDrvDmaClose(self);
        for (Int32 i = DMA_CH_MAX - 1; i >= 0; i--)
        {
            if (g_dma_channels_[i] != NULL)
                TDrvDmaChDestroy(g_dma_channels_[i]);
        }
        self->cross_tag_ = kDrvMemCrossTagDestroy;
    }
    return result;
}

Int32 TDrvDmaOpen(TDrvDma *self)
{
    Int32 result = kHccDrvSocCommonSuccessLine;
    if (self == NULL)
        result = kHccDrvSocCommonParamLine;
    else if (self->is_open_)
        result = kHccDrvSocCommonOpenedLine;
    else
    {
        // TODO: 使能DMA时钟、复位控制器并清除全局状态。
        self->cross_tag_ = kDrvMemCrossTagOpen;
        self->is_open_ = True;
    }
    return result;
}

Int32 TDrvDmaClose(TDrvDma *self)
{
    Int32 result = kHccDrvSocCommonSuccessLine;
    if (self == NULL)
        result = kHccDrvSocCommonParamLine;
    else if (!self->is_open_)
        result = kHccDrvSocCommonClosedLine;
    else
    {
        for (Int32 i = 0; i < DMA_CH_MAX; i++)
        {
            if (g_dma_channels_[i] != NULL && g_dma_channels_[i]->is_open_)
                TDrvDmaChClose(g_dma_channels_[i]);
        }
        // TODO: 关闭DMA中断和接口时钟。
        self->cross_tag_ = kDrvMemCrossTagClose;
        self->is_open_ = False;
    }
    return result;
}
