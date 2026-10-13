/**
 * @file drv_xcu_spi.c
 * @brief XCU SPI从机协议驱动参考实现
 * @note 本文件保留XCU通讯协议、乒乓缓冲与DMA回调收敛流程；
 *       所有与具体MCU寄存器、时钟、引脚复用相关的操作均以TODO标注。
 */
#include "drv_xcu_spi.h"
#include "drv_common.h"

enum
{
    kDrvXcuSpiItemIdle = 0,       ///< 项空闲，可以写入数据
    kDrvXcuSpiItemCommit,         ///< 项已提交，等待发起传输
    kDrvXcuSpiItemBusy,           ///< 项正在收发
    kDrvXcuSpiItemFinished,       ///< 项收发完成，接收数据有效
    kDrvXcuSpiItemError,          ///< 项收发完成，但发生错误
    kDrvXcuSpiTag = 0x5AU,        ///< 协议包起始标签
    kDrvXcuSpiTagCont = 0x5BU,    ///< 主控制器仍有数据待发送
    kDrvXcuSpiTile = 0x5B5CU,     ///< 协议包结束标签
    kDrvXcuSpiItemMask = kDrvXcuSpiItemMax - 1U,
    kDrvXcuSpiDefaultTimeout = 50U,
};

/**
 * 设置到主控制器的传输请求信号。
 * @note TODO: 使用目标平台GPIO驱动，将请求引脚输出为active对应电平。
 */
static void TDrvXcuSpiSetRequest(const TDrvXcuSpi *self, Bool active)
{
    (void)self;
    (void)active;
    // TODO: 按cfg_->req_group、req_pin和req_level配置请求GPIO输出电平。
}

/**
 * 初始化SPI及请求引脚。
 * @note TODO: 配置CLK、SOUT、SIN、CS复用和输入输出属性；请求引脚应输出非活动电平。
 */
static void TDrvXcuSpiInitPort(const TDrvXcuSpi *self)
{
    (void)self;
    // TODO: 调用目标平台PORT/GPIO接口完成复用、方向和默认电平配置。
}

/**
 * 释放SPI及请求引脚。
 * @note TODO: 先撤销请求信号，再将SPI和请求引脚恢复为默认安全状态。
 */
static void TDrvXcuSpiDonePort(const TDrvXcuSpi *self)
{
    TDrvXcuSpiSetRequest(self, False);
    // TODO: 使用目标平台PORT/GPIO驱动恢复引脚默认状态。
}

/**
 * 初始化SPI从机硬件。
 * @note TODO: 关闭模块后配置从机模式、CPOL/CPHA、帧宽、CS选择、FIFO水位和DMA请求；
 *       清除遗留状态并在配置完成后使能SPI。具体寄存器序列必须以目标芯片手册为准。
 */
static void TDrvXcuSpiInitHw(const TDrvXcuSpi *self)
{
    (void)self;
    // TODO: 根据目标芯片SPI寄存器模型完成从机初始化。
}

/**
 * 停止当前传输。
 * @note TODO: 禁止SPI向DMA发起请求，并按手册停止或清空FIFO。
 */
static void TDrvXcuSpiStopTransfer(TDrvXcuSpi *self)
{
    TDrvDmaChDisableRequest(&self->dma_ch_tx_);
    TDrvDmaChDisableRequest(&self->dma_ch_rx_);
    TDrvTimerItemStop(self->timer_);
    TDrvXcuSpiSetRequest(self, False);
    // TODO: 关闭目标SPI外设的TX/RX DMA请求并处理FIFO。
}

/** 初始化协议状态和乒乓缓冲项。本函数仅在线程上下文调用。 */
static void TDrvXcuSpiInitMember(TDrvXcuSpi *self)
{
    Int32 i;

    self->finished_index_ = 0U;
    self->write_index_ = 0U;
    self->read_index_ = 0U;
    self->tx_completed_ = False;
    self->rx_completed_ = False;
    self->error_ = False;
    self->timeout_ = False;
    self->busy_ = False;
    for (i = 0; i < kDrvXcuSpiItemMax; i++)
    {
        self->items_[i].write_pos = 0U;
        self->items_[i].read_pos = 0U;
        self->items_[i].state = kDrvXcuSpiItemIdle;
        self->items_[i].reserved = 0U;
    }
}

