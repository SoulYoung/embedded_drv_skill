#include "drv_can.h"
#include "drv_common.h"
#include "drv_timer_srv.h"

enum TPreemptiveStateTag
{
    kPrmptStateIdle,
    kPrmptStateSending,
};

enum TDrvCanAbortTag
{
    kDrvCanAbortPtb = 0x01,             ///< 由主发送中止
    kDrvCanAbortStb = 0x02,             ///< 由从发送中止
};

/**
 * 获取当前模式下单个报文的数据大小。
 * @param self: 对象实例指针
 * @return 单个报文占用的字节数
 */
static UInt32 TDrvCanGetDataSize(TDrvCan *self)
{
    UInt32 size = 0;

    if (self->mode_ == CAN_MODE_CAN20)
        size = sizeof(TCanData);
    else if (self->mode_ == CAN_MODE_CANFD)
        size = sizeof(TCanfdData);

    return size;
}

/**
 * 执行发送操作。
 * @param self: 对象实例指针
 * @param mb_index: 发送邮箱索引
 * @param data: 发送的数据
 * @return 无返回值
 */
static void TDrvCanDoSend(TDrvCan *self, Int32 mb_index, void *data)
{
    (void)self;
    (void)mb_index;
    (void)data;
    // TODO: 按目标MCU报文RAM/邮箱格式写入ID、帧类型、长度和数据。
    // TODO: 按手册触发对应发送邮箱，并保证与中断状态机同步。
}

/**
 * 从队列中取出一个报文进行发送。
 * @param self: 对象实例指针
 * @return
 */
static void TDrvCanSend(TDrvCan *self)
{
    TDrvCanUnifiedData data;

    if (TOfRingBufRead(&self->tx_buf_, &data, TDrvCanGetDataSize(self)) > 0)
    {
        self->send_error_count_ = 0;
        self->sending_ = True;

        if (self->mode_ == CAN_MODE_CAN20)
            self->tx_data_id_ = data.can20.param.data_id;
        else if (self->mode_ == CAN_MODE_CANFD)
            self->tx_data_id_ = data.canfd.param.data_id;

        TDrvCanDoSend(self, 1, &data);
    }
}

static void TDrvCanSendPreemptive(TDrvCan *self)
{
    self->preemptive_send_error_count_ = 0;

    if (self->mode_ == CAN_MODE_CAN20)
        TDrvCanDoSend(self, 0, &self->preemptive_data_);
    else if (self->mode_ == CAN_MODE_CANFD)
        TDrvCanDoSend(self, 0, self->preemptive_fd_data_buf_);
}

/**
 * 发送CAN报文后的处理。
 * @param self: 对象实例
 * @param success: True-成功, False-失败
 * @return 无返回值
 */
static void TDrvCanOnSent(TDrvCan *self, Bool success)
{
    if (self->sending_)
    {
        self->sending_ = False;

        if (self->OnSent != NULL && self->tx_data_id_ != 0)
            self->OnSent(self, self->tx_data_id_, success);
    }

    if (!self->bus_off_ && TOfRingBufDataSize(&self->tx_buf_) > 0)
        TDrvCanSend(self);
}

/**
 * 发送优先报文后的处理。
 * @param self: 对象实例
 * @param success: True-成功, False-失败
 * @return 无返回值
 */
static void TDrvCanOnPreemptiveSent(TDrvCan *self, Bool success)
{
    UInt32 data_id = 0;

    if (self->preemptive_state_ == kPrmptStateSending)
    {
        if (self->mode_ == CAN_MODE_CAN20)
            data_id = self->preemptive_data_.param.data_id;
        else if (self->mode_ == CAN_MODE_CANFD && self->preemptive_fd_data_buf_ != NULL)
            data_id = self->preemptive_fd_data_buf_->param.data_id;

        // 先清，给OnSent接力发送的可能。
        self->preemptive_state_ = kPrmptStateIdle;

        if (self->OnSent != NULL && data_id != 0)
            self->OnSent(self, data_id, success);
    }
}

/**
 * 收到CAN报文后的处理。
 * @param self: 对象实例
 * @param mb_index: 接收邮箱或FIFO索引
 * @return 无返回值
 */
