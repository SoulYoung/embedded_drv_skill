/**
 * @file drv_xcu_spi_cfg.h
 * @brief xcu驱动配置 
 * @note 
 * @par 修改日志
 * <table>
 * <tr><th>Date                 <th>Version         <th>Author              <th>Description
 * <tr><td>2025-12-11           <td>1.0             <td>软件工程院           <td>创建初始版本
 * @copyright 杭州鸿泉物联网技术股份有限公司
 */

#ifndef _DRV_XCU_SPI_CFG_H_
#define _DRV_XCU_SPI_CFG_H_

#include <ofc_types.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct TDrvXcuSpiCfgTag TDrvXcuSpiCfg;
struct TDrvXcuSpiCfgTag
{
    UInt8 clk_group;                ///< 端口组编号, 参见[drv_port_def.h](@ref TDrvPortGroup)
    UInt8 clk_pin;                  ///< 引脚编号, 参见[drv_port_def.h](@ref TDrvPortPin)
    UInt8 clk_mux;                  ///< 复用功能
    
    UInt8 sout_group;               ///< 端口组编号, 参见[drv_port_def.h](@ref TDrvPortGroup)
    UInt8 sout_pin;                 ///< 引脚编号, 参见[drv_port_def.h](@ref TDrvPortPin)
    UInt8 sout_mux;                 ///< 复用功能

    UInt8 sin_group;                ///< 端口组编号, 参见[drv_port_def.h](@ref TDrvPortGroup)
    UInt8 sin_pin;                  ///< 引脚编号, 参见[drv_port_def.h](@ref TDrvPortPin)
    UInt8 sin_mux;                  ///< 复用功能

    UInt8 cs_group;                 ///< 端口组编号, 参见[drv_port_def.h](@ref TDrvPortGroup)
    UInt8 cs_pin;                   ///< 引脚编号, 参见[drv_port_def.h](@ref TDrvPortPin)
    UInt8 cs_mux;                   ///< 复用功能

    UInt8 req_group;                ///< 端口组编号, 参见[drv_port_def.h](@ref TDrvPortGroup)
    UInt8 req_pin;                  ///< 引脚编号, 参见[drv_port_def.h](@ref TDrvPortPin)
    UInt8 req_mux;                  ///< 复用功能
    Bool req_level;                 ///< 触发电平(True: 上升沿有效, False: 下降沿有效)

    UInt8 cs_index;                 ///< cs的索引,即cs0, cs1, cs2...
    UInt8 index;                    ///< 模块索引,范围[0-2]

    UInt8 dma_ch_rx;                ///< 用于接收的DMA通道号
    UInt8 dma_ch_tx;                ///< 用于发送的DMA通道号
    UInt8 dma_src_rx;               ///< 用于接收的DMA输入源 
    UInt8 dma_src_tx;               ///< 用于发送的DMA输入源 

    UInt8 cpol;                     ///< 设置SPI的CPOL, TBOX和AIDVR均为0
    UInt8 cpha;                     ///< 设置SPI的CPAH, TBOX为1,AIDVR为0
    UInt8 frame_size;               ///< 设置spi一帧的bit数; TBOX为32,AIDVR为8
};

extern const TDrvXcuSpiCfg g_drv_xcu_spi_cfg;

#ifdef __cplusplus
}  // extern "C"
#endif

#endif // _DRV_XCU_SPI_CFG_H_


