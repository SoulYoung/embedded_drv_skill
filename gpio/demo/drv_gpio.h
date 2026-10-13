/**
 * @file drv_gpio.h
 * @brief 通用输入输出驱动层
 * @note

 * @par 修改日志
 * <table>
 * <tr><th>Date             <th>Version         <th>Author              <th>Description
 * <tr><td>2025-12-08       <td>1.0             <td>软件工程院           <td>从drv_port.h中剥离而来
 * @copyright 杭州鸿泉物联网技术股份有限公司
 */
#ifndef _DRV_GPIO_H_
#define _DRV_GPIO_H_

#include <ofc_types.h>
#include <ofc_object.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * 端口控制的基类.基类不能用于创建对象
 * @par 示例
 * @code
   TDrvPortCreate(&g_drv_port);
   TDrvGpioCreate(&g_drv_gpio);
   ...
   TDrvPortSetMux(&g_drv_port, kDrvPortGroupC, kDrvPortPin3, kDrvPortMuxGpio);
   TDrvPortSetPullMode(&g_drv_port, kDrvPortGroupC, kDrvPortPin3, kDrvPortPullDisabled);
   TDrvPortSetOutputEnabled(&g_drv_port, kDrvPortGroupC, kDrvPortPin3, True);
   ...
   TDrvGpioSetHighLevel(&g_drv_gpio, kDrvPortGroupC, kDrvPortPin3);
   ...
   TDrvGpioSetLowLevel(&g_drv_gpio, kDrvPortGroupC, kDrvPortPin3);
   ...
   TDrvGpioDestroy(&g_drv_gpio);
   TDrvPortDestroy(&g_drv_port);
 * @endcode
*/
typedef struct TDrvGpioTag TDrvGpio;
struct TDrvGpioTag
{
PRIVATE
    UInt32 cross_tag_;      ///< 越界标记
    Bool is_open_;          ///< 打开标记
    UInt8 reserved_[3];     ///< 对齐
};

extern TDrvGpio g_drv_gpio; ///< 通用输入输出驱动对象

/// --------------------------------- 必要接口 start ---------------------------------
/**
 * 驱动创建
 * @param self: 对象实例指针
 * @return 当返回值 >= 0 时, 表示成功 \n
 *         当返回值 < 0 时, 表示失败, 参见[hcc_error_code_drv_soc_common](@ref THccDrvSocCommonTag)
 */
Int32 TDrvGpioCreate(TDrvGpio *self);

/**
 * 驱动销毁
 * @param self: 对象实例指针
 * @return 当返回值 >= 0 时, 表示成功 \n
 *         当返回值 < 0 时, 表示失败, 参见[hcc_error_code_drv_soc_common](@ref THccDrvSocCommonTag)
 */
Int32 TDrvGpioDestroy(TDrvGpio *self);

/**
 * 驱动打开
 * @param self: 对象实例指针
 * @return 当返回值 >= 0 时, 表示成功 \n
 *         当返回值 < 0 时, 表示失败, 参见[hcc_error_code_drv_soc_common](@ref THccDrvSocCommonTag)
 */
Int32 TDrvGpioOpen(TDrvGpio *self);

/**
 * 驱动关闭
 * @param self: 对象实例指针
 * @return 当返回值 >= 0 时, 表示成功 \n
 *         当返回值 < 0 时, 表示失败, 参见[hcc_error_code_drv_soc_common](@ref THccDrvSocCommonTag)
 */
Int32 TDrvGpioClose(TDrvGpio *self);

/**
 * 设置引脚输出电平
 * @param self: 对象实例指针
 * @param group: 端口组, 参见[drv_port](@ref TDrvPortGroup)
 * @param pin: 引脚号, 参见[drv_port](@ref TDrvPortPin)
 * @param level: true-高电平, false-低电平
 * @return 当返回值 >= 0 时, 表示成功 \n
 *         当返回值 < 0 时, 表示失败, 参见[hcc_error_code_drv_soc_common](@ref THccDrvSocCommonTag)
 */