/**
 * 启动一个完整协议包的DMA收发。
 * @note 调用顺序不可改变：先准备TX DMA，再准备RX DMA，再使能SPI DMA请求，
 *       随后标记项为Busy、启动超时定时器，最后通过请求脚通知主控制器。
 */
static void TDrvXcuSpiTransfer(TDrvXcuSpi *self, TDrvXcuSpiItem *item)
{
    UInt32 frames = (UInt32)sizeof(TDrvXcuSpiData) / ((UInt32)self->cfg_->frame_size >> 3U);

    self->tx_completed_ = False;
    self->rx_completed_ = False;
    self->error_ = False;
    self->timeout_ = False;
    self->busy_ = True;
    TDrvDmaChSetSrcAddr(&self->dma_ch_tx_, (Int32)(UInt32)&item->tx);
    TDrvDmaChSetMajorLoopCount(&self->dma_ch_tx_, (Int32)frames);
    TDrvDmaChEnableRequest(&self->dma_ch_tx_);
    TDrvDmaChSetDstAddr(&self->dma_ch_rx_, (Int32)(UInt32)&item->rx);
    TDrvDmaChSetMajorLoopCount(&self->dma_ch_rx_, (Int32)frames);
    TDrvDmaChEnableRequest(&self->dma_ch_rx_);
    // TODO: 使能目标SPI的TX和RX DMA请求。
    item->state = kDrvXcuSpiItemBusy;
    TDrvTimerItemStart(self->timer_);
    TDrvXcuSpiSetRequest(self, True);
}

/**
 * 汇合TX与RX DMA完成事件，并处理一个完整协议包。
 * @note 本函数仅在中断上下文调用；OnComplete和OnError同样发生在中断上下文。
 */
static void TDrvXcuSpiComplete(TDrvXcuSpi *self)
{
    TDrvXcuSpiItem *item;
    TDrvXcuSpiData *rx;
    Bool server_req;

    if (self->tx_completed_ && self->rx_completed_)
    {
        item = &self->items_[self->read_index_];
        rx = &item->rx;
        self->tx_completed_ = False;
        self->rx_completed_ = False;
        TDrvTimerItemStop(self->timer_);
        TDrvXcuSpiSetRequest(self, False);
        if (self->error_ || rx->len > kDrvXcuSpiBufSize || rx->tile != kDrvXcuSpiTile
            || (rx->tag != kDrvXcuSpiTag && rx->tag != kDrvXcuSpiTagCont))
        {
            self->error_ = False;
            self->busy_ = False;
            item->state = kDrvXcuSpiItemError;
            self->read_index_ = (self->read_index_ + 1U) & kDrvXcuSpiItemMask;
            if (self->OnError != NULL)
                self->OnError(self, self->timeout_);
        }
        else
        {
            item->state = kDrvXcuSpiItemFinished;
            self->read_index_ = (self->read_index_ + 1U) & kDrvXcuSpiItemMask;
            item = &self->items_[self->read_index_];
            server_req = (rx->tag == kDrvXcuSpiTagCont);
            if (item->state == kDrvXcuSpiItemCommit)
            {
                server_req = False;
                TDrvXcuSpiTransfer(self, item);
            }
            else
                self->busy_ = False;
            if (self->OnComplete != NULL)
                self->OnComplete(self, server_req);
        }
    }
}

/** TX DMA完成回调。TODO: 在此先关闭目标SPI的TX DMA请求。 */
static void TDrvXcuSpiOnTxComplete(TDrvDmaCh *sender)
{
    TDrvXcuSpi *self = (TDrvXcuSpi *)sender->receiver;

    // TODO: 关闭目标SPI的TX DMA请求。
    self->tx_completed_ = True;
    TDrvXcuSpiComplete(self);
}

/** RX DMA完成回调。TODO: 在此先关闭目标SPI的RX DMA请求。 */
static void TDrvXcuSpiOnRxComplete(TDrvDmaCh *sender)
{
    TDrvXcuSpi *self = (TDrvXcuSpi *)sender->receiver;

    // TODO: 关闭目标SPI的RX DMA请求。
    self->rx_completed_ = True;
    TDrvXcuSpiComplete(self);
}