static void TDrvCanOnRecv(TDrvCan *self, Int32 mb_index)
{
    Bool recved = False;

    (void)mb_index;
    // TODO: 从目标MCU接收FIFO/邮箱读取完整报文，填写bus_index和接收时间戳，写入rx_buf_。
    // TODO: 按手册释放当前接收项，并循环处理仍待取出的报文。

    if (recved && self->OnRecv != NULL)
        self->OnRecv(self);
}

/** Bus-Off事件处理。 */
static void TDrvCanOnBusOff(TDrvCan *self)
{
    // 注: Bus Off后所有中断都会停止产生。
    if (self->OnBusOff != NULL)
    {
        if (!self->auto_recover_)
            self->bus_off_ = True;

        self->OnBusOff(self);
    }
    else if (!self->auto_recover_)
        TDrvCanBusOffRecover(self);
}

/**
 * 中断处理。
 * @param self: 对象实例
 * @return
 */
static void TDrvCanIRQHandler(TDrvCan *self)
{
    (void)self;
    // TODO: 读取并清除目标MCU CAN中断标志。
    // TODO: 接收完成时调用TDrvCanOnRecv；发送完成时调用TDrvCanOnSent/TDrvCanOnPreemptiveSent。
    // TODO: 对错误、发送中止和Bus-Off更新error_frame_count_、abort及bus_off_状态。
}

/** 波特率检测中断。 */
static void TDrvCanDetectIrqErrorAndBusOff(TDrvCan *self)
{
    (void)self;
    // TODO: 按目标MCU中断状态判定“收到有效报文”或“波特率不匹配”，并调用OnDetect。
}

/// 使能时钟
static Int32 TDrvCanEnableClock(TDrvCan *self)
{
    Int32 result = kHccDrvSocCommonSuccessLine;

    (void)self;
    // TODO: 使能CAN接口/功能时钟，并按需要产生控制器复位。
    return result;
}

/// 失能时钟
static void TDrvCanDisableClock(TDrvCan *self)
{
    (void)self;
    // TODO: 在确认控制器已停止后关闭CAN时钟。
}

static Int32 TDrvCanInitPort(TDrvCan *self)
{
    Int32 result = kHccDrvSocCommonSuccessLine;

    (void)self;
    // TODO: 配置CAN RX/TX及收发器待机控制引脚。
    return result;
}

static void TDrvCanDonePort(TDrvCan *self)
{
    (void)self;
    // TODO: 恢复或释放由CAN独占的端口配置。
}

static Int32 TDrvCanDoOpen(TDrvCan *self)
{
    Int32 result = kHccDrvSocCommonSuccessLine;

    (void)self;
    // TODO: 进入配置模式，设置CAN/CAN FD位时序、消息RAM、自动恢复和默认过滤器。
    return result;
}

static void TDrvCanEnableInterrupts(TDrvCan *self)
{
    (void)self;
    // TODO: 清除Pending，登记实例对应关系并使能目标MCU的CAN IRQ。
}

static void TDrvCanDisableInterrupts(TDrvCan *self)
{
    (void)self;
    // TODO: 禁用CAN中断和NVIC，并清除Pending。
}

static void TDrvCanDoClose(TDrvCan *self)
{
    (void)self;
    // TODO: 停止发送接收，退出工作模式并等待控制器复位/停止完成。
}

