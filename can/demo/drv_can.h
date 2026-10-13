/**
 * @file drv_can.h
 * @brief CAN驱动
 * @note
 * @par          修改日志
 * <table>
 * <tr><th>Date        <th>Version               <th>Author          <th>Description
 * <tr><td>2025-12-06  <td>1.0                   <td>软件工程院      <td>创建初始版本
 * <tr><td>2026-03-16  <td>1.0                   <td>软件工程院      <td>支持CANFD
 * @copyright 杭州鸿泉物联网技术股份有限公司
 */
#ifndef _DRV_CAN_H_
#define _DRV_CAN_H_

#include <ofc_object.h>
#include <ofc_types.h>
#include <ofc_ring_buf.h>
#include "can_def.h"
#include "canfd_def.h"
#include "drv_can_cfg.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * CAN波特率配置值
 */
typedef struct TDrvCanBaudRateTag TDrvCanBaudRate;
struct TDrvCanBaudRateTag
{
    unsigned char pres_div;  ///< 预分频系数
    unsigned char prop_seg;  ///< 传播段
    unsigned char pseg1;  ///< 相位段1
    unsigned char pseg2;  ///< 相位段2
};

typedef enum TCANModeTag TCANMode;
enum TCANModeTag
{
    CAN_MODE_CAN20 = 0,    ///< CAN2.0模式
    CAN_MODE_CANFD = 1     ///< CANFD模式
};

typedef union {
    TCanData can20;
    TCanfdData canfd;
} TDrvCanUnifiedData;

/**
 * 构造一个Can控制器对象.默认波特率为250K
 *
 * 依赖的驱动:
 * 1. drv_port: 需事先创建端口
 * 2. drv_timer_svr: 收到的数据的时间来自g_timer_srv_tick
 *
 * @par 示例
 * @code
   TDrvCan Can;

   TDrvCanCreate(&Can, &g_drv_can_cfgs[0]);
   TDrvCanOpen(&Can);

   // TDrvCanWrite...
   // ...
   TDrvCanClose(&Can);
   OfDestroyObject((TOfObject *)&Can);
 * @endcode
*/
typedef struct TDrvCanTag TDrvCan;
typedef struct TDrvCanCfgExTag TDrvCanCfgEx;

/**
 * CAN数据发送后的事件,用于告知这个数据报是否发送成功
 * @param self: 对象实例指针
 * @param data_id: CAN数据id即为调用CAN发送时TCANVData.data_id
 * @param success: 用于表示数据是否发送成功,true表示发送成功,否则表示发送失败
 * @return
 */
typedef void (*TDrvCanSentEvent) (TDrvCan *self, UInt32 data_id, Bool success);

/**
 * 收到CAN数据时的事件,主动发起通知，告知有收到数据
 * @param self: 对象实例指针
 * @param
 * @return
 */
typedef void (*TDrvCanRecvEvent) (TDrvCan *self);

/**
 * CAN总线在Bus off时的触发的事件
 * @param self: 对象实例指针
 * @return
 */
typedef void (*TDrvCanBusOffEvent) (TDrvCan *self);

/**
 * 波特率侦测事件,用于告知当前的波特率是否与总线匹配
 * @param self: 对象实例指针
 * @param success: 用于表示当前波特率与总线是否匹配,true表示匹配,否则表示不匹配
 * @return
 */
typedef void (*TDrvCanDetectEvent) (TDrvCan *self, Bool success);

/**
 * CAN总线在打开时,进入正常工作状态前,设置硬件过滤器的事件
 * @param self: 对象实例指针
 * @return
 */
typedef void (*TDrvCanSetFilterEvent) (TDrvCan *self);

struct TDrvCanTag
{
PRIVATE
    UInt32 cross_tag_;       ///< 越界标记
    const TDrvCanCfg *cfg_;  ///< CAN配置信息
    const TDrvCanCfgEx *cfg_ex_;  ///< CAN扩展配置
    TOfRingBuf rx_buf_;  ///< 接收数据缓冲区
    TOfRingBuf tx_buf_;  ///< 发送数据缓冲区

    UInt32 tx_data_id_;  ///< 当前发送数据的数据id
    Int32 send_error_count_;  ///< 当前发送报文发送失败次数
    Int32 preemptive_send_error_count_;  ///< 优先发送报文发送失败次数

    Int32 error_frame_count_; ///< 错误帧个数

