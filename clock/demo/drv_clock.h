/**
 * @file drv_clock.h
 * @brief 时钟驱动层
 * @note
 * @par 修改日志
 * <table>
 * <tr><th>Date             <th>Version         <th>Author          <th>Description
 * <tr><td>2025-12-08       <td>1.0             <td>软件工程院       <td>创建初始版本
 * @copyright 杭州鸿泉物联网技术股份有限公司
 */
#ifndef _DRV_CLOCK_H_
#define _DRV_CLOCK_H_

#include <ofc_types.h>
#include <ofc_object.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * 时钟源(提供者)
 * @note TODO: 按目标MCU用户手册补齐所有系统、总线、分频和低速时钟源枚举。
 */
typedef enum
{
    kDrvClockSrcVhsi                        = 0U,       ///< 内部超高速时钟
    kDrvClockSrcHse                         = 1U,       ///< 外部振荡器时钟
    kDrvClockSrcHsi                         = 2U,       ///< 内部高速时钟
    kDrvClockSrcSpll                        = 3U,       ///< 系统锁相环时钟
    kDrvClockSrcSys                         = 4U,       ///< 系统时钟
    kDrvClockSrcBus                         = 5U,       ///< 外设总线时钟
    kDrvClockSrcFlash                       = 6U,       ///< Flash时钟
    kDrvClockSrcMAX,
} TDrvClockSrc;

/**
 * 时钟输出名称
 * @note TODO: 按目标MCU的CLKOUT/时钟输出选择器补齐枚举。
 */
typedef enum
{
    /// TODO: 按目标MCU时钟输出选择器补齐枚举。
    kDrvClockRunOutSys                      = 0U,       ///< 系统时钟输出
    kDrvClockRunOutBus,                                 ///< 总线时钟输出
    kDrvClockRunOutMAX,
} TDrvClockRunOut;

/**
 * 外设时钟名称
 * @note TODO: 按目标MCU外设时钟选择器补齐枚举。
 **/
typedef enum
{
    kDrvClockIClkDma0                     = 0,      ///< DMA0 clock
    kDrvClockIClkMax,                               ///< Maximum value
} TDrvClockIClk;

/**
 * 外设功能时钟名称
 * @note TODO: 按目标MCU外设时钟选择器补齐枚举。
 **/
typedef enum
{
    kDrvClockFClkDma0                     = 0,      ///< DMA0 clock
    kDrvClockFClkMax,                               ///< Maximum value
} TDrvClockFClk;

/**
 * 外设复位标识
 * @note TODO: 按目标MCU复位控制器定义补齐枚举。
 */
typedef enum
{
    kDrvClockRcmIdBase                      = 0U,       ///< 复位标识起始值
    kDrvClockRcmIdMAX,
} TDrvClockRcmIdType;

/**
 * 构造一个时钟管理控制对象
 * @par 示例
 * @code
   TDrvClockCreate(&g_drv_clock);
   ...
   // 确保先调用 TDrvIrqOpen(&g_drv_irq);
   TDrvClockOpen(&g_drv_clock);
   ...
   TDrvClockSetPeriphIClk(&g_drv_clock, iclk, True);
   TDrvClockSetPeriphFClk(&g_drv_clock, fclk, src, True);
   TDrvClockSoftResetPeriph(&g_drv_clock, reset_id);
   ...
   TDrvClockClose(&g_drv_clock);
   TDrvClockDestroy(&g_drv_clock);
 * @endcode
 */
typedef struct TDrvClockTag TDrvClock;
struct TDrvClockTag
{
PRIVATE
    UInt32 cross_tag_;              ///< 越界标记
    Bool is_open_;                  ///< 打开标记
    UInt8 reserved_[3];             ///< 对齐
};

/// 时钟管理对象
extern TDrvClock g_drv_clock;

/// --------------------------------- 必要接口 start ---------------------------------
/**
 * 创建驱动
 * @param self: 对象实例指针
 * @return 当返回值 >= 0 时, 表示成功 
 *         当返回值 < 0 时, 表示失败, 参见[hcc_error_code_drv_soc_common](@ref THccDrvSocCommonTag)
 */
Int32 TDrvClockCreate(TDrvClock *self);