Int32 TDrvGpioSetLevel(TDrvGpio *self, UInt8 group, UInt8 pin, Bool level);

/**
 * 引脚电平设置为高
 * @param self: 对象实例指针
 * @param group: 端口组, 参见[drv_port](@ref TDrvPortGroup)
 * @param pin: 引脚号, 参见[drv_port](@ref TDrvPortPin)
 * @return 当返回值 >= 0 时, 表示成功 \n
 *         当返回值 < 0 时, 表示失败, 参见[hcc_error_code_drv_soc_common](@ref THccDrvSocCommonTag)
 */
Int32 TDrvGpioSetHighLevel(TDrvGpio *self, UInt8 group, UInt8 pin);

/**
 * 引脚电平设置为低
 * @param self: 对象实例指针
 * @param group: 端口组, 参见[drv_port](@ref TDrvPortGroup)
 * @param pin: 引脚号, 参见[drv_port](@ref TDrvPortPin)
 * @return 当返回值 >= 0 时, 表示成功 \n
 *         当返回值 < 0 时, 表示失败, 参见[hcc_error_code_drv_soc_common](@ref THccDrvSocCommonTag)
 */
Int32 TDrvGpioSetLowLevel(TDrvGpio *self, UInt8 group, UInt8 pin);

/**
 * 获取引脚输入电平
 * @param self: 对象实例指针
 * @param group: 端口组, 参见[drv_port](@ref TDrvPortGroup)
 * @param pin: 引脚号, 参见[drv_port](@ref TDrvPortPin)
 * @param level: 获取的输入水平, 成功时有效, true-高电平, false-低电平
 * @return 当返回值 >= 0 时, 表示成功 \n
 *         当返回值 < 0 时, 表示失败, 参见[hcc_error_code_drv_soc_common](@ref THccDrvSocCommonTag)
 */
Int32 TDrvGpioGetLevel(TDrvGpio *self, UInt8 group, UInt8 pin, Bool *level);

// GPIO中断~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
typedef enum
{
    kDrvGpioIntTriggerDisable       = 0U,       ///< 禁止触发
    kDrvGpioIntTriggerRising        = 1U,       ///< 上升沿触发
    kDrvGpioIntTriggerFalling       = 2U,       ///< 下降沿触发
    kDrvGpioIntTriggerEdgeBoth      = 3U,       ///< 双边沿触发
    kDrvGpioIntTriggerMax,
} TDrvGpioIntTrigger;

typedef struct
{
    UInt8 group;                                ///< 端口组编号, 参见[drv_port.h](@ref TDrvPortGroup)
    UInt8 pin;                                  ///< 引脚编号, 参见[drv_port.h](@ref TDrvPortPin)
    // UInt8 mux;                                  ///< 复用功能, 参见[drv_port.h](@ref TDrvPortMux)
    UInt8 run_trigger;                          ///< 正常工作时触发模式, 参见[drv_gpio.h](@ref TDrvGpioIntTrigger)
    UInt8 sleep_trigger;                        ///< 休眠唤醒时触发模式, 参见[drv_gpio.h](@ref TDrvGpioIntTrigger)
} TDrvGpioIntCfg;

typedef struct TDrvGpioIntTag TDrvGpioInt;

/**
 * 事件函数, 当产生中断时触发调用
 * @param self: 对象实例指针
 * @return 无返回值
 */
typedef void (*TDrvGpioIntOnIrq)(TDrvGpioInt *sender);

/**
 * 构造一个引脚中断读写对象
 * @note
 * @par 示例
 * @code
   TDrvGpioInt drv;
   const TDrvGpioIntCfg cfg;

   TDrvGpioIntCreate(&drv, &cfg);
   TDrvGpioIntOpen(&drv);
   /// ...
   TDrvGpioIntClose(&drv);
   OfDestroyObject((TOfObject *)&drv);
 * @endcode
*/
struct TDrvGpioIntTag
{
PRIVATE
    UInt32 cross_tag_;              ///< 越界标记

PUBLIC
    void *receiver;                 ///< 事件接收者
    TDrvGpioIntOnIrq OnIrq;         ///< 事件函数

PRIVATE
    struct TDrvGpioIntTag *next_;   ///< 下一节点
    const TDrvGpioIntCfg *cfg_;     ///< 外部配置指针
    Bool permanent_;                ///< 持续标记
    Bool is_open_;                  ///< 打开标记
    UInt8 reserved[2];              ///< 对齐
};

