/**
 * @file drv_port.h
 * @brief 端口驱动层
 * @note 使用MUX功能时, 使用者需要清楚的了解各个引脚的复用功能
 *      切勿将同一功能复用到多个引脚(除非需要)
 * @par 修改日志
 * <table>
 * <tr><th>Date             <th>Version         <th>Author          <th>Description
 * <tr><td>2025-12-08       <td>1.0             <td>软件工程院       <td>创建初始版本
 * @copyright 杭州鸿泉物联网技术股份有限公司
 */
#ifndef _DRV_PORT_H_
#define _DRV_PORT_H_

#include <ofc_types.h>
#include <ofc_object.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * 端口组编号
 */
typedef enum 
{
    kDrvPortGroupA              = 0U,                ///< 端口 A
    kDrvPortGroupB              = 1U,                ///< 端口 B
    kDrvPortGroupC              = 2U,                ///< 端口 C
    kDrvPortGroupD              = 3U,                ///< 端口 D
    kDrvPortGroupE              = 4U,                ///< 端口 E
    kDrvPortGroupMax,
} TDrvPortGroup;

/**
 * 端口引脚编号
 */
typedef enum 
{
    kDrvPortPin0                = 0U,               ///< 编号 0
    kDrvPortPin1                = 1U,               ///< 编号 1
    kDrvPortPin2                = 2U,               ///< 编号 2
    kDrvPortPin3                = 3U,               ///< 编号 3
    kDrvPortPin4                = 4U,               ///< 编号 4
    kDrvPortPin5                = 5U,               ///< 编号 5
    kDrvPortPin6                = 6U,               ///< 编号 6
    kDrvPortPin7                = 7U,               ///< 编号 7
    kDrvPortPin8                = 8U,               ///< 编号 8
    kDrvPortPin9                = 9U,               ///< 编号 9
    kDrvPortPin10               = 10U,              ///< 编号 10
    kDrvPortPin11               = 11U,              ///< 编号 11
    kDrvPortPin12               = 12U,              ///< 编号 12
    kDrvPortPin13               = 13U,              ///< 编号 13
    kDrvPortPin14               = 14U,              ///< 编号 14
    kDrvPortPin15               = 15U,              ///< 编号 15
    kDrvPortPin16               = 16U,              ///< 编号 16
    kDrvPortPin17               = 17U,              ///< 编号 17
    kDrvPortPin18               = 18U,              ///< 编号 18
    kDrvPortPin19               = 19U,              ///< 编号 19
    kDrvPortPin20               = 20U,              ///< 编号 20
    kDrvPortPin21               = 21U,              ///< 编号 21
    kDrvPortPin22               = 22U,              ///< 编号 22
    kDrvPortPin23               = 23U,              ///< 编号 23
    kDrvPortPin24               = 24U,              ///< 编号 24
    kDrvPortPin25               = 25U,              ///< 编号 25
    kDrvPortPin26               = 26U,              ///< 编号 26
    kDrvPortPin27               = 27U,              ///< 编号 27
    kDrvPortPin28               = 28U,              ///< 编号 28
    kDrvPortPin29               = 29U,              ///< 编号 29
    kDrvPortPin30               = 30U,              ///< 编号 30
    kDrvPortPin31               = 31U,              ///< 编号 31
    kDrvPortPinMax,
} TDrvPortPin;

/**
 * 端口引脚输出功能的枚举
 */
typedef enum
{
    kDrvPortMuxDefault          = 0U,               ///< 引脚功能, 默认高阻或者模拟输入
    kDrvPortMuxGpio             = 1U,               ///< 引脚通用输入输出
    kDrvPortMuxAlt2             = 2U,               ///< 引脚功能2
    kDrvPortMuxAlt3             = 3U,               ///< 引脚功能3
    kDrvPortMuxAlt4             = 4U,               ///< 引脚功能4
    kDrvPortMuxAlt5             = 5U,               ///< 引脚功能5
    kDrvPortMuxAlt6             = 6U,               ///< 引脚功能6
    kDrvPortMuxAlt7             = 7U,               ///< 引脚功能7
    kDrvPortMuxAltMax,
} TDrvPortMux;

/**
 * 端口引脚拉功能的枚举
 */
typedef enum
{
    kDrvPortPullDisabled        = 0U,               ///< 引脚拉功能禁止
    kDrvPortPullDown            = 1U,               ///< 引脚下拉使能
    kDrvPortPullUp              = 2U,               ///< 引脚上拉使能
    kDrvPortPullMax,
} TDrvPortPull;

/**
 * 端口控制的基类.基类不能用于创建对象
 * @par 示例
 * @code
   TDrvPortCreate(&g_drv_port);
   ...
   TDrvPortSetMux(&g_drv_port, kDrvPortGroupA, kDrvPortPin15, kDrvPortMuxGpio);
   TDrvPortSetPullMode(&g_drv_port, kDrvPortGroupA, kDrvPortPin15, kDrvPortPullDisabled);
   ...
   // ALT模式对应设置
   ...
   TDrvPortSetOutputEnabled(&g_drv_port, kDrvPortGroupA, kDrvPortPin15, True);
   ...
   TDrvPortDestroy(&g_drv_port);
 * @endcode
*/
typedef struct TDrvPortTag TDrvPort;
struct TDrvPortTag
{
PRIVATE
    UInt32 cross_tag_;          ///< 越界标记
    Bool is_open_;              ///< 打开标记
    UInt8 reserved_[3];         ///< 对齐
};

