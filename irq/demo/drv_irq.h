/**
 * @file drv_irq.h
 * @brief 中断驱动层
 * @note
 * @par 修改日志
 * <table>
 * <tr><th>Date        <th>Version               <th>Author          <th>Description
 * <tr><td>2026-08-21  <td>1.0                   <td>软件工程院      <td>创建初始版本
 * @copyright 杭州鸿泉物联网技术股份有限公司
 */
#ifndef _DRV_IRQ_H_
#define _DRV_IRQ_H_

#include <ofc_types.h>
#include <ofc_object.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct TDrvIrqTag TDrvIrq;
struct TDrvIrqTag
{
PRIVATE
    UInt32 cross_tag_;              ///< 越界标记
    Int8 global_ref_;               ///< 全局中断引用计数
    Bool is_open_;                  ///< 打开标记
    UInt8 reserved_[2];             ///< 对齐
};

extern TDrvIrq g_drv_irq;

/// --------------------------------- 必要接口 start ---------------------------------
/**
 * 驱动创建
 * @param self: 对象实例指针
 * @return 当返回值 >= 0 时, 表示成功 \n
 *         当返回值 < 0 时, 表示失败, 参见[hcc_error_code_drv_soc_common](@ref THccDrvSocCommonTag)
 */
Int32 TDrvIrqCreate(TDrvIrq *self);

/**
 * 驱动销毁
 * @param self: 对象实例指针
 * @return 当返回值 >= 0 时, 表示成功 \n
 *         当返回值 < 0 时, 表示失败, 参见[hcc_error_code_drv_soc_common](@ref THccDrvSocCommonTag)
 */
Int32 TDrvIrqDestroy(TDrvIrq *self);

/**
 * 驱动打开
 * @note 本函数要在依赖它的函数调用之前调用。
 * @param self: 对象实例指针
 * @return 当返回值 >= 0 时, 表示成功 \n
 *         当返回值 < 0 时, 表示失败, 参见[hcc_error_code_drv_soc_common](@ref THccDrvSocCommonTag)
 */
Int32 TDrvIrqOpen(TDrvIrq *self);

/**
 * 驱动关闭
 * @note 仅关闭驱动对象，不改变未平衡的全局中断引用计数。
 * @param self: 对象实例指针
 * @return 当返回值 >= 0 时, 表示成功 \n
 *         当返回值 < 0 时, 表示失败, 参见[hcc_error_code_drv_soc_common](@ref THccDrvSocCommonTag)
 */
Int32 TDrvIrqClose(TDrvIrq *self);

/**
 * 使能全局中断
 * @note 与 TDrvIrqDisableGlobal 配对使用，最后一次使能才恢复全局中断。
 * @param self: 对象实例指针
 * @return 当返回值 >= 0 时, 表示成功 \n
 *         当返回值 < 0 时, 表示失败, 参见[hcc_error_code_drv_soc_common](@ref THccDrvSocCommonTag)
 */
Int32 TDrvIrqEnableGlobal(TDrvIrq *self);

/**
 * 禁止全局中断
 * @note 支持嵌套调用，第一次禁止时关闭全局中断。
 * @param self: 对象实例指针
 * @return 当返回值 >= 0 时, 表示成功 \n
 *         当返回值 < 0 时, 表示失败, 参见[hcc_error_code_drv_soc_common](@ref THccDrvSocCommonTag)
 */
Int32 TDrvIrqDisableGlobal(TDrvIrq *self);

/**
 * 使能独立中断
 * @param self: 对象实例指针
 * @param num: 外设中断编号，参见芯片头文件 IRQn_Type
 * @return 当返回值 >= 0 时, 表示成功 \n
 *         当返回值 < 0 时, 表示失败, 参见[hcc_error_code_drv_soc_common](@ref THccDrvSocCommonTag)
 */
Int32 TDrvIrqEnable(TDrvIrq *self, UInt8 num);

/**
 * 禁止独立中断
 * @param self: 对象实例指针
 * @param num: 外设中断编号，参见芯片头文件 IRQn_Type
 * @return 当返回值 >= 0 时, 表示成功 \n
 *         当返回值 < 0 时, 表示失败, 参见[hcc_error_code_drv_soc_common](@ref THccDrvSocCommonTag)
 */
Int32 TDrvIrqDisable(TDrvIrq *self, UInt8 num);
/// --------------------------------- 必要接口 end ---------------------------------

/// --------------------------------- 特殊接口 start ---------------------------------
/// --------------------------------- 特殊接口 end ---------------------------------

#ifdef __cplusplus
}  // extern "C"
#endif

#endif // _DRV_IRQ_H_