/** TX DMA错误回调；等待RX侧结束后由TDrvXcuSpiComplete统一收敛。 */
static void TDrvXcuSpiOnTxError(TDrvDmaCh *sender, UInt32 errors)
{
    TDrvXcuSpi *self = (TDrvXcuSpi *)sender->receiver;

    (void)errors;
    // TODO: 关闭目标SPI的TX DMA请求。
    self->error_ = True;
    self->tx_completed_ = True;
    TDrvXcuSpiComplete(self);
}

/** RX DMA错误回调；等待TX侧结束后由TDrvXcuSpiComplete统一收敛。 */
static void TDrvXcuSpiOnRxError(TDrvDmaCh *sender, UInt32 errors)
{
    TDrvXcuSpi *self = (TDrvXcuSpi *)sender->receiver;

    (void)errors;
    // TODO: 关闭目标SPI的RX DMA请求。
    self->error_ = True;
    self->rx_completed_ = True;
    TDrvXcuSpiComplete(self);
}

/** 传输超时回调：停止两路DMA后强制汇合为一个错误包。 */
static void TDrvXcuSpiOnTimer(TDrvTimerItem *sender)
{
    TDrvXcuSpi *self = (TDrvXcuSpi *)sender->receiver;

    TDrvXcuSpiStopTransfer(self);
    // TODO: 依目标芯片手册清空SPI收发FIFO，避免移位器残留数据。
    self->error_ = True;
    self->timeout_ = True;
    self->tx_completed_ = True;
    self->rx_completed_ = True;
    TDrvXcuSpiComplete(self);
    self->timeout_ = False;
}

/** 提交当前写项；临界区只保护状态、索引和busy判定。 */
static void TDrvXcuSpiCommitItem(TDrvXcuSpi *self, TDrvXcuSpiItem *item)
{
    Bool start = False;
    TDrvXcuSpiItem *transfer_item = NULL;

    item->tx.tag = kDrvXcuSpiTag;
    item->tx.len = item->write_pos;
    item->tx.tile = kDrvXcuSpiTile;
    item->rx.tag = 0U;
    item->rx.len = 0U;
    item->rx.tile = 0U;
    item->write_pos = 0U;
    item->read_pos = 0U;
    TDrvIrqDisableGlobal(&g_drv_irq);
    self->write_index_ = (self->write_index_ + 1U) & kDrvXcuSpiItemMask;
    item->state = kDrvXcuSpiItemCommit;
    if (!self->busy_)
    {
        transfer_item = &self->items_[self->read_index_];
        if (transfer_item->state == kDrvXcuSpiItemCommit)
        {
            self->busy_ = True;
            start = True;
        }
    }
    TDrvIrqEnableGlobal(&g_drv_irq);
    if (start)
        TDrvXcuSpiTransfer(self, transfer_item);
}

Int32 TDrvXcuSpiCreate(TDrvXcuSpi *self, const TDrvXcuSpiCfg *cfg)
{
    Int32 result = kHccDrvSocCommonSuccessLine;

    if (self == NULL || cfg == NULL || cfg->index >= SPI_INSTANCE_COUNT || cfg->cs_index > 3U
        || (cfg->frame_size != 8U && cfg->frame_size != 16U && cfg->frame_size != 32U))
        result = kHccDrvSocCommonParamLine;
    else
    {
        self->cross_tag_ = kDrvMemCrossTagCreate;
        self->receiver = NULL;
        self->OnComplete = NULL;
        self->OnError = NULL;
        self->cfg_ = cfg;
        self->cfg_ex_ = &g_drv_spi_base_cfg_ex_[cfg->index];
        self->timer_ = TDrvTimerSrvCreateTimer(&g_drv_timer_srv);
        if (self->timer_ == NULL)
            result = kHccDrvSocCommonOutOfRangeLine;
        else
        {
            self->is_open_ = False;
            TDrvXcuSpiInitMember(self);
            memset(&self->dma_ch_rx_, 0, sizeof(self->dma_ch_rx_));
            memset(&self->dma_ch_tx_, 0, sizeof(self->dma_ch_tx_));
            TDrvDmaChCreate(&self->dma_ch_rx_, &g_drv_dma, cfg->dma_ch_rx, cfg->dma_src_rx);
            TDrvDmaChCreate(&self->dma_ch_tx_, &g_drv_dma, cfg->dma_ch_tx, cfg->dma_src_tx);
            self->dma_ch_rx_.receiver = self;
            self->dma_ch_rx_.OnComplete = TDrvXcuSpiOnRxComplete;
            self->dma_ch_rx_.OnError = TDrvXcuSpiOnRxError;
            self->dma_ch_tx_.receiver = self;
            self->dma_ch_tx_.OnComplete = TDrvXcuSpiOnTxComplete;
            self->dma_ch_tx_.OnError = TDrvXcuSpiOnTxError;
            self->timer_->receiver = self;
            self->timer_->OnTimer = TDrvXcuSpiOnTimer;
            TDrvTimerItemSetInterval(self->timer_, kDrvXcuSpiDefaultTimeout);
            result = kHccDrvSocCommonSuccessLine;
        }
    }
    DRV_LOG_RESULT_ERROR("xcu_spi", result);
    return result;
}

