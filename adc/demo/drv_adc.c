#include "drv_adc.h"
#include "drv_common.h"

////////////////////////////////////////////////////////////////////////////////
// ADC数据结构
enum
{
    kDrvAdcTimeout = 5000U,       ///< TODO: 按目标MCU最大转换时间确认
    kDrvAdcResolution = 12U,      ///< TODO: 按目标MCU分辨率确认
    kDrvAdcInstanceMax = 1U,      ///< TODO: 按目标MCU实例数量确认
};

struct TDrvAdcCfgExTag
{
    void *base;                   ///< TODO: 替换为目标MCU ADC寄存器基址类型
    UInt8 iclk_src;               ///< TODO: ADC接口时钟源
    UInt8 fclk_src;               ///< TODO: ADC功能时钟源
    UInt8 rclk_src;               ///< TODO: ADC复位时钟
};

static const TDrvAdcCfgEx g_drv_adc_cfg_ex_[kDrvAdcInstanceMax] =
{
    {
        .base = NULL,             ///< TODO: 填写ADC0基址和时钟/复位配置
        .iclk_src = 0U,
        .fclk_src = 0U,
        .rclk_src = 0U,
    },
};

////////////////////////////////////////////////////////////////
/// 模块内部函数
static Int32 TDrvAdcDoClose(TDrvAdc *self);

static Int32 TDrvAdcDoCreate(TDrvAdc *self, const TDrvAdcCfg *cfg)
{
    Int32 result = kHccDrvSocCommonSuccessLine;
    Bool status = False;

    if (self == NULL || cfg == NULL || cfg->index >= kDrvAdcInstanceMax)
        result = kHccDrvSocCommonParamLine;
    else
    {
        TDrvIrqDisableGlobal(&g_drv_irq);
        status = (0U == self->c_ref_);
        self->c_ref_++;
        TDrvIrqEnableGlobal(&g_drv_irq);
        if (status)
        {
            self->cross_tag_ = kDrvMemCrossTagCreate;
            self->cfg_ = cfg;
            self->cfg_ex_ = &g_drv_adc_cfg_ex_[cfg->index];
            self->o_ref_ = 0U;
        }
    }

    return result;
}

static void TDrvAdcDoDestroy(TDrvAdc *self)
{
    Bool status = False;

    TDrvIrqDisableGlobal(&g_drv_irq);
    if (self->c_ref_ > 0U)
    {
        self->c_ref_--;
        status = (0U == self->c_ref_);
    }
    TDrvIrqEnableGlobal(&g_drv_irq);
    if (status)
    {
        self->cross_tag_ = kDrvMemCrossTagDestroy;
        self->cfg_ = NULL;
        self->cfg_ex_ = NULL;
    }
}

/// 使能时钟
static void TDrvAdcEnableClock(TDrvAdc *self)
{
    (void)self;
    /* TODO: 使用g_drv_clock和cfg_ex_->iclk_src/fclk_src/rclk_src打开时钟并复位ADC。 */
}

/// 失能时钟
static void TDrvAdcDisableClock(TDrvAdc *self)
{
    (void)self;
    /* TODO: 按用户手册顺序关闭ADC、复位或关闭时钟。 */
}

static void TDrvAdcInitPort(TDrvAdcCh *self)
{
    (void)self;
    /* TODO: 使用ch_cfg_->group/pin/mux配置外部模拟引脚；定义内部通道规则。 */
}

static void TDrvAdcDonePort(TDrvAdcCh *self)
{
    (void)self;
    /* TODO: 恢复外部模拟引脚默认状态。 */
}

static Int32 TDrvAdcDoOpen(TDrvAdc *self)
{
    Int32 result = kHccDrvSocCommonSuccessLine;
    Bool status = False;

    TDrvIrqDisableGlobal(&g_drv_irq);
    status = (0U == self->o_ref_);
    self->o_ref_++;
    TDrvIrqEnableGlobal(&g_drv_irq);
    if (!status)
        result = kHccDrvSocCommonSuccessLine;
    else if (self->cfg_ == NULL || self->cfg_ex_ == NULL)
    {
        TDrvIrqDisableGlobal(&g_drv_irq);
        self->o_ref_--;
        TDrvIrqEnableGlobal(&g_drv_irq);
        result = kHccDrvSocCommonNotReadyLine;
    }
    else
    {
        TDrvAdcEnableClock(self);
        /* TODO: 配置ADC分频、分辨率、参考源、对齐、模式、标志和校准。 */
        /* TODO: 配置失败或校准超时时设置result并回滚时钟及o_ref_。 */
        if (result == kHccDrvSocCommonSuccessLine)
        {
            self->cross_tag_ = kDrvMemCrossTagOpen;
        }
    }

    return result;
}

static Int32 TDrvAdcDoClose(TDrvAdc *self)
{
    Int32 result = kHccDrvSocCommonSuccessLine;
    Bool status = False;

    TDrvIrqDisableGlobal(&g_drv_irq);
    if (self->o_ref_ > 0U)
    {
        self->o_ref_--;
        status = (0U == self->o_ref_);
    }
    else
    {
        result = kHccDrvSocCommonClosedLine;
    }
    TDrvIrqEnableGlobal(&g_drv_irq);
    if (result == kHccDrvSocCommonSuccessLine && status)
    {
        /* TODO: 停止转换，关闭中断/DMA，清除硬件状态。 */
        TDrvAdcDisableClock(self);
        self->cross_tag_ = kDrvMemCrossTagClose;
    }

    return result;
}