Int32 TDrvCanCreate(TDrvCan *self, const TDrvCanCfg *cfg, void *tx_buf, Int32 tx_buf_size, void *rx_buf, Int32 rx_buf_size)
{
    Int32 result = kHccDrvSocCommonSuccessLine;

    if (self == NULL || cfg == NULL || tx_buf == NULL || tx_buf_size <= 0 || rx_buf == NULL || rx_buf_size <= 0)
        result = kHccDrvSocCommonParamLine;
    else
    {
        self->cross_tag_ = kDrvMemCrossTagCreate;
        self->cfg_ = cfg;
        // TODO: 在目标平台适配区按 cfg->index 绑定私有 CAN 资源描述。
        self->cfg_ex_ = NULL;
        TOfRingBufCreate(&self->tx_buf_, tx_buf, tx_buf_size);
        TOfRingBufCreate(&self->rx_buf_, rx_buf, rx_buf_size);
        self->is_open_ = False;
        self->tx_data_id_ = 0;
        self->send_error_count_ = 0;
        self->preemptive_send_error_count_ = 0;
        self->sending_ = False;
        self->preemptive_state_ = kPrmptStateIdle;
        self->auto_recover_ = True;
        self->baud_rate_detect_mode_ = False;
        self->bus_off_ = False;
        self->filter_writeable_ = False;
        self->error_frame_count_ = 0;
        self->abort = 0;
        self->mode_ = CAN_MODE_CAN20;
        self->receiver = NULL;
        self->OnSent = NULL;
        self->OnRecv = NULL;
        self->OnBusOff = NULL;
        self->OnDetect = NULL;
        self->OnSetFilter = NULL;
        self->preemptive_fd_data_buf_ = NULL;

        if (!TDrvCanSetBaudRate(self, 250000))
            result = kHccDrvSocCommonParamLine;
    }
    return result;
}

Int32 TDrvCanDestroy(TDrvCan *self)
{
    Int32 result = kHccDrvSocCommonSuccessLine;

    if (self == NULL)
        result = kHccDrvSocCommonParamLine;
    else
    {
        if (self->is_open_)
            TDrvCanClose(self);
        OfDestroyObject((TOfObject *)&self->tx_buf_);
        OfDestroyObject((TOfObject *)&self->rx_buf_);
        self->cross_tag_ = kDrvMemCrossTagDestroy;
    }
    return result;
}

Int32 TDrvCanSetMode(TDrvCan *self, TCANMode mode)
{
    Int32 result = kHccDrvSocCommonSuccessLine;

    if (self == NULL || mode > CAN_MODE_CANFD)
        result = kHccDrvSocCommonParamLine;
    else if (self->is_open_)
        result = kHccDrvSocCommonOpenedLine;
    else
        self->mode_ = mode;
    return result;
}

Int32 TDrvCanOpen(TDrvCan *self)
{
    Int32 result = kHccDrvSocCommonSuccessLine;

    if (self == NULL)
        result = kHccDrvSocCommonParamLine;
    else if (self->is_open_)
        result = kHccDrvSocCommonOpenedLine;
    else if (self->mode_ == CAN_MODE_CANFD && self->preemptive_fd_data_buf_ == NULL)
        result = kHccDrvSocCommonCallBackLine;
    else
    {
        TOfRingBufClear(&self->tx_buf_);
        TOfRingBufClear(&self->rx_buf_);
        self->tx_data_id_ = 0;
        self->sending_ = False;
        self->preemptive_state_ = kPrmptStateIdle;
        self->bus_off_ = False;

        if (self->OnBusOff == NULL)
            self->auto_recover_ = True;

        result = TDrvCanEnableClock(self);
        if (result == kHccDrvSocCommonSuccessLine)
            result = TDrvCanInitPort(self);
        if (result == kHccDrvSocCommonSuccessLine)
            result = TDrvCanDoOpen(self);
        if (result == kHccDrvSocCommonSuccessLine)
        {
            self->filter_writeable_ = True;
            if (self->OnSetFilter != NULL)
                self->OnSetFilter(self);
            self->filter_writeable_ = False;
            TDrvCanEnableInterrupts(self);
            self->cross_tag_ = kDrvMemCrossTagOpen;
            self->is_open_ = True;
        }
    }
    return result;
}

Int32 TDrvCanClose(TDrvCan *self)
{
    Int32 result = kHccDrvSocCommonSuccessLine;

    if (self == NULL)
        result = kHccDrvSocCommonParamLine;
    else if (!self->is_open_)
        result = kHccDrvSocCommonClosedLine;
    else
    {
        TDrvCanDisableInterrupts(self);
        TDrvCanDoClose(self);
        TDrvCanDonePort(self);
        TDrvCanDisableClock(self);
        self->cross_tag_ = kDrvMemCrossTagClose;
        self->is_open_ = False;
    }
    return result;
}

