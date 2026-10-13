/**
 * @file drv_pmc.h
 * @brief 电源模式切换驱动层
 * @note 电源管理模块依赖于时钟模块, 因此需要在时钟稳定后进行调用

 * @par          修改日志
 * <table>
 * <tr><th>Date             <th>Version               <th>Author          <th>Description
 * <tr><td>2025-12-10       <td>1.0                   <td>软件工程院      <td>创建初始版本
 * @copyright 杭州鸿泉物联网技术股份有限公司
 */
#ifndef _DRV_PMC_H_
#define _DRV_PMC_H_

#include <ofc_object.h>
#include <ofc_types.h>

#ifdef __cplusplus
extern "C" {
#endif

/// 电源模式的枚举
enum TDrvPmcModeTag
{
    kDrvPmcModeBase                     = 0,                ///< 基址
    kDrvPmcModeRun                      = kDrvPmcModeBase,  ///< 运行模式
    kDrvPmcModeSleep                    = 1,                ///< 睡眠模式
    kDrvPmcModeVlps                     = 2,                ///< 低功耗停止模式，部分 MCU 对应 DeepSleep 模式
    kDrvPmcModeStandby                  = 3,                ///< 待机模式
    kDrvPmcModeMax,
};
typedef enum TDrvPmcModeTag TDrvPmcMode;

/**
 * 功耗模式类.
 * @par 示例
 * @code
    TDrvPmcCreate(&g_drv_pmc);
    TDrvPmcOpen(&g_drv_pmc);
    ...
    TDrvPmcSelectMode(&g_drv_pmc, kDrvPmcModeRun);
    ...
    TDrvPmcClose(&g_drv_pmc);
    TDrvPmcDestroy(&g_drv_pmc);
 * @endcode
*/
typedef struct TDrvPmcTag TDrvPmc;
struct TDrvPmcTag
{
PRIVATE
    UInt32 cross_tag_;      ///< 越界标记
    Bool is_open_;          ///< 打开标记
};

/// 电源模式切换对象
extern TDrvPmc g_drv_pmc;

/// --------------------------------- 必要接口 start ---------------------------------
/**
 * 构造函数
 * @param self: 对象实例指针
 * @return >= 0: 成功; < 0: 失败
 */
Int32 TDrvPmcCreate(TDrvPmc *self);

/**
 * 析构函数
 * @param self: 对象实例指针
 * @return >= 0: 成功; < 0: 失败
 */
Int32 TDrvPmcDestroy(TDrvPmc *self);

/**
 * 打开电源模式切换功能
 * @param self: 对象实例指针
 * @return >= 0: 成功; < 0: 失败
 */
Int32 TDrvPmcOpen(TDrvPmc *self);

/**
 * 关闭电源模式切换功能
 * @param self: 对象实例指针
 * @return >= 0: 成功; < 0: 失败
 */
Int32 TDrvPmcClose(TDrvPmc *self);

/**
 * 选择电源模式
 * @param self: 对象实例指针
 * @param mode: 工作模式, 参见[drv_pmc](@ref TDrvPmcModeTag)
 * @return >= 0: 成功; < 0: 失败
 */
Int32 TDrvPmcSelectMode(TDrvPmc *self, TDrvPmcMode mode);
/// --------------------------------- 必要接口 end ---------------------------------

/// --------------------------------- 特殊接口 start ---------------------------------
/// --------------------------------- 特殊接口 end ---------------------------------

#ifdef __cplusplus
}  // extern "C"
#endif

#endif ///< _DRV_PMC_H_