/**
 * 构造
 * @param self: 对象实例指针
 * @param cfg: 中断引脚配置
 * @return 当返回值 >= 0 时, 表示成功 \n
 *         当返回值 < 0 时, 表示失败, 参见[hcc_error_code_drv_soc_common](@ref THccDrvSocCommonTag)
 */
Int32 TDrvGpioIntCreate(TDrvGpioInt *self, const TDrvGpioIntCfg *cfg);

/**
 * 析构
 * @param self: 对象实例指针
 * @return 当返回值 >= 0 时, 表示成功 \n
 *         当返回值 < 0 时, 表示失败, 参见[hcc_error_code_drv_soc_common](@ref THccDrvSocCommonTag)
 */
Int32 TDrvGpioIntDestroy(TDrvGpioInt *self);

/**
 * 设置中断使用的模式(run or sleep)
 * @note 本函数需要在 TDrvGpioIntOpen 之前使用
 *    -# 设置为持续产生模式时, 需要在中断事件中调用 TDrvExtIntClose 关闭中断, 否则CPU将一直处于中断响应中
 * @param self: 对象实例指针
 * @param permanent: True-持续的产生中断, False-只产生一次中断
 * @return 当返回值 >= 0 时, 表示成功 \n
 *         当返回值 < 0 时, 表示失败, 参见[hcc_error_code_drv_soc_common](@ref THccDrvSocCommonTag)
 */
Int32 TDrvGpioIntSetMode(TDrvGpioInt *self, Bool permanent);

/**
 * 打开
 * @note 本函数必须放置在引脚配置为GPIO输入之后.
 *    -# 否则引脚默认是高电平的情况下,在刚配置完后会产生一次rising中断.
 * @param self: 对象实例指针
 * @return 当返回值 >= 0 时, 表示成功 \n
 *         当返回值 < 0 时, 表示失败, 参见[hcc_error_code_drv_soc_common](@ref THccDrvSocCommonTag)
 */
Int32 TDrvGpioIntOpen(TDrvGpioInt *self);

/**
 * 查询是否已打开
 * @param self: 对象实例指针
 * @param is_open: True-已打开, False-未打开
 * @return 当返回值 >= 0 时, 表示成功 \n
 *         当返回值 < 0 时, 表示失败, 参见[hcc_error_code_drv_soc_common](@ref THccDrvSocCommonTag)
 */
Int32 TDrvGpioIntIsOpen(TDrvGpioInt *self, Bool *is_open);

/**
 * 关闭
 * @param self: 对象实例指针
 * @return 当返回值 >= 0 时, 表示成功 \n
 *         当返回值 < 0 时, 表示失败, 参见[hcc_error_code_drv_soc_common](@ref THccDrvSocCommonTag)
 */
Int32 TDrvGpioIntClose(TDrvGpioInt *self);

/**
 * 获取中断标志
 * @note: 仅在总中断关闭的情况下有效,否则始终返回False
 * @param self: 对象实例指针
 * @param flag: True-中断已产生, False-中断未产生
 * @return 当返回值 >= 0 时, 表示成功 \n
 *         当返回值 < 0 时, 表示失败, 参见[hcc_error_code_drv_soc_common](@ref THccDrvSocCommonTag)
 */
Int32 TDrvGpioIntGetFlag(TDrvGpioInt *self, Bool *flag);
/// --------------------------------- 必要接口 end ---------------------------------

/// --------------------------------- 特殊接口 start ---------------------------------
/// --------------------------------- 特殊接口 end ---------------------------------

#ifdef __cplusplus
}  // extern "C"
#endif

#endif // _DRV_GPIO_H_