    Bool is_open_;  ///< 是否已经打开
    Bool sending_;  ///< 数据正在发送中
    Bool auto_recover_;  ///< 自动从Bus Off状态恢复(默认为自动恢复)
    Bool baud_rate_detect_mode_;  ///< 波特率自动侦测模式
    Byte preemptive_state_;  ///< 优先发送数据的状态(参见TPreemptiveStateTag)
    Bool bus_off_;  ///< 处于BusOff状态
    Bool filter_writeable_;  ///< 表示当前状态可以写入CAN过滤表
    UInt8 abort;  ///< 中止码, 指示主或从发出的中止指令

    TDrvCanBaudRate baud_rate_; ///< 标称波特率
    TCanData preemptive_data_;  ///< 优先发送的数据
    /// v1.1增加canfd相关配置
    TCANMode mode_;  ///< CAN模式
    TDrvCanBaudRate fd_baudrate_;   ///< flexible data rate
PUBLIC
    /// 存放事件接收者
    void *receiver;

    /// 报文发送完成事件
    /// @note:
    /// 1: 仅data_id不为0的报文会产生这个回调
    /// 2: 数据发送的结果可能是失败
    /// 3: 如果数据未发完,进入BusOff状态,则该事件会在BusOff恢复后,正常发送完成或者发送超时后收到.
    /// 4: 该事件在中断中调用,故需要尽快退出
    TDrvCanSentEvent OnSent;

    /// 收到报文事件.注: 该事件在中断中调用,故需要尽快退出
    TDrvCanRecvEvent OnRecv;

    /// 总线错误事件.注: 该事件在中断中调用,故需要尽快退出; 如果不是自动从BusOff状态恢复,需要调用TDrvCanBusOffRecover函数
    TDrvCanBusOffEvent OnBusOff;

    /// 波特率侦测结果事件. 注: 该事件在中断中调用,故需要尽快退出; 如果检测成功表示有数据收到,可以调用读取函数读取数据
    TDrvCanDetectEvent OnDetect;

    /// 在驱动打开时,设置CAN硬件过滤器的事件.可以为空,表示接收所有报文.注: 
    /// 1. 只有该事件中能调用TDrvCanSetFifoFilter函数
    /// 2. 如果设置了该事件,必须至少调用TDrvCanSetFifoFilter设置过滤器,否则一个CAN报文都无法收取
    TDrvCanSetFilterEvent OnSetFilter;

    /// v1.1增加canfd相关配置 如果模式是canfd才需要提供这块内存 大小为sizeof(TCanfdData)
    TCanfdData *preemptive_fd_data_buf_;  ///< canfd优先发送数据缓存区
};

/// --------------------------------- 必要接口 start ---------------------------------
/**
 * CAN驱动构造函数
 * @param self: 对象实例指针
 * @param cfg: 配置信息
 * @param tx_buf: 发送缓冲区,不允许为空
 * @param tx_buf_size: 发送缓冲区的大小.缓冲区大小必须是sizeof(TCanData)的整数倍
 * @return 当返回值 >= 0 时, 表示成功 \n
 *         当返回值 < 0 时, 表示失败, 参见[hcc_error_code_drv_soc_common](@ref THccDrvSocCommonTag)
 */
Int32 TDrvCanCreate(TDrvCan *self, const TDrvCanCfg *cfg, void *tx_buf, Int32 tx_buf_size, void *rx_buf, Int32 rx_buf_size);

/**
 * CAN驱动析构函数
 * @param self: 对象实例指针
 * @return 当返回值 >= 0 时, 表示成功 \n
 *         当返回值 < 0 时, 表示失败, 参见[hcc_error_code_drv_soc_common](@ref THccDrvSocCommonTag)
*/
Int32 TDrvCanDestroy(TDrvCan *self);

/**
 * 获取当前CAN的波特率
 * @param self: 对象实例指针
 * @return 返回波特率的值,单位:Hz
 */
UInt32 TDrvCanGetBaudRate(TDrvCan *self);

/**
 * 获取当前CANFD的波特率
 * @note 仅适用于支持配置成CANFD的控制器
 * @param self: 对象实例指针
 * @param baudrate: 标称波特率
 * @param fd_baudrate: 灵活速率波特率
 * @return 当返回值 >= 0 时, 表示成功 \n
 *         当返回值 < 0 时, 表示失败参见[drv_canfd](@ref TDrvCanfdErrorCodeTag)
 */
Int32 TDrvCanfdGetBaudRate(TDrvCan *self, UInt32 *baudrate, UInt32 *fd_baudrate);