Int32 TDrvXcuSpiDestroy(TDrvXcuSpi *self)
{
    Int32 result = kHccDrvSocCommonSuccessLine;

    if (self == NULL)
        result = kHccDrvSocCommonParamLine;
    else if (self->is_open_)
        result = kHccDrvSocCommonOpenedLine;
    else
    {
        TDrvDmaChDestroy(&self->dma_ch_tx_);
        TDrvDmaChDestroy(&self->dma_ch_rx_);
        self->cross_tag_ = kDrvMemCrossTagDestroy;
        result = kHccDrvSocCommonSuccessLine;
    }
    DRV_LOG_RESULT_ERROR("xcu_spi", result);
    return result;
}

Int32 TDrvXcuSpiSetTimeout(TDrvXcuSpi *self, UInt32 timeout)
{
    Int32 result = kHccDrvSocCommonSuccessLine;

    if (self == NULL || timeout == 0U)
        result = kHccDrvSocCommonParamLine;
    else
    {
        TDrvTimerItemSetInterval(self->timer_, timeout);
        result = kHccDrvSocCommonSuccessLine;
    }
    DRV_LOG_RESULT_ERROR("xcu_spi", result);
    return result;
}

Int32 TDrvXcuSpiOpen(TDrvXcuSpi *self)
{
    Int32 result = kHccDrvSocCommonSuccessLine;

    if (self == NULL)
        result = kHccDrvSocCommonParamLine;
    else if (self->is_open_)
        result = kHccDrvSocCommonOpenedLine;
    else
    {
        TDrvXcuSpiInitMember(self);
        // TODO: 使用cfg_ex_->iclk_src、fclk_src和rclk_src完成目标SPI时钟使能与复位。
        TDrvXcuSpiInitPort(self);
        TDrvXcuSpiInitHw(self);
        result = TDrvDmaChOpen(&self->dma_ch_rx_);
        if (result >= kHccDrvSocCommonSuccessLine)
            result = TDrvDmaChOpen(&self->dma_ch_tx_);
        if (result >= kHccDrvSocCommonSuccessLine)
        {
            // TODO: 将DMA RX/TX外设地址分别设置为目标SPI的数据寄存器，并按frame_size设置传输宽度；失败时回收已打开资源并返回该错误码。
            self->cross_tag_ = kDrvMemCrossTagOpen;
            self->is_open_ = True;
        }
    }
    DRV_LOG_RESULT_ERROR("xcu_spi", result);
    return result;
}

Int32 TDrvXcuSpiClose(TDrvXcuSpi *self)
{
    Int32 result = kHccDrvSocCommonSuccessLine;

    if (self == NULL)
        result = kHccDrvSocCommonParamLine;
    else if (!self->is_open_)
        result = kHccDrvSocCommonClosedLine;
    else
    {
        TDrvXcuSpiStopTransfer(self);
        // TODO: 关闭目标SPI并撤销SPI时钟；如手册要求，应先执行模块软复位。
        TDrvDmaChClose(&self->dma_ch_tx_);
        TDrvDmaChClose(&self->dma_ch_rx_);
        TDrvXcuSpiDonePort(self);
        self->is_open_ = False;
        self->cross_tag_ = kDrvMemCrossTagClose;
        result = kHccDrvSocCommonSuccessLine;
    }
    DRV_LOG_RESULT_ERROR("xcu_spi", result);
    return result;
}