/**
 * 销毁驱动
 * @param self: 对象实例指针
 * @return 当返回值 >= 0 时, 表示成功 
 *         当返回值 < 0 时, 表示失败, 参见[hcc_error_code_drv_soc_common](@ref THccDrvSocCommonTag)
 */
Int32 TDrvClockDestroy(TDrvClock *self);

/**
 * 初始化整个系统时钟
 * @note 需要在TDrvIrqOpen之后调用
 * @param self: 对象实例指针
 * @return 当返回值 >= 0 时, 表示成功 
 *         当返回值 < 0 时, 表示失败, 参见[hcc_error_code_drv_soc_common](@ref THccDrvSocCommonTag)
 */
Int32 TDrvClockOpen(TDrvClock *self);

/**
 * 反初始化整个系统时钟
 * @param self: 对象实例指针
 * @return 当返回值 >= 0 时, 表示成功 
 *         当返回值 < 0 时, 表示失败, 参见[hcc_error_code_drv_soc_common](@ref THccDrvSocCommonTag)
 */
Int32 TDrvClockClose(TDrvClock *self);

/**
 * 获取时钟源频率
 * @note 作为时钟源提供系统时钟
 * @param self: 对象实例指针
 * @param name: 时钟源名称, 参见[drv_clock](@ref TDrvClockSrc)
 * @param freq: 频率值
 * @return 当返回值 >= 0 时, 表示成功 
 *         当返回值 < 0 时, 表示失败, 参见[hcc_error_code_drv_soc_common](@ref THccDrvSocCommonTag)
 */
Int32 TDrvClockGetSrcFreq(TDrvClock *self, UInt8 name, UInt32 *freq);

/**
 * 将时钟输出到CLKOUT引脚
 * @note CLKOUT对应的引脚由PORT模块设置。
 * @param self: 对象实例指针
 * @param name: 输出时钟的名称, 参见[drv_clock](@ref TDrvClockRunOut)
 * @param enable: True使能输出, False禁止输出
 * @return 当返回值 >= 0 时, 表示成功 
 *         当返回值 < 0 时, 表示失败, 参见[hcc_error_code_drv_soc_common](@ref THccDrvSocCommonTag)
 */
Int32 TDrvClockSetRunOutGate(TDrvClock *self, UInt8 name, Bool enable);

/**
 * 设置外设Interface/bus时钟
 * @note 外设时钟需要在TDrvClockOpen之后调用。
 * @param self: 对象实例指针
 * @param iclk: 外设时钟名称, 参见[drv_clock](@ref TDrvClockIClk)
 * @param enable: True使能, False禁止
 * @return 当返回值 >= 0 时, 表示成功 
 *         当返回值 < 0 时, 表示失败, 参见[hcc_error_code_drv_soc_common](@ref THccDrvSocCommonTag)
 */
Int32 TDrvClockSetPeriphIClk(TDrvClock *self, UInt8 iclk, Bool enable);

/**
 * 设置外设Function/core时钟
 * @note 外设时钟需要在TDrvClockOpen之后调用。
 * @param self: 对象实例指针
 * @param fclk: 外设功能时钟名称, 参见[drv_clock](@ref TDrvClockFClk)
 * @param src: 时钟源, 参见目标MCU时钟头文件
 * @param enable: True使能, False禁止
 * @return 当返回值 >= 0 时, 表示成功 
 *         当返回值 < 0 时, 表示失败, 参见[hcc_error_code_drv_soc_common](@ref THccDrvSocCommonTag)
 */
Int32 TDrvClockSetPeriphFClk(TDrvClock *self, UInt8 fclk, UInt32 src, Bool enable);

/**
 * 软复位外设寄存器
 * @param self: 对象实例指针
 * @param num: 外设编号, 参见(@ref TDrvClockRcmIdType)
 * @return 当返回值 >= 0 时, 表示成功 
 *         当返回值 < 0 时, 表示失败, 参见[hcc_error_code_drv_soc_common](@ref THccDrvSocCommonTag)
 */
Int32 TDrvClockSoftResetPeriph(TDrvClock *self, UInt8 num);
/// --------------------------------- 必要接口 end ---------------------------------

/// --------------------------------- 特殊接口 start ---------------------------------
/// --------------------------------- 特殊接口 end ---------------------------------

#ifdef __cplusplus
}  // extern "C"
#endif

#endif //_DRV_CLOCK_H_
