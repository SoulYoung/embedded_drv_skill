/**
 * @file drv_reset.h
 * @brief 芯片复位驱动层
 * @note

 * @par 修改日志
 * <table>
 * <tr><th>Date             <th>Version         <th>Author              <th>Description
 * <tr><td>2025-12-10       <td>1.0             <td>软件工程院           <td>创建初始版本
 * @copyright 杭州鸿泉物联网技术股份有限公司
 */
#ifndef _DRV_RESET_H_
#define _DRV_RESET_H_

#include <ofc_object.h>
#include <ofc_types.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @par 示例
 * @code
   TDrvResetCreate(&g_drv_reset);
   TDrvResetOpen(&g_drv_reset);
   ...
   UInt32 reason;
   TDrvResetGetReason(&g_drv_reset, kDrvResetTypeChip, &reason);
   if ((reason & kDrvResetChipReasonWdog) != 0)
   {
      // To Do Something
   }
   ...
   TDrvResetClose(&g_drv_reset);
   TDrvResetDestroy(&g_drv_reset);
 * @endcode
*/

/// 复位类型
typedef enum
{
    kDrvResetTypeBase   = 0,
    kDrvResetTypeChip   = kDrvResetTypeBase,    ///< 芯片复位
    kDrvResetTypeMax,
} TDrvResetType;

/// 芯片复位的原因
/// @note TODO: 按目标MCU复位状态寄存器重新定义具体原因位。
typedef enum
{
    kDrvResetChipReasonPor          = 1U << 16,     ///< 上电时复位
    kDrvResetChipReasonLvr          = 1U << 17,     ///< 低电压复位
    kDrvResetChipReasonSw           = 1U << 18,     ///< 软件复位, 指用户设置RAM内核 SYSRESETREQ bit 为1, 产生的复位
    kDrvResetChipReasonLockUp       = 1U << 19,     ///< 内核死锁复位
    kDrvResetChipReasonAckError     = 1U << 20,     ///< 休眠模式下应答错误复位
    kDrvResetChipReasonWdog         = 1U << 21,     ///< 看门狗复位
    kDrvResetChipReasonXoscLoss     = 1U << 23,     ///< 晶体振荡器故障复位
    kDrvResetChipReasonPllUnlock    = 1U << 24,     ///< PLL失锁复位
    kDrvResetChipReasonVhsiLoss     = 1U << 25,     ///< VHSI丢失复位
    kDrvResetChipReasonExt          = 1U << 26,     ///< 外部引脚复位
    kDrvResetChipReasonSmu          = 1U << 27,     ///< SMU错误复位
    kDrvResetChipReasonEcc2         = 1U << 28,     ///< ECC 2bit错误复位
} TDrvResetChipReason;

/// 产生的复位模式
typedef enum
{
    TDrvResetTriggerBase        = 0,
    TDrvResetTriggerChip        = TDrvResetTriggerBase,  ///< 芯片系统复位
    TDrvResetTriggerMax,
} TDrvResetTriggerMode;

typedef struct TDrvResetTag TDrvReset;
struct TDrvResetTag
{
PRIVATE
    UInt32 cross_tag_;              ///< 越界标记
    UInt32 reason;                  ///< 复位原因
    Bool is_open;                   ///< 是否打开
};

extern TDrvReset g_drv_reset; ///< 复位驱动对象

/// --------------------------------- 必要接口 start ---------------------------------
/**
 * 创建设备驱动
 * @param self: 对象实例指针
 * @return 当返回值 >= 0 时, 表示成功 \n
 *         当返回值 < 0 时, 表示失败, 参见[drv_error_code](@ref TDrvErrorCodeTag)
 */
Int32 TDrvResetCreate(TDrvReset *self);

/**
 * 析构设备驱动
 * @param self: 对象实例指针
 * @return 当返回值 >= 0 时, 表示成功 \n
 *         当返回值 < 0 时, 表示失败, 参见[drv_error_code](@ref TDrvErrorCodeTag)
 */
Int32 TDrvResetDestroy(TDrvReset *self);

/**
 * 初始化整个复位系统
 * @param self: 对象实例指针
 * @return 当返回值 >= 0 时, 表示成功 \n
 *         当返回值 < 0 时, 表示失败, 参见[drv_error_code](@ref TDrvErrorCodeTag)
 */
Int32 TDrvResetOpen(TDrvReset *self);

/**
 * 反初始化整个复位系统
 * @param self: 对象实例指针
 * @return 当返回值 >= 0 时, 表示成功 \n
 *         当返回值 < 0 时, 表示失败, 参见[drv_error_code](@ref TDrvErrorCodeTag)
 */
Int32 TDrvResetClose(TDrvReset *self);

/**
 * 获取复位原因
 * @param self: 对象实例指针
 * @param type: 复位类型
 * @param reason: 复位类型, 不同的位表示不同的原因
 * @return 当返回值 >= 0 时, 表示成功 \n
 *         当返回值 < 0 时, 表示失败, 参见[drv_error_code](@ref TDrvErrorCodeTag)
 */
Int32 TDrvResetGetReason(TDrvReset *self, TDrvResetType type, UInt32 *reason);

/**
 * 清除复位标志
 * @param self: 对象实例指针
 * @return 当返回值 >= 0 时, 表示成功 \n
 *         当返回值 < 0 时, 表示失败, 参见[drv_error_code](@ref TDrvErrorCodeTag)
 */
Int32 TDrvResetClrFlags(TDrvReset *self, TDrvResetType type);

/**
 * 触发复位
 * @param self: 对象实例指针
 * @param mode: 复位模式
 * @return 当返回值 >= 0 时, 表示成功 \n
 *         当返回值 < 0 时, 表示失败, 参见[drv_error_code](@ref TDrvErrorCodeTag)
 */
Int32 TDrvResetTrigger(TDrvReset *self, TDrvResetTriggerMode mode);
/// --------------------------------- 必要接口 end ---------------------------------

/// --------------------------------- 特殊接口 start ---------------------------------
/// --------------------------------- 特殊接口 end ---------------------------------

#ifdef __cplusplus
}  // extern "C"
#endif

#endif // _DRV_RESET_H_