Bool TDrvXcuSpiReadable(TDrvXcuSpi *self)
{
    Bool result = False;
    TDrvXcuSpiItem *item;

    if (self != NULL)
    {
        item = &self->items_[self->finished_index_];
        while (item->state == kDrvXcuSpiItemError
               || (item->state == kDrvXcuSpiItemFinished && item->rx.len == 0U))
        {
            item->state = kDrvXcuSpiItemIdle;
            self->finished_index_ = (self->finished_index_ + 1U) & kDrvXcuSpiItemMask;
            item = &self->items_[self->finished_index_];
        }
        result = (item->state == kDrvXcuSpiItemFinished);
    }
    return result;
}

Int32 TDrvXcuSpiRead(TDrvXcuSpi *self, void *buf, Int32 size)
{
    Int32 result = 0;
    TDrvXcuSpiItem *item;
    Byte *data;

    if (self != NULL && buf != NULL && size > 0)
    {
        data = (Byte *)buf;
        item = &self->items_[self->finished_index_];
        while (size > 0 && (item->state == kDrvXcuSpiItemFinished || item->state == kDrvXcuSpiItemError))
        {
            if (item->state == kDrvXcuSpiItemError)
            {
                item->state = kDrvXcuSpiItemIdle;
                self->finished_index_ = (self->finished_index_ + 1U) & kDrvXcuSpiItemMask;
                item = &self->items_[self->finished_index_];
            }
            else
            {
                Int32 count = (Int32)item->rx.len - (Int32)item->read_pos;

                if (count > size)
                    count = size;
                if (count > 0)
                {
                    memcpy(data, &item->rx.data[item->read_pos], (UInt32)count);
                    item->read_pos += (Byte)count;
                    data += count;
                    size -= count;
                    result += count;
                }
                if (item->read_pos >= item->rx.len)
                {
                    item->state = kDrvXcuSpiItemIdle;
                    self->finished_index_ = (self->finished_index_ + 1U) & kDrvXcuSpiItemMask;
                    item = &self->items_[self->finished_index_];
                }
            }
        }
    }
    return result;
}

Int32 TDrvXcuSpiFreeSize(TDrvXcuSpi *self)
{
    Int32 result = 0;

    if (self != NULL && self->items_[self->write_index_].state == kDrvXcuSpiItemIdle)
        result = (Int32)sizeof(self->items_[self->write_index_].tx.data) - self->items_[self->write_index_].write_pos;
    return result;
}

Int32 TDrvXcuSpiWrite(TDrvXcuSpi *self, const void *data, Int32 size, Int32 *free_size)
{
    Int32 result = 0;
    TDrvXcuSpiItem *item;

    if (self != NULL && data != NULL && size > 0)
    {
        item = &self->items_[self->write_index_];
        if (item->state == kDrvXcuSpiItemIdle)
        {
            result = (Int32)sizeof(item->tx.data) - item->write_pos;
            if (result > size)
                result = size;
            memcpy(&item->tx.data[item->write_pos], data, (UInt32)result);
            item->write_pos += (Byte)result;
            if (free_size != NULL)
                *free_size = (Int32)sizeof(item->tx.data) - item->write_pos;
        }
    }
    return result;
}

void TDrvXcuSpiCommit(TDrvXcuSpi *self)
{
    if (self != NULL && self->is_open_ && self->items_[self->write_index_].state == kDrvXcuSpiItemIdle)
        TDrvXcuSpiCommitItem(self, &self->items_[self->write_index_]);
}

Bool TDrvXcuSpiReset(TDrvXcuSpi *self, Bool force)
{
    Bool result = False;

    if (self != NULL && self->is_open_)
    {
        TDrvXcuSpiSetRequest(self, False);
        // TODO: 读取目标SPI忙状态；force为False且仍忙时应返回False。
        if (force)
        {
            TDrvXcuSpiStopTransfer(self);
            // TODO: 清空目标SPI收发FIFO与状态标志。
            TDrvXcuSpiInitMember(self);
            result = True;
        }
    }
    return result;
}