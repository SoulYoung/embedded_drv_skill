/**
 * @file drv_timer_srv.h
 * @brief 定时服务器驱动,用于产生周期性触发事件
 * @note

 * @par          修改日志
 * <table>
 * <tr><th>Date             <th>Version         <th>Author          <th>Description
 * <tr><td>2025-12-11       <td>1.0             <td>软件工程院       <td>创建初始版本

 * @copyright 杭州鸿泉物联网技术股份有限公司
 */

#ifndef _DRV_TIMER_SRV_H_
#define _DRV_TIMER_SRV_H_

#include <ofc_object.h>
#include <ofc_types.h>

#ifdef __cplusplus
extern "C" {
#endif

/// 最多支持的定时器个数
#define MAX_TIMER_COUNT 16

typedef struct TDrvTimerSrvTag TDrvTimerSrv;

/**
 * 定时器,用于产生周期性触发事件
 * 注:
 * 1. 创建定时器TDrvTimerItem前必须先要创建TDrvTimerSrv的实例(仅需一次即可)
 * 2. 创建的定时器不允许删除
 * @par 示例
 * @code
   TDrvTimerItem *timer = TDrvTimerSrvCreateTimer(&g_drv_timer_srv);
   timer->receiver = self;
   timer->OnTimer = OnTimer;
   TDrvTimerItemSetInterval(timer, 10);
   TDrvTimerItemStart(timer);
   ...
   TDrvTimerItemStop(timer);
   // OfDestroyObject((TOfObject *)&timer); // 注: 创建的定时器不允许删除
 * @endcode
*/
typedef struct TDrvTimerItemTag TDrvTimerItem;

/**
 * 定时器定时事件的函数类型
 * @param sender: 事件的产生者
 * @return
 */
typedef void (*TDrvTimerOnTimerEvent)(TDrvTimerItem *sender);

struct TDrvTimerItemTag
{
PUBLIC
    void *receiver;                     ///< 事件的接收者
    TDrvTimerOnTimerEvent OnTimer;      ///< 定时事件; 注: 该事件在中断中调用

PRIVATE
    UInt32 interval_;                   ///< 定时器间隔
    UInt32 tick_count_;                 ///< 当前计数值
    Bool enabled_;                      ///< 定时器是否使能
    UInt8 reserved_[3];                 ///< 对齐
};

/// --------------------------------- 必要接口 start ---------------------------------
/**
 * 获取定时器的定时间隔
 * @param self: 对象实例指针
 * @return
 */
UInt32 TDrvTimerItemInterval(TDrvTimerItem *self);

/**
 * 设置定时间隔
 * @param self: 对象实例指针
 * @param ms: 定时间隔,单位ms
 * @return
 */
void TDrvTimerItemSetInterval(TDrvTimerItem *self, UInt32 ms);

/**
 * 启动定时器
 * 注: 启动后首次OnTimer事件会发生在定时间隔减一到定时间隔之间; 如果定时间隔设置为1,则会在定时器使能时刻到1ms时触发OnTimer
 * @param self: 对象实例
 * @return
 */
void TDrvTimerItemStart(TDrvTimerItem *self);

/**
 * 停止定时器
 * @param self: 对象实例
 * @return
 */
void TDrvTimerItemStop(TDrvTimerItem *self);

/**
 * 定时器服务,用于管理其它TDrvTimerItem
 * 注:创建定时器TDrvTimerItem前必须先要创建TDrvTimerSrv的实例(仅需一次即可
 * @par 示例
 * @code
   TDrvTimerSrvCreate(&g_drv_timer_srv);
   TDrvTimerSrvOpen(&g_drv_timer_srv);
   ...
   TDrvTimerSrvClose(&g_drv_timer_srv);
   TDrvTimerSrvDestroy(&g_drv_timer_srv);
 * @endcode
*/
struct TDrvTimerSrvTag
{
PRIVATE
    UInt32 cross_tag_;                          ///< 越界标记
    const void* cfg_;                           ///< 配置信息
    TDrvTimerItem timers_[MAX_TIMER_COUNT];     ///< 定时器列表
    Int32 count_;                               ///< 当前的定时器个数
    Int32 index_;                               ///< 可取的索引值
    Bool is_open_;                              ///< 是否已经打开
    UInt8 reserved_[3];                         ///< 对齐
};

/**
 * 定时服务器器构造函数
 * @param self: 对象实例指针
 * @return 当返回值 >= 0 时, 表示成功 \n
 *         当返回值 < 0 时, 表示失败, 参见[drv_error_code](@ref TDrvErrorCodeTag)
 */
Int32 TDrvTimerSrvCreate(TDrvTimerSrv *self);

/**
 * 定时服务器器析构函数
 * @param self: 对象实例指针
 * @return 当返回值 >= 0 时, 表示成功 \n
 *         当返回值 < 0 时, 表示失败, 参见[drv_error_code](@ref TDrvErrorCodeTag)
 */
Int32 TDrvTimerSrvDestroy(TDrvTimerSrv *self);

/**
 * 打开定时服务器器
 * @param self: 对象实例指针
 * @return 当返回值 >= 0 时, 表示成功 \n
 *         当返回值 < 0 时, 表示失败, 参见[drv_error_code](@ref TDrvErrorCodeTag)
 */
Int32 TDrvTimerSrvOpen(TDrvTimerSrv *self);

/**
 * 关闭定时服务器器(同时会停止所有定时器)
 * @param self: 对象实例指针
 * @return 当返回值 >= 0 时, 表示成功 \n
 *         当返回值 < 0 时, 表示失败, 参见[drv_error_code](@ref TDrvErrorCodeTag)
 */
Int32 TDrvTimerSrvClose(TDrvTimerSrv *self);

/**
 * 创建定时器对象.
 * 注:
 *   1. 定时器一旦创建,无法释放
 *   2. 当定时器个数超过MAX_TIMER_COUNT时,创建失败
 * @param self: 对象实例指针
 * @return 创建的定时器对象, 该定时器不允许外部释放.如果创建失败返回NULL
 */
TDrvTimerItem *TDrvTimerSrvCreateTimer(TDrvTimerSrv *self);

extern TDrvTimerSrv g_drv_timer_srv;  ///< 定时服务器对象

extern volatile UInt32 g_timer_srv_tick;  ///< 定时服务器产生的tick值.仅在定时器服务器创建后有效(TDrvTimerSrvCreate)

/**
 * 获取高精度定时器时间
 * @note 
 *  时间计算方式: 
 * 当前的精确时间点为tick + value * 1.0 / max_value, 即当前时间在tick后的value / max_value毫秒处
 * @param tick: 返回当前tick值
 * @param value: 当前计数个数
 * @param max_value: 1ms的计数个数
 * @return 无返回值
 */
void DrvTimerSrvGetHighPerformanceCounter(UInt32 *tick, UInt32 *value, UInt32 *max_value);
/// --------------------------------- 必要接口 end ---------------------------------

/// --------------------------------- 特殊接口 start ---------------------------------
/// --------------------------------- 特殊接口 end ---------------------------------

#ifdef __cplusplus
}  // extern "C"
#endif

#endif // _DRV_TIMER_SRV_H_


