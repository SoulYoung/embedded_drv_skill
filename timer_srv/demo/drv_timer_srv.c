#include "drv_timer_srv.h"
#include "drv_common.h"

TDrvTimerSrv g_drv_timer_srv;
volatile UInt32 g_timer_srv_tick = 0;

static void TDrvTimerItemCreate(TDrvTimerItem *self)
{
    self->receiver = NULL;
    self->OnTimer = NULL;
    self->enabled_ = False;
    self->interval_ = 10;
    self->tick_count_ = 0;
}

UInt32 TDrvTimerItemInterval(TDrvTimerItem *self)
{
    UInt32 result = 0;
    if (self != NULL)
        result = self->interval_;
    return result;
}

void TDrvTimerItemSetInterval(TDrvTimerItem *self, UInt32 ms)
{
    if (self != NULL && ms > 0)
        self->interval_ = ms;
}

void TDrvTimerItemStart(TDrvTimerItem *self)
{
    if (self != NULL)
    {
        self->tick_count_ = 0;
        self->enabled_ = True;
    }
}

void TDrvTimerItemStop(TDrvTimerItem *self)
{
    if (self != NULL)
        self->enabled_ = False;
}

/** 由目标MCU定时器中断调用的1ms节拍处理。 */
void TDrvTimerSrvIrqHandler(void)
{
    TDrvTimerSrv *self = &g_drv_timer_srv;
    // TODO: 确认并清除目标MCU周期定时器中断标志。
    g_timer_srv_tick++;
    dmb();
    for (Int32 i = self->count_ - 1; i >= 0; i--)
    {
        TDrvTimerItem *timer = &self->timers_[i];
        if (timer->enabled_)
        {
            timer->tick_count_++;
            if (timer->tick_count_ >= timer->interval_)
            {
                if (timer->OnTimer != NULL)
                    timer->OnTimer(timer);
                timer->tick_count_ = 0;
            }
        }
    }
}

Int32 TDrvTimerSrvCreate(TDrvTimerSrv *self)
{
    Int32 result = kHccDrvSocCommonSuccessLine;
    if (self == NULL)
        result = kHccDrvSocCommonParamLine;
    else
    {
        self->cross_tag_ = kDrvMemCrossTagCreate;
        self->cfg_ = NULL;
        self->count_ = 0;
        self->index_ = 0;
        self->is_open_ = False;
        for (Int32 i = 0; i < MAX_TIMER_COUNT; i++)
            self->timers_[i].enabled_ = False;
    }
    return result;
}

Int32 TDrvTimerSrvDestroy(TDrvTimerSrv *self)
{
    Int32 result = kHccDrvSocCommonSuccessLine;
    if (self == NULL)
        result = kHccDrvSocCommonParamLine;
    else
    {
        if (self->is_open_)
            TDrvTimerSrvClose(self);
        self->cross_tag_ = kDrvMemCrossTagDestroy;
    }
    return result;
}

Int32 TDrvTimerSrvOpen(TDrvTimerSrv *self)
{
    Int32 result = kHccDrvSocCommonSuccessLine;
    if (self == NULL)
        result = kHccDrvSocCommonParamLine;
    else if (self->is_open_)
        result = kHccDrvSocCommonOpenedLine;
    else
    {
        // TODO: 使能定时器时钟，配置稳定1ms周期，清中断并使能IRQ。
        self->cross_tag_ = kDrvMemCrossTagOpen;
        self->is_open_ = True;
    }
    return result;
}

Int32 TDrvTimerSrvClose(TDrvTimerSrv *self)
{
    Int32 result = kHccDrvSocCommonSuccessLine;
    if (self == NULL)
        result = kHccDrvSocCommonParamLine;
    else if (!self->is_open_)
        result = kHccDrvSocCommonClosedLine;
    else
    {
        for (Int32 i = 0; i < MAX_TIMER_COUNT; i++)
            self->timers_[i].enabled_ = False;
        // TODO: 禁用定时器IRQ、停止计数器、清标志并关闭时钟。
        self->cross_tag_ = kDrvMemCrossTagClose;
        self->is_open_ = False;
    }
    return result;
}

TDrvTimerItem *TDrvTimerSrvCreateTimer(TDrvTimerSrv *self)
{
    TDrvTimerItem *result = NULL;
    if (self != NULL && self->index_ < MAX_TIMER_COUNT)
    {
        result = &self->timers_[self->index_];
        self->index_++;
        self->count_++;
        TDrvTimerItemCreate(result);
    }
    return result;
}

void DrvTimerSrvGetHighPerformanceCounter(UInt32 *tick, UInt32 *value, UInt32 *max_value)
{
    if (tick != NULL && value != NULL && max_value != NULL)
    {
        *tick = g_timer_srv_tick;
        *value = 0;
        *max_value = 0;
        // TODO: 连续读取tick和自由运行计数器，返回1ms内精确计数及最大值。
    }
}