/**
 * 设置CAN总线的波特率(CANFD 则是仲裁段波特率).设备的默认波特率为250kbps
 * @note 
 * 1. 仅在调用TDrvCanOpen前设置有效
 * 2. 内部自动计算预分频值和相位段值,如果计算失败返回False
 * @param self: 对象实例指针
 * @param baud_rate: 设置的波特率,单位: Hz
 * @return 设置成功返回True,否则返回False
 */
Bool TDrvCanSetBaudRate(TDrvCan *self, UInt32 baud_rate);

/**
 * 设置CANFD数据的波特率
 * @note
 * 1. 仅在调用TDrvCanOpen前设置有效
 * 2. 内部自动计算预分频值和相位段值,如果计算失败返回False
 * @param self: 对象实例指针
 * @param baud_rate: 设置的标称波特率,单位: Hz
 * @return 当返回值 >= 0 时, 表示成功 \n
 *         当返回值 < 0 时, 表示失败参见[drv_canfd](@ref TDrvCanfdErrorCodeTag)
 */
Bool TDrvCanfdSetBaudRate(TDrvCan *self, UInt32 baud_rate);

/**
 * 设置CAN总线模式 默认是CAN2.0
 * @note 
 * 1. 仅在调用TDrvCanOpen前设置有效
 * 2. 如果设置为CANFD,则只能使用TDrvCanfdSetBaudRate和TDrvCanfdSetBaudRateEx来设置波特率
 * @param self: 对象实例指针
 * @param mode: CAN模式,参见TCANModeTag
 * @return 当返回值 >= 0 时, 表示成功 \n
 *         当返回值 < 0 时, 表示失败参见[drv_canfd](@ref TDrvCanfdErrorCodeTag)
 */
Int32 TDrvCanSetMode(TDrvCan *self, TCANMode mode);

/**
 * 设置CAN总线的波特率(CANFD 则是仲裁段波特率).
 * @note: 
 * 1. 仅在调用TDrvCanOpen前设置有效
 * 2. 波特率计算公式: CAN模块输入时钟频率 / div / (1 + prog_seg + phase_seg1 + phase_seg2)
 * @param self: 对象实例指针
 * @param div: 预分频系数;用于产生TQ(TQ时长=div / 输入时钟频率)
 * @param prop_seg: 传播段的TQ数
 * @param phase_seg1: 相位段1的TQ数
 * @param phase_seg2: 相位段2的TQ数
 * @return 返回实际的波特率,如果参数有误,则返回0
 */
UInt32 TDrvCanSetBaudRateEx(TDrvCan *self, Byte div, Byte prop_seg, Byte phase_seg1, Byte phase_seg2);

/**
 * 设置CANFD 数据段的波特率.
 * @note: 
 * 1. 仅在调用TDrvCanOpen前设置有效
 * 2. 波特率计算公式: CAN模块输入时钟频率 / div / (1 + prog_seg + phase_seg1 + phase_seg2)
 * @param self: 对象实例指针
 * @param div: 标称波特率预分频系数;用于产生TQ(TQ时长=div / 输入时钟频率)
 * @param prop_seg: 传播段的TQ数
 * @param phase_seg1: 相位段1的TQ数
 * @param phase_seg2: 相位段2的TQ数
 * @return 返回实际的波特率,如果参数有误,则返回0
 */
UInt32 TDrvCanfdSetBaudRateEx(TDrvCan *self, Byte div, Byte prop_seg, Byte phase_seg1, Byte phase_seg2);

/**
 * 获取当前是否为自动从BusOff状态恢复(默认为是)
 * @param self: 对象实例指针
 * @return 返回True表示自动恢复,返回False表示手动恢复
 */
Bool TDrvCanAutoRecover(TDrvCan *self);

/**
 * 设置CAN模块处于Bus Off时,是否自动从Bus Off状态恢复
 * @note: 
 *  1. 如果禁用自动恢复,则需要在BusOff后调用TDrvCanBusOffRecover来从BusOff状态恢复.
 *  2. 仅在调用TDrvCanOpen前调用有效
 *  3. 如果禁用自动恢复,则必须设置OnBusOff事件,否则设置无效
 * @param self: 对象实例指针
 * @param auto_recover: True: 自动从Bus Off状态恢复; False: 需手动恢复
 * @return 
 */
void TDrvCanSetAutoRecover(TDrvCan *self, Bool auto_recover);