Int32 TDrvCanRead(TDrvCan *self, void *buf, Int32 count)
{
    Int32 result = 0;

    if (self != NULL && buf != NULL && count > 0)
    {
        result = TOfRingBufDataSize(&self->rx_buf_) / TDrvCanGetDataSize(self);
        if (result > count)
            result = count;
        if (result > 0)
            TOfRingBufRead(&self->rx_buf_, buf, result * TDrvCanGetDataSize(self));
    }
    return result;
}

Int32 TDrvCanWrite(TDrvCan *self, const void *data, Int32 count)
{
    Int32 result = 0;

    if (self != NULL && data != NULL && count > 0 && self->is_open_ && !self->baud_rate_detect_mode_)
    {
        result = TOfRingBufFreeSize(&self->tx_buf_) / TDrvCanGetDataSize(self);
        if (result > count)
            result = count;
        if (result > 0)
        {
            TOfRingBufWrite(&self->tx_buf_, (void *)data, result * TDrvCanGetDataSize(self));
            if (!self->sending_ && !self->bus_off_)
                TDrvCanSend(self);
        }
    }
    return result;
}

Bool TDrvCanWritePreemptive(TDrvCan *self, const void *data)
{
    Bool result = False;

    if (self != NULL && data != NULL && self->is_open_ && !self->baud_rate_detect_mode_)
    {
        result = (self->preemptive_state_ == kPrmptStateIdle) && !self->bus_off_;
        if (result)
        {
            if (self->mode_ == CAN_MODE_CAN20)
                self->preemptive_data_ = *((TCanData *)data);
            else if (self->preemptive_fd_data_buf_ != NULL)
                *(self->preemptive_fd_data_buf_) = *((TCanfdData *)data);

            self->preemptive_state_ = kPrmptStateSending;
            TDrvCanSendPreemptive(self);
        }
    }
    return result;
}

UInt32 TDrvCanGetBaudRate(TDrvCan *self)
{
    UInt32 result = 0;

    if (self != NULL)
    {
        // TODO: 获取CAN实际功能时钟，按目标MCU位时序公式计算标称波特率。
    }
    return result;
}

Int32 TDrvCanfdGetBaudRate(TDrvCan *self, UInt32 *baudrate, UInt32 *fd_baudrate)
{
    Int32 result = kHccDrvSocCommonSuccessLine;

    if (self == NULL || baudrate == NULL || fd_baudrate == NULL)
        result = kHccDrvSocCommonParamLine;
    else
    {
        // TODO: 根据目标MCU实际CAN功能时钟和两组位时序计算波特率。
        *baudrate = 0;
        *fd_baudrate = 0;
        result = kHccDrvSocCommonNotSupportLine;
    }
    return result;
}

Bool TDrvCanSetBaudRate(TDrvCan *self, UInt32 baud_rate)
{
    Bool result = False;

    if (self != NULL && !self->is_open_ && baud_rate > 0)
    {
        // TODO: 按目标MCU时钟、预分频和时间段范围，计算并保存baud_rate_。
        result = True;
    }
    return result;
}

Bool TDrvCanfdSetBaudRate(TDrvCan *self, UInt32 baud_rate)
{
    Bool result = False;

    if (self != NULL && !self->is_open_ && self->mode_ == CAN_MODE_CANFD && baud_rate > 0)
    {
        // TODO: 按目标MCU CAN FD数据段位时序范围，计算并保存fd_baudrate_。
        result = True;
    }
    return result;
}

UInt32 TDrvCanSetBaudRateEx(TDrvCan *self, Byte div, Byte prop_seg, Byte phase_seg1, Byte phase_seg2)
{
    UInt32 result = 0;

    if (self != NULL && !self->is_open_ && div > 0 && prop_seg > 0 && phase_seg1 > 0 && phase_seg2 > 0)
    {
        self->baud_rate_.pres_div = div;
        self->baud_rate_.prop_seg = prop_seg;
        self->baud_rate_.pseg1 = phase_seg1;
        self->baud_rate_.pseg2 = phase_seg2;
        result = TDrvCanGetBaudRate(self);
    }
    return result;
}

