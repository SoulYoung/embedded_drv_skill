/**
 * @file drv_wdog.c
 * @brief 看门狗驱动程序
 * @note 仅在TODO位置填写目标MCU寄存器、SDK和时钟配置。
 */
#include "drv_wdog.h"
#include "drv_common.h"

/// 看门狗时钟源
typedef enum
{
    kDrvWDogClkSrcLsi               = 0U,       ///< TODO: 目标 MCU 的低速内部时钟
    kDrvWDogClkSrcBus               = 1U,       ///< TODO: 目标 MCU 的总线时钟
    kDrvWDogClkSrcHsi               = 2U,       ///< TODO: 目标 MCU 的高速内部时钟
    kDrvWDogClkSrcHse               = 3U,       ///< TODO: 目标 MCU 的高速外部时钟
} TDrvWDogClkSrc;

/// 工作模式，用于指示看门狗在低功耗和调试模式下是否继续运行
struct TDrvWDogOpModeTag
{
    Bool stop;                      ///< TODO: True 表示低功耗模式不停止，False 表示停止
    Bool debug;                     ///< TODO: True 表示调试模式下继续运行，False 表示停止
};
typedef struct TDrvWDogOpModeTag TDrvWDogOpMode;

struct TDrvWDogCfgExTag
{
    void *base;                    ///< TODO: 替换为目标 MCU 看门狗寄存器基址类型
    UInt16 win_val;                ///< TODO: 窗口值
    UInt8 clk_src;                 ///< TODO: 时钟源，参见 TDrvWDogClkSrc
    TDrvWDogOpMode op_mode;        ///< TODO: 低功耗和调试模式配置
    Bool update_enable;            ///< TODO: 是否允许启动后再次配置
    Bool win_enable;               ///< TODO: 是否使能窗口喂狗
    Bool pre_enable;               ///< TODO: 是否使能预分频
};

static const TDrvWDogCfgEx g_drv_wdog_cfg_ex_[] =
{
    {
        .base = NULL,              ///< TODO: 填写看门狗基址和默认配置
        .win_val = 0U,
        .clk_src = kDrvWDogClkSrcLsi,
        .op_mode =
        {
            .stop = False,
            .debug = False,
        },
        .update_enable = True,
        .win_enable = False,
        .pre_enable = True,
    },
};

TDrvWDog g_drv_wdog;               ///< 看门狗定时器驱动对象

/**
 * 解锁看门狗, 在更新看门狗寄存器前需要解锁
 * @param self: 对象实例指针
 */
static void TDrvWDogUnlock(TDrvWDog *self)
{
    (void)self;
    /* TODO: 按目标 MCU 手册写入解锁序列，并等待寄存器完成解锁。 */
}

Int32 TDrvWDogCreate(TDrvWDog *self, UInt8 index)
{
    Int32 result = kHccDrvSocCommonSuccessLine;
    if (self == NULL || index >= (sizeof(g_drv_wdog_cfg_ex_) / sizeof(g_drv_wdog_cfg_ex_[0])))
        result = kHccDrvSocCommonParamLine;
    else
    {
        self->cross_tag_ = kDrvMemCrossTagCreate;
        self->cfg_ex_ = &g_drv_wdog_cfg_ex_[index];
        self->is_open_ = False;
    }
    return result;
}

Int32 TDrvWDogDestroy(TDrvWDog *self)
{
    Int32 result = kHccDrvSocCommonSuccessLine;
    if (self == NULL)
        result = kHccDrvSocCommonParamLine;
    else
    {
        if (self->is_open_)
            TDrvWDogClose(self);
        self->cross_tag_ = kDrvMemCrossTagDestroy;
        self->cfg_ex_ = NULL;
        self->is_open_ = False;
    }
    return result;
}

Int32 TDrvWDogOpen(TDrvWDog *self, UInt8 timeout)
{
    Int32 result = kHccDrvSocCommonSuccessLine;
    if (self == NULL || timeout == 0U)
        result = kHccDrvSocCommonParamLine;
    else if (self->is_open_)
        result = kHccDrvSocCommonOpenedLine;
    else
    {
        /* TODO: 检查 timeout 范围，按手册的时钟、预分频和计数器宽度换算装载值。 */
        /* TODO: 使能并等待时钟稳定；按手册顺序解锁、配置和等待更新完成。 */
        /* TODO: 启动看门狗并确认启动后是否允许关闭或重新配置。 */
        result = kHccDrvSocCommonNotSupportLine;
    }
    return result;
}

Int32 TDrvWDogClose(TDrvWDog *self)
{
    Int32 result = kHccDrvSocCommonSuccessLine;
    if (self == NULL)
        result = kHccDrvSocCommonParamLine;
    else if (!self->is_open_)
        result = kHccDrvSocCommonClosedLine;
    else
    {
        /* TODO: 按手册确认硬件是否能停止；不能停止时仅关闭对象状态，不关闭共享时钟。 */
        self->cross_tag_ = kDrvMemCrossTagClose;
        self->is_open_ = False;
    }
    return result;
}

Int32 TDrvWDogFeed(TDrvWDog *self)
{
    Int32 result = kHccDrvSocCommonSuccessLine;
    if (self == NULL)
        result = kHccDrvSocCommonParamLine;
    else if (!self->is_open_)
        result = kHccDrvSocCommonClosedLine;
    else
    {
        /* TODO: 按手册写入刷新序列；窗口模式下确认喂狗时机和最小间隔。 */
        result = kHccDrvSocCommonNotSupportLine;
    }
    return result;
}

Int32 TDrvWDogReset(TDrvWDog *self)
{
    Int32 result = kHccDrvSocCommonSuccessLine;
    if (self == NULL)
        result = kHccDrvSocCommonParamLine;
    else
    {
        /* TODO: 根据项目约定调用内核系统复位接口或触发看门狗复位。 */
        result = kHccDrvSocCommonNotSupportLine;
    }
    return result;
}