Int32 TDrvAdcCreate(TDrvAdc *self, const TDrvAdcCfg *cfg)
{
    Int32 result = kHccDrvSocCommonSuccessLine;

    if (self == NULL || cfg == NULL)
        result = kHccDrvSocCommonParamLine;
    else
    {
        result = TDrvAdcDoCreate(self, cfg);
    }

    return result;
}

Int32 TDrvAdcDestroy(TDrvAdc *self)
{
    Int32 result = kHccDrvSocCommonSuccessLine;

    if (self == NULL)
        result = kHccDrvSocCommonParamLine;
    else if (self->o_ref_ != 0U)
        result = kHccDrvSocCommonOpenedLine;
    else
    {
        TDrvAdcDoDestroy(self);
    }

    return result;
}

Int32 TDrvAdcOpen(TDrvAdc *self)
{
    Int32 result = kHccDrvSocCommonSuccessLine;

    if (self == NULL)
        result = kHccDrvSocCommonParamLine;
    else
    {
        result = TDrvAdcDoOpen(self);
    }

    return result;
}

Int32 TDrvAdcClose(TDrvAdc *self)
{
    Int32 result = kHccDrvSocCommonSuccessLine;

    if (self == NULL)
        result = kHccDrvSocCommonParamLine;
    else
    {
        result = TDrvAdcDoClose(self);
    }

    return result;
}

Int32 TDrvAdcChCreate(TDrvAdcCh *self, const TDrvAdcChCfg *ch_cfg, TDrvAdc *adc, const TDrvAdcCfg *cfg)
{
    Int32 result = kHccDrvSocCommonSuccessLine;

    if (self == NULL || adc == NULL || ch_cfg == NULL || cfg == NULL || ch_cfg->y == 0U)
        result = kHccDrvSocCommonParamLine;
    else
    {
        result = TDrvAdcDoCreate(adc, cfg);
        if (result == kHccDrvSocCommonSuccessLine)
        {
            self->cross_tag_ = kDrvMemCrossTagCreate;
            self->adc_ = adc;
            self->ch_cfg_ = ch_cfg;
            self->is_open_ = False;
        }
    }

    return result;
}

Int32 TDrvAdcChDestroy(TDrvAdcCh *self)
{
    Int32 result = kHccDrvSocCommonSuccessLine;

    if (self == NULL)
        result = kHccDrvSocCommonParamLine;
    else if (self->is_open_)
        result = kHccDrvSocCommonOpenedLine;
    else
    {
        TDrvAdcDoDestroy(self->adc_);
        self->cross_tag_ = kDrvMemCrossTagDestroy;
        self->ch_cfg_ = NULL;
        self->adc_ = NULL;
    }

    return result;
}

Int32 TDrvAdcChOpen(TDrvAdcCh *self)
{
    Int32 result = kHccDrvSocCommonSuccessLine;

    if (self == NULL || self->adc_ == NULL || self->ch_cfg_ == NULL)
        result = kHccDrvSocCommonParamLine;
    else if (self->is_open_)
        result = kHccDrvSocCommonOpenedLine;
    else
    {
        result = TDrvAdcDoOpen(self->adc_);
        if (result == kHccDrvSocCommonSuccessLine)
        {
            TDrvAdcInitPort(self);
            /* TODO: 检查index/spt，配置采样时间和转换序列。 */
            self->cross_tag_ = kDrvMemCrossTagOpen;
            self->is_open_ = True;
        }
    }

    return result;
}

Int32 TDrvAdcChClose(TDrvAdcCh *self)
{
    Int32 result = kHccDrvSocCommonSuccessLine;

    if (self == NULL || self->adc_ == NULL)
        result = kHccDrvSocCommonParamLine;
    else if (!self->is_open_)
        result = kHccDrvSocCommonClosedLine;
    else
    {
        TDrvAdcDonePort(self);
        result = TDrvAdcDoClose(self->adc_);
        if (result == kHccDrvSocCommonSuccessLine)
        {
            self->cross_tag_ = kDrvMemCrossTagClose;
            self->is_open_ = False;
        }
    }

    return result;
}

Int32 TDrvAdcChReadOrigValue(TDrvAdcCh *self, Int32 *value)
{
    Int32 result = kHccDrvSocCommonSuccessLine;

    if (self == NULL || value == NULL)
        result = kHccDrvSocCommonParamLine;
    else if (!self->is_open_)
        result = kHccDrvSocCommonClosedLine;
    else
    {
        (void)kDrvAdcTimeout;
        /* TODO: 选择ch_cfg_->index，清标志，启动转换，超时等待EOC，读取原始值。 */
        result = kHccDrvSocCommonNotSupportLine;
    }

    return result;
}

Int32 TDrvAdcChReadConvValue(TDrvAdcCh *self, Int32 *value)
{
    Int32 result = kHccDrvSocCommonSuccessLine;
    Int32 temp_value = 0;

    if (self == NULL || value == NULL || self->ch_cfg_ == NULL)
        result = kHccDrvSocCommonParamLine;
    else
    {
        result = TDrvAdcChReadOrigValue(self, &temp_value);
        if (result == kHccDrvSocCommonSuccessLine)
        {
            temp_value *= self->ch_cfg_->ref_vol;
            temp_value /= ((1 << kDrvAdcResolution) - 1);
            temp_value *= self->ch_cfg_->x;
            temp_value /= self->ch_cfg_->y;
            temp_value += self->ch_cfg_->offset;
            *value = temp_value;
        }
    }

    return result;
}