UInt32 TDrvCanfdSetBaudRateEx(TDrvCan *self, Byte div, Byte prop_seg, Byte phase_seg1, Byte phase_seg2)
{
    UInt32 result = 0;

    if (self != NULL && !self->is_open_ && self->mode_ == CAN_MODE_CANFD && div > 0 && prop_seg > 0 && phase_seg1 > 0 && phase_seg2 > 0)
    {
        self->fd_baudrate_.pres_div = div;
        self->fd_baudrate_.prop_seg = prop_seg;
        self->fd_baudrate_.pseg1 = phase_seg1;
        self->fd_baudrate_.pseg2 = phase_seg2;
        // TODO: 根据实际CAN FD功能时钟计算数据段波特率。
    }
    return result;
}

Bool TDrvCanAutoRecover(TDrvCan *self)
{
    Bool result = False;

    if (self != NULL)
        result = self->auto_recover_;
    return result;
}

void TDrvCanSetAutoRecover(TDrvCan *self, Bool auto_recover)
{
    if (self != NULL && !self->is_open_)
        self->auto_recover_ = auto_recover;
}

void TDrvCanBusOffRecover(TDrvCan *self)
{
    if (self != NULL && self->is_open_ && !self->auto_recover_)
    {
        // TODO: 按手册执行手动Bus-Off恢复，并确认控制器已回到正常状态。
        self->bus_off_ = False;
    }
}

Byte TDrvCanBusNo(TDrvCan *self)
{
    Byte result = 0;

    if (self != NULL && self->cfg_ != NULL)
        result = self->cfg_->index;
    return result;
}

Bool TDrvCanDetectMode(TDrvCan *self)
{
    Bool result = False;

    if (self != NULL)
        result = self->baud_rate_detect_mode_;
    return result;
}

void TDrvCanSetDetectMode(TDrvCan *self, Bool detect)
{
    if (self != NULL && !self->is_open_)
        self->baud_rate_detect_mode_ = detect;
}

Bool TDrvCanBusOff(TDrvCan *self)
{
    Bool result = False;

    if (self != NULL)
        result = self->bus_off_;
    return result;
}

void TDrvCanAbortSend(TDrvCan *self)
{
    if (self != NULL && self->is_open_)
    {
        if (self->preemptive_state_ == kPrmptStateSending)
            self->abort |= kDrvCanAbortPtb;
        if (self->sending_)
            self->abort |= kDrvCanAbortStb;
        // TODO: 按手册向目标MCU发送邮箱写入中止请求。
    }
}

void TDrvCanClearSendFifo(TDrvCan *self)
{
    if (self != NULL && self->bus_off_)
    {
        // 清优先发送数据。
        if (self->preemptive_state_ != kPrmptStateIdle)
        {
            self->preemptive_state_ = kPrmptStateIdle;
        }

        // 清发送队列中的所有报文；目标实现应按原框架对data_id非零项补发失败通知。
        TOfRingBufClear(&self->tx_buf_);
    }
}

Bool TDrvCanSetFifoFilter(TDrvCan *self, Int32 index, UInt32 mask, UInt32 filter, Bool ide)
{
    Bool result = False;

    if (self != NULL && self->filter_writeable_ && index >= 0 && index < TDrvCanGetFifoFilterCount(self))
    {
        (void)mask;
        (void)filter;
        (void)ide;
        // TODO: 将通用mask/filter/ide语义转换为目标MCU硬件过滤表项。
        result = True;
    }
    return result;
}

Int32 TDrvCanGetFifoFilterCount(TDrvCan *self)
{
    Int32 result = kHccDrvSocCommonParamLine;

    if (self != NULL)
    {
        // TODO: 返回目标MCU当前控制器实际支持的FIFO过滤器数量。
        result = kHccDrvSocCommonNotSupportLine;
    }
    return result;
}

Int32 TDrvCanGetErrorFrameCount(TDrvCan *self)
{
    Int32 result = kHccDrvSocCommonParamLine;

    if (self != NULL)
        result = self->error_frame_count_;
    return result;
}