extern TDrvPort g_drv_port;     ///< 端口驱动对象

/// --------------------------------- 必要接口 start ---------------------------------
/**
 * 驱动创建
 * @param self: 对象实例指针
 * @return 当返回值 >= 0 时, 表示成功 \n
 *         当返回值 < 0 时, 表示失败, 参见[hcc_error_code_drv_soc_common](@ref THccDrvSocCommonTag)
 */
Int32 TDrvPortCreate(TDrvPort *self);

/**
 * 驱动销毁
 * @param self: 对象实例指针
 * @return 当返回值 >= 0 时, 表示成功 \n
 *         当返回值 < 0 时, 表示失败, 参见[hcc_error_code_drv_soc_common](@ref THccDrvSocCommonTag)
 */
Int32 TDrvPortDestroy(TDrvPort *self);

/**
 * 驱动打开
 * @note 需要在TDrvIrqOpen之后调用
 * @param self: 对象实例指针
 * @return 当返回值 >= 0 时, 表示成功 \n
 *         当返回值 < 0 时, 表示失败, 参见[hcc_error_code_drv_soc_common](@ref THccDrvSocCommonTag)
 */
Int32 TDrvPortOpen(TDrvPort *self);

/**
 * 驱动关闭
 * @param self: 对象实例指针
 * @return 当返回值 >= 0 时, 表示成功 \n
 *         当返回值 < 0 时, 表示失败, 参见[hcc_error_code_drv_soc_common](@ref THccDrvSocCommonTag)
 */
Int32 TDrvPortClose(TDrvPort *self);

/**
 * Port设置复用功能
 * @param self: 对象实例指针
 * @param group: 端口组, 参见[drv_port](@ref TDrvPortGroup)
 * @param pin: 引脚号, 参见[drv_port](@ref TDrvPortPin)
 * @param mux: 复用功能类型, 参见[drv_port](@ref TDrvPortMux)
 * @return 当返回值 >= 0 时, 表示成功 \n
 *         当返回值 < 0 时, 表示失败, 参见[hcc_error_code_drv_soc_common](@ref THccDrvSocCommonTag)
 */
Int32 TDrvPortSetMux(TDrvPort *self, UInt8 group, UInt8 pin, UInt8 mux);

/**
 * Port设置输入缓存
 * @param self: 对象实例指针
 * @param group: 端口组, 参见[drv_port](@ref TDrvPortGroup)
 * @param pin: 引脚号, 参见[drv_port](@ref TDrvPortPin)
 * @param enable: True-使能输入, False-禁止输入
 * @return 当返回值 >= 0 时, 表示成功 \n
 *         当返回值 < 0 时, 表示失败, 参见[hcc_error_code_drv_soc_common](@ref THccDrvSocCommonTag)
 */
Int32 TDrvPortSetInputEnabled(TDrvPort *self, UInt8 group, UInt8 pin, Bool enable);

/**
 * Port设置输出缓存
 * @param self: 对象实例指针
 * @param group: 端口组, 参见[drv_port](@ref TDrvPortGroup)
 * @param pin: 引脚号, 参见[drv_port](@ref TDrvPortPin)
 * @param enable: True-使能输出, False-禁止输出
 * @return 当返回值 >= 0 时, 表示成功 \n
 *         当返回值 < 0 时, 表示失败, 参见[hcc_error_code_drv_soc_common](@ref THccDrvSocCommonTag)
 */
Int32 TDrvPortSetOutputEnabled(TDrvPort *self, UInt8 group, UInt8 pin, Bool enable);

/**
 * Port设置拉模式
 * @param self: 对象实例指针
 * @param group: 端口组, 参见[drv_port](@ref TDrvPortGroup)
 * @param pin: 引脚号, 参见[drv_port](@ref TDrvPortPin)
 * @param pull: 拉模式, 参见[drv_port](@ref TDrvPortPull)
 * @return 当返回值 >= 0 时, 表示成功 \n
 *         当返回值 < 0 时, 表示失败, 参见[hcc_error_code_drv_soc_common](@ref THccDrvSocCommonTag)
 */
Int32 TDrvPortSetPullMode(TDrvPort *self, UInt8 group, UInt8 pin, UInt8 pull);

/**
 * Port设备驱动开漏输出使能(引脚之间控制独立,无需锁定)
 * @param self: 对象实例指针
 * @param group: 端口组, 参见[drv_port](@ref TDrvPortGroup)
 * @param pin: 引脚号, 参见[drv_port](@ref TDrvPortPin)
 * @param enable: True-开漏输出, False-推挽输出, 默认推挽输出
 * @return 当返回值 >= 0 时, 表示成功 \n
 *         当返回值 < 0 时, 表示失败, 参见[hcc_error_code_drv_soc_common](@ref THccDrvSocCommonTag)
 */
Int32 TDrvPortSetOpenDrainEnable(TDrvPort *self, UInt8 group, Int32 pin, Bool enable);
/// --------------------------------- 必要接口 end ---------------------------------

/// --------------------------------- 特殊接口 start ---------------------------------
/// --------------------------------- 特殊接口 end ---------------------------------

#ifdef __cplusplus
}  // extern "C"
#endif

#endif // _DRV_PORT_H_

