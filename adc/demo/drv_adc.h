/**
 * @file drv_adc.h
 * @brief 模拟数字转换驱动程序
 * @note

 * @par          修改日志
 * <table>
 * <tr><th>Date        <th>Version               <th>Author          <th>Description
 * <tr><td>2025-12-06  <td>1.0                   <td>软件工程院      <td>创建初始版本
 * @copyright 杭州鸿泉物联网技术股份有限公司
 */

#ifndef _DRV_ADC_H_
#define _DRV_ADC_H_

#include <ofc_types.h>
#include <ofc_object.h>
#include "drv_adc_cfg.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct TDrvAdcCfgExTag TDrvAdcCfgEx;

/**
 * ADC采样时间.
 * @note TODO: 根据目标MCU用户手册调整枚举项及其寄存器映射。
 */
typedef enum
{
    TDrvAdcSampleTimeClkBase            = 0U,
    TDrvAdcSampleTimeClk5               = TDrvAdcSampleTimeClkBase,
    TDrvAdcSampleTimeClk10,
    TDrvAdcSampleTimeClk15,
    TDrvAdcSampleTimeClk23,
    TDrvAdcSampleTimeClk35,
    TDrvAdcSampleTimeClk45,
    TDrvAdcSampleTimeClk85,
    TDrvAdcSampleTimeClk185             = 7U,
    TDrvAdcSampleTimeClkMAX,
} TDrvAdcSampleTime;

typedef struct TDrvAdcTag TDrvAdc;
/**
 * 操作ADC通道的对象
 * @note
 * @par
 */
typedef struct TDrvAdcChTag TDrvAdcCh;
struct TDrvAdcChTag
{
PRIVATE
    UInt32 cross_tag_;                      ///< 越界标记
    TDrvAdc *adc_;                          ///< ADC管理对象
    const TDrvAdcChCfg *ch_cfg_;            ///< 通道的配置信息
    Bool is_open_;                          ///< 是否已经打开
PUBLIC
};

/// --------------------------------- 必要接口 start ---------------------------------
/**
 * ADC通道构造函数.
 * @note
 *  1. 一个ADC控制器可以有多个通道,每个通道对应一个电压信号值。
 *  2. 端口必须在Open前创建。
 * @param self: 对象实例指针
 * @param ch_cfg: 通道配置结构
 * @param adc: ADC对象
 * @param cfg: ADC配置项指针
 * @return 当返回值 >= 0 时, 表示成功；当返回值 < 0 时, 表示失败。
 */
Int32 TDrvAdcChCreate(TDrvAdcCh *self, const TDrvAdcChCfg *ch_cfg, TDrvAdc *adc, const TDrvAdcCfg *cfg);

/**
 * 析构函数.
 * @param self: 对象实例指针
 * @return 当返回值 >= 0 时, 表示成功；当返回值 < 0 时, 表示失败。
 */
Int32 TDrvAdcChDestroy(TDrvAdcCh *self);

/**
 * 打开ADC通道.
 * @param self: 对象实例指针
 * @return 当返回值 >= 0 时, 表示成功；当返回值 < 0 时, 表示失败。
 */
Int32 TDrvAdcChOpen(TDrvAdcCh *self);

/**
 * 关闭ADC通道.
 * @param self: 对象实例指针
 * @return 当返回值 >= 0 时, 表示成功；当返回值 < 0 时, 表示失败。
 */
Int32 TDrvAdcChClose(TDrvAdcCh *self);

/**
 * ADC通道读取的原始值.
 * @param self: 对象实例指针
 * @param value: ADC转换原始值
 * @return 当返回值 >= 0 时, 表示成功；当返回值 < 0 时, 表示失败。
 */
Int32 TDrvAdcChReadOrigValue(TDrvAdcCh *self, Int32 *value);

/**
 * ADC通道读取的转换值.
 * @param self: 对象实例指针
 * @param value: ADC转换值，单位由ref_vol约定，当前模板按mV处理
 * @return 当返回值 >= 0 时, 表示成功；当返回值 < 0 时, 表示失败。
 */
Int32 TDrvAdcChReadConvValue(TDrvAdcCh *self, Int32 *value);

/**
 * 构造一个ADC主控制器对象.
 * @note ADC控制器使用c_ref_管理构造引用，使用o_ref_管理打开引用。
 */
struct TDrvAdcTag
{
PRIVATE
    UInt32 cross_tag_;                      ///< 越界标记
    const TDrvAdcCfg *cfg_;                 ///< 内部配置
    const TDrvAdcCfgEx *cfg_ex_;            ///< 内部拓展配置
    UInt8 c_ref_;                           ///< 构造函数的引用计数
    UInt8 o_ref_;                           ///< 打开函数的引用计数
};

/**
 * 驱动类的构造函数.
 * @param self: 对象实例指针
 * @param cfg: 配置项指针
 * @return 当返回值 >= 0 时, 表示成功；当返回值 < 0 时, 表示失败。
 */
Int32 TDrvAdcCreate(TDrvAdc *self, const TDrvAdcCfg *cfg);

/**
 * 析构函数.
 * @param self: 对象实例指针
 * @return 当返回值 >= 0 时, 表示成功；当返回值 < 0 时, 表示失败。
 */
Int32 TDrvAdcDestroy(TDrvAdc *self);

/**
 * 驱动类的打开.
 * @param self: 对象实例指针
 * @return 当返回值 >= 0 时, 表示成功；当返回值 < 0 时, 表示失败。
 */
Int32 TDrvAdcOpen(TDrvAdc *self);

/**
 * 驱动类的关闭.
 * @param self: 对象实例指针
 * @return 当返回值 >= 0 时, 表示成功；当返回值 < 0 时, 表示失败。
 */
Int32 TDrvAdcClose(TDrvAdc *self);

/// --------------------------------- 必要接口 end ---------------------------------

/// --------------------------------- 特殊接口 start ---------------------------------
/// --------------------------------- 特殊接口 end ---------------------------------

#ifdef __cplusplus
}  ///< extern "C"
#endif

#endif  ///< _DRV_ADC_H_