/**
 * 从Bus Off状态恢复到正常状态
 * @note: 仅在Bus Off状态下调用
 * @note: Bus Off状态需要经过128个序列的,每个序列11个隐性位之后才能恢复.在这之前调用本函数,实际上跟自动恢复一样
 * @param self: 对象实例指针
 * @return 
 */
void TDrvCanBusOffRecover(TDrvCan *self);

/**
 * 获取当前是否为波特率侦测模式(默认为否)
 * @param self: 对象实例指针
 * @return 返回True表示自动侦测波特率模式,返回False表示正常收发模式
 */
Bool TDrvCanDetectMode(TDrvCan *self);

/**
 * 设置为自动侦测波特率模式或者正常收发模式
 * @note: 
 *  1. 仅在调用TDrvCanOpen前调用有效
 * @param self: 对象实例指针
 * @param detect: True: 自动侦测波特率模式; False: 正常收发模式
 * @return 
 */
void TDrvCanSetDetectMode(TDrvCan *self, Bool detect);

/**
 * 获取CAN模块的编号(索引)
 * @param self: 对象实例指针
 * @return 0: 第一路CAN, 1: 第二路CAN, ...
 */
Byte TDrvCanBusNo(TDrvCan *self);

/**
 * 打开CAN驱动,用于数据收发或者用于波特率侦测
 * @note: 波特率自动侦测模式注意事项
 *  1. 需要CAN驱动关闭的情况下执行
 *  2. 执行完毕后需要调用TDrvCanClose关闭侦测模式后方可再打开CAN驱动
 *  3. 侦测时,会自动从BusOff状态恢复
 *  4. 侦测时,OnSent, OnBusOff事件均不会调用,但OnRecv事件还是会调用
 *  5. 侦测时,会产生一个或者多个OnDetect事件,用于告知侦测的结果
 *  6. 如果对端只有一个CAN节点在发数据,即发送的数据没有应答是无法自动侦测的(控制器在收到正确但未应答的报文,只会报Bit0错误;总线波特率比设置的波特率大的时候,有可能不产生错误中断)
 * @note: 波特率自动侦测模式侦测方式
 *  1. 设置OnDetect事件
 *  2. 调用TDrvCanSetDetectMode,参数detect为True
 *  3. 调用TDrvCanSetBaudRate设置一种波特率(注意: 检测时波特率务必由低到高,否则可能会出错)
 *  4. 调用TDrvCanOpen,开始检测该波特率是否正确
 *  5. 通过OnDetect事件得知当前波特率检查的结果(需要外设有数据发送才能侦测,且回调该事件)
 *  6. 在OnDetect事件中设置TSfEvent来通知检测结果
 *  7. 主程序在收到事件后判断是否检测成功
 *  8. 如果检测失败,则调用TDrvCanClose关闭检测,然后跳转到步骤3
 *  9. 如果检测成功,则调用TDrvCanClose关闭检测
 * @param self: 对象实例指针
 * @return 当返回值 >= 0 时, 表示成功 \n
 *         当返回值 < 0 时, 表示失败, 参见[hcc_error_code_drv_soc_common](@ref THccDrvSocCommonTag)
 */
Int32 TDrvCanOpen(TDrvCan *self);

/**
 * 驱动关闭
 * @param self: 对象实例指针
 * @return
 */
Int32 TDrvCanClose(TDrvCan *self);

/**
 * 读取收到的CAN数据
 * @note: 接收报文的时间戳来自g_timer_srv的tick,故打开CAN驱动前需要先初始化g_timer_srv
 * @param self: 对象实例指针
 * @param buf: 存放CAN数据的缓冲区
 * @param count: Buf能容纳多少个TCANVData类型的数据
 * @return 返回实际读取到的TCANVData类型的数据个数
 */
Int32 TDrvCanRead(TDrvCan *self, void *buf, Int32 count);

/**
 * 写CAN数据到发送队列.写完后会自动开始发送
 * @note: 如果是波特率侦测模式,则数据无法发送
 * @param self: 对象实例指针
 * @param data: 需要发送的CAN数据
 * @param count: Data包含的TCANVData类型的数据的个数
 * @return 返回实际发送的TCANVData类型的数据个数
 */
Int32 TDrvCanWrite(TDrvCan *self, const void *data, Int32 count);

