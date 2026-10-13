/**
 * @file drv_adc_cfg.h
 * @brief ADC驱动配置 
 * @note 
 * @author 软件工程院
 * @date 2025-12-05
 * @version 1.0

 * @par          修改日志
 * <table>
 * <tr><th>Date             <th>Version         <th>Author          <th>Description
 * <tr><td>2025-12-05       <td>1.0             <td>软件工程院       <td>创建初始版本
 * @copyright 杭州鸿泉物联网技术股份有限公司
 */
#ifndef _DRV_ADC_CFG_H_
#define _DRV_ADC_CFG_H_

#include <ofc_types.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct TDrvAdcChCfgTag TDrvAdcChCfg;
struct TDrvAdcChCfgTag
{
    UInt8 group;                    ///< 端口组
    UInt8 pin;                      ///< 端口引脚
    UInt8 mux;                      ///< 端口复用功能
    UInt32 x;                       ///< 外部分压电阻，比例分子
    UInt32 y;                       ///< 外部分压电阻，比例分母，不能为0
    Int32 offset;                   ///< 偏移量，有正负之分
    UInt32 ref_vol;                 ///< 参考电压
    UInt8 spt;                      ///< 采样时间
    UInt8 index;                    ///< 通道号
};

typedef struct TDrvAdcCfgTag TDrvAdcCfg;
struct TDrvAdcCfgTag
{
    UInt8 index;                    ///< ADC索引
    UInt8 reserved[3];              ///< 对齐
};

#ifdef __cplusplus
}  ///< extern "C"
#endif

#endif  ///< _DRV_ADC_CFG_H_
