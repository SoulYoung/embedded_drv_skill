/**
 * @file drv_can_cfg.h
 * @brief CAN驱动配置 
 * @note 
 * @author 软件工程院
 * @date 2025-12-09
 * @version 1.0

 * @par 修改日志
 * <table>
 * <tr><th>Date             <th>Version         <th>Author          <th>Description
 * <tr><td>2025-12-09       <td>1.0             <td>软件工程院       <td>创建初始版本
 * @copyright 杭州鸿泉物联网技术股份有限公司
 */

#ifndef _DRV_CAN_CFG_H_
#define _DRV_CAN_CFG_H_

#include <ofc_types.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct TDrvCanCfgTag TDrvCanCfg;
struct TDrvCanCfgTag
{
    UInt8       rx_group;       ///< 接收引脚所在的端口
    UInt8       tx_group;       ///< 发送引脚所在的端口
    UInt8       stb_group;      ///< 收发芯片standby模式控制引脚端口
    UInt8       rx_pin;         ///< 接收引脚编号
    UInt8       tx_pin;         ///< 发送引脚编号
    UInt8       stb_pin;        ///< 收发芯片standby模式控制引脚编号
    UInt8       rx_pin_mux;     ///< rx引脚复用功能
    UInt8       tx_pin_mux;     ///< tx引脚复用功能
    UInt8       id;             ///< CAN控制器ID, 用于接受数据时的bus_index成员的赋值
    UInt8       index;          ///< CAN控制器索引，实际有效范围及 CAN FD 能力由目标平台资料确认
};

#ifdef __cplusplus
}  // extern "C"
#endif

#endif // _DRV_CAN_CFG_H_