/**
 * 优先发送CAN数据.
 * @note: 函数与TDrvCanWrite的区别
 * 1. TDrvCanWrite把CAN数据放入发送队列,而本函数不把数据加入发送队列
 * 2. 本函数写入的CAN数据优先发送
 * 3. TDrvCanWrite支持一次性写入多个CAN数据,而本函数只能写入一个数据
 * 4. 如果本函数上一次的数据还没有发送,则直接返回失败
 * 5. 如果是波特率侦测模式,则数据无法发送
 * 6. 本函数使用的mb和TDrvCanWrite使用的mb不同(之前版本使用同一个mb),本函数使用mb优先级高于另一个
 * @param self: 对象实例指针
 * @param data: 需要发送的CAN数据
 * @return 返回True表示写入成功,等待优先发送; False表示上一次调用本函数的数据还没有发送
 */
Bool TDrvCanWritePreemptive(TDrvCan *self, const void *data);

/**
 * 返回当前是否处于busoff状态
 * @note: 仅当BusOff恢复模式为手工模式,且设置了OnBusOff事件,BusOff出现时才会返回True
 * @param self: 对象实例指针
 * @return True: 当前处于BusOff状态; False: 当前为非BusOff状态
 */
Bool TDrvCanBusOff(TDrvCan *self);

/**
 * 取消当前正在发送的CAN报文
 * @note: 对于队列中data_id不为0的报文会产生OnSent事件
 * @param self: 对象实例指针
 * @return 
 */
void TDrvCanAbortSend(TDrvCan *self);

/**
 * 清除发送队列中的所有报文(包括优先发送的数据);
 * @note
 *  1. 仅在BusOff状态下调用有效
 *  2. 对于队列中data_id不为0的报文会产生OnSent事件
 * @param self: 对象实例指针
 * @return 
 */
void TDrvCanClearSendFifo(TDrvCan *self);

/**
 * 设置FIFO的硬件过滤器
 * @note
 *  1. 仅在OnSetFilter事件中可以调用本函数,其它地方调用均返回False
 *  2. mask和filter的关系如下:
 *     2.1. mask.bit[x]=0, 输入报文ID.bit[x]是0或者1都接收; 标准帧x的范围为[0..10], 扩展帧x的范围为[0..28]
 *     2.2. mask.bit[x]=1, 输入报文ID.bit[x]==Filter.bit[x],则接收,否则丢弃
 *     2.3. mask.bit[31]=0, 数据帧和远程帧都接收
 *     2.4. mask.bit[31]=1, 
 *        2.4.1. filter.bit[31]=0: 仅接收数据帧
 *        2.4.2. filter.bit[31]=1: 仅接收远程帧
 *     2.5. mask.bit[30]=0, 标准帧和扩展帧都接收
 *     2.6. mask.bit[30]=1, 
 *        2.6.1. filter.bit[31]=0: 仅接收标准帧
 *        2.6.2. filter.bit[31]=1: 仅接收扩展帧
 *  3. 如果在OnSetFilter事件中不调用本函数,默认设置的过滤器为不接收任何CAN报文,即FIFO无效
 *  4. TODO: 按目标MCU过滤器能力填写与上述通用语义的差异及转换规则。
 *  5. 多个过滤器同时使用时, 遵守白名单规则, 只要其中一个满足即可通过.
 * @param self: 对象实例指针
 * @param index: Fifo过滤器的索引.索引从0开始; 过滤器个数可以通过TDrvCanGetFifoFilterCount获取
 * @param mask: 掩码; Bit30对应IDE掩码; bit[0..28]对应扩展帧Id, bit[0..10]对应标准帧Id; 参见note
 * @param filter: 过滤器值,参见note
 * @param ide: mask和filter是标准帧还是扩展帧; 0: 标准帧, 1: 扩展帧
 * @return 设置成功返回True,否则返回False; 
 */
Bool TDrvCanSetFifoFilter(TDrvCan *self, Int32 index, UInt32 mask, UInt32 filter, Bool ide);

/**
 * 获取FIFO的过滤器个数
 * @param self: 对象实例指针
 * @return 过滤器个数
 */
Int32 TDrvCanGetFifoFilterCount(TDrvCan *self);

/**
 * 获取错误帧个数
 * @note
 *  1. 获取从open开始检测到的所有错误帧个数
 * @param self: 对象实例指针
 * @return >= 0 表示实际错误帧个数, < 0 表示错误码
 */
Int32 TDrvCanGetErrorFrameCount(TDrvCan *self);
/// --------------------------------- 必要接口 end ---------------------------------

/// --------------------------------- 特殊接口 start ---------------------------------
/// --------------------------------- 特殊接口 end ---------------------------------

#ifdef __cplusplus
}  // extern "C"
#endif

#endif// _DRV_CAN_H_

