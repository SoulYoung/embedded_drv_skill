/**
 * @file drv_wdog.h
 * @brief 看门狗驱动程序
 * @note
 * @par          修改日志
 * <table>
 * <tr><th>Date           <th>Version               <th>Author          <th>Description
 * <tr><td>2025-12-11     <td>1.0                   <td>软件工程院       <td>创建初始版本
 * @copyright 杭州鸿泉物联网技术股份有限公司
 */
#ifndef _DRV_WDOG_H_
#define _DRV_WDOG_H_

#include <ofc_object.h>
#include <ofc_types.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * 构造一个看门狗对象
 * @note
 * @par 示例
 * @code
   TDrvWDogCreate(&g_drv_wdog, 0);
   TDrvWDogOpen(&g_drv_wdog, 2);
   while (1)
   {
       TDrvWDogFeed(&g_drv_wdog);
   }
   TDrvWDogClose(&g_drv_wdog);
   TDrvWDogDestroy(&g_drv_wdog);
 * @endcode
 */
typedef struct TDrvWDogTag TDrvWDog;
struct TDrvWDogCfgExTag;
typedef struct TDrvWDogCfgExTag TDrvWDogCfgEx;

struct TDrvWDogTag
{
PRIVATE
    UInt32 cross_tag_;              ///< 越界标记
    const TDrvWDogCfgEx *cfg_ex_;   ///< 内部配置
    Bool is_open_;                  ///< 打开标记
};

/// 看门狗对象
extern TDrvWDog g_drv_wdog;

/// --------------------------------- 必要接口 start ---------------------------------
/**
 * 驱动创建
 * @param self: 对象实例指针
 * @param index: 看门狗实例编号，由目标 MCU 确认
 * @return 当返回值 >= 0 时, 表示成功
 *         当返回值 < 0 时, 表示失败, 参见[THccDrvSocCommon](@ref THccDrvSocCommonTag)
 */
Int32 TDrvWDogCreate(TDrvWDog *self, UInt8 index);

/**
 * 析构设备驱动
 * @param self: 对象实例指针
 * @return 当返回值 >= 0 时, 表示成功
 *         当返回值 < 0 时, 表示失败, 参见[THccDrvSocCommon](@ref THccDrvSocCommonTag)
 */
Int32 TDrvWDogDestroy(TDrvWDog *self);
/**
 * 驱动打开
 * @param self: 对象实例指针
 * @param timeout: 看门狗超时时间，单位和取值范围由目标 MCU 用户手册确认
 * @return 当返回值 >= 0 时, 表示成功
 *         当返回值 < 0 时, 表示失败, 参见[THccDrvSocCommon](@ref THccDrvSocCommonTag)
 */
Int32 TDrvWDogOpen(TDrvWDog *self, UInt8 timeout);
/**
 * 驱动关闭
 * @param self: 对象实例指针
 * @return 当返回值 >= 0 时, 表示成功
 *         当返回值 < 0 时, 表示失败, 参见[THccDrvSocCommon](@ref THccDrvSocCommonTag)
 */
Int32 TDrvWDogClose(TDrvWDog *self);
/**
 * 喂看门狗
 * @param self: 对象实例
 * @return 当返回值 >= 0 时, 表示成功
 *         当返回值 < 0 时, 表示失败, 参见[THccDrvSocCommon](@ref THccDrvSocCommonTag)
 */
Int32 TDrvWDogFeed(TDrvWDog *self);
/**
 * 请求系统复位
 * @note 具体复位方式根据目标 MCU 和项目约定实现，可直接使用内核复位接口。
 * @param self: 对象实例
 * @return 当返回值 >= 0 时, 表示成功
 *         当返回值 < 0 时, 表示失败, 参见[THccDrvSocCommon](@ref THccDrvSocCommonTag)
 */
Int32 TDrvWDogReset(TDrvWDog *self);

/// --------------------------------- 必要接口 end ---------------------------------

/// --------------------------------- 特殊接口 start ---------------------------------
/// --------------------------------- 特殊接口 end ---------------------------------

#ifdef __cplusplus
}  // extern "C"
#endif

#endif  // _DRV_WDOG_H_
