/**
 * @file drv_xcu_spi.h
 * @brief SPI与主控制器之间的通讯接口驱动
 * @note
 * @author Leo

 * @par          修改日志
 * <table>
 * <tr><th>Date             <th>Version         <th>Author              <th>Description
 * <tr><td>2026-09-18       <td>1.0             <td>软件工程院           <td>创建初始版本
 * @copyright 杭州鸿泉物联网技术股份有限公司
 */

#ifndef _DRV_XCU_SPI_H_
#define _DRV_XCU_SPI_H_

#include <ofc_object.h>
#include <ofc_types.h>

#include "drv_dma.h"
#include "drv_timer_srv.h"
#include "drv_xcu_spi_cfg.h"
#include "drv_common.h"

#ifdef __cplusplus
extern "C" {
#endif

enum
{
    kDrvXcuSpiItemPower = 1,  ///< 乒乓缓冲区个数的幂
    kDrvXcuSpiItemMax = 1 << kDrvXcuSpiItemPower,  ///< 最大项数,必须是2的n次方

    kDrvXcuSpiBufSize = 124,  ///< TDrvXcuSpiData的数据区长度
};

/**
 * SPI与Xcu之间的驱动数据定义
 */
typedef struct TDrvXcuSpiDataTag TDrvXcuSpiData;
struct TDrvXcuSpiDataTag
{
    Byte tag;  ///< 标签
    Byte len;  ///< 长度
    Byte data[kDrvXcuSpiBufSize];  ///< 数据缓存
    Word tile; ///< 结束符
};

/**
 * SPI与Xcu之间的驱动数据项定义
 */
typedef struct TDrvXcuSpiItemTag TDrvXcuSpiItem;
struct TDrvXcuSpiItemTag
{
    TDrvXcuSpiData tx;  ///< 发送数据结构
    TDrvXcuSpiData rx;  ///< 接收数据结构
    Byte write_pos;     ///< 发送写指针; 仅线程会设置
    Byte read_pos;      ///< 接收读指针; 仅线程会设置
    Byte state;         ///< 状态; 线程和中断均会设置
    Byte reserved;      ///< 保留
};

/**
 * XCU相关的SPI从驱动,用于完成与主控制器端的数据交互
 *
 * 依赖的驱动:
 * 1. drv_port, 需事先创建端口g_drv_port_c
 * 2. drv_dma, 需事先创建dma控制器g_drv_dma
 * 3. drv_timer_srv, 需事先创建定时服务器g_drv_timer_srv
 *
 * @par 示例
 * @code
   TDrvXcuSpi spi;
   TDrvXcuSpiCreate(&spi, &g_drv_xcu_spi_cfg);
   spi.OnComplete = OnComplete;
   spi.OnError = OnError;
   spi.recevicer = self;

   TDrvXcuSpiOpen(&spi);
   // ...
   if (TDrvXcuSpiFreeSize(&spi) > 0)
   {
       TDrvXcuSpiWrite(&spi, data, size, &free_size);
       TDrvXcuSpiCommit(&spi);
   }

   if (TDrvXcuSpiReadable(&spi))
       TDrvXcuSpiRead(&spi, buf, size);

   //...
   TDrvXcuSpiClose(&spi);
   OfDestroyObject((TOfObject *)&spi);
 * @endcode
*/
typedef struct TDrvXcuSpiTag TDrvXcuSpi;

/**
 * 数据收发完成事件的函数类型
 * @param sender: 事件的产生者
 * @param server_req: 服务器是否有数据请求发送
 * @return
 */
typedef void (*TDrvXcuSpiDoComplete)(TDrvXcuSpi *sender, Bool server_req);

/**
 * 当传输有错误发生时调用,用户需要把这个错误提交到处理线程,调用TDrvXcuSpiReset来复位传输
 * @param sender: 事件的产生者
 * @param timeout: 是否为超时错误.传输时主控制器如果不响应会发生超时错误;如果为false表示其它错误,如DMA接收错误等
 * @return
 */
typedef void (*TDrvXcuSpiDoError)(TDrvXcuSpi *sender, Bool timeout);


struct TDrvXcuSpiTag
{
PRIVATE
    UInt32 cross_tag_;                                  ///< 越界标记

PUBLIC
    void *receiver;                                     ///< 事件的接收者

    /// 收发完成事件; 注意: 1. DMA中断中调用 2. 不要在函数里面发送和读取数据需提交到处理线程处理
    TDrvXcuSpiDoComplete OnComplete;

    /// 收发错误或者超时(50ms)事件; 注意: 1. DMA中断调用 2. 错误需提交到处理线程,需要延迟至少1ms后调用TDrvXcuSpiReset来复位传输
    TDrvXcuSpiDoError OnError;

PRIVATE
    TDrvXcuSpiItem items_[kDrvXcuSpiItemMax];           ///< 数据项数组
    const TDrvXcuSpiCfg *cfg_;                          ///< SPI配置
    const TDrvSpiBaseCfgEx *cfg_ex_;                    ///< 扩展配置

    TDrvDmaCh dma_ch_tx_;                               ///< 用于SPI数据发送的DMA通道
    TDrvDmaCh dma_ch_rx_;                               ///< 用于SPI数据接收的DMA通道
    TDrvTimerItem *timer_;                              ///< 定时器,用于检查传输超时.注: 定时器不要释放

    Bool is_open_;                                      ///< 表示驱动是否打开; 仅线程会设置
    Byte finished_index_;                               ///< 完成索引项; 仅线程会设置
    Byte write_index_;                                  ///< 写索引项; 仅线程会设置
    Byte read_index_;                                   ///< 读索引项; 仅中断会设置(初始化除外)
    Bool tx_completed_;                                 ///< 发送完成标志; 仅中断会设置(初始化除外)
    Bool rx_completed_;                                 ///< 接收完成标志; 仅中断会设置(初始化除外)
    Bool error_;                                        ///< 传输错误标志; 仅中断会设置(初始化除外)
    Bool timeout_;                                      ///< 传输超时标志,表示主控制器没有响应
    Bool busy_;                                         ///< 传输标志,表示传输任务启动,DMA准备好传输到传输完成之间均为true; 线程和中断均会设置
    UInt8 reserved_[3];                                 ///< 对齐
};

/// --------------------------------- 必要接口 start ---------------------------------
/**
 * 驱动创建
 * @param self: 对象实例指针
 * @param cfg: SPI相关的硬件配置信息
 * @return 当返回值 >= 0 时, 表示成功 \n
 *         当返回值 < 0 时, 表示失败, 参见[drv_error_code](@ref TDrvErrorCodeTag)
 */
Int32 TDrvXcuSpiCreate(TDrvXcuSpi *self, const TDrvXcuSpiCfg *cfg);

/**
 * 驱动销毁
 * @param self: 对象实例指针
 * @return 当返回值 >= 0 时, 表示成功 \n
 *         当返回值 < 0 时, 表示失败, 参见[drv_error_code](@ref TDrvErrorCodeTag)
 */
Int32 TDrvXcuSpiDestroy(TDrvXcuSpi *self);

/**
 * 设置超时时间,默认为50ms
 * @param self: 对象实例指针
 * @param timeout: 超时时间,单位为毫秒
 * @return 当返回值 >= 0 时, 表示成功 \n
 *         当返回值 < 0 时, 表示失败, 参见[drv_error_code](@ref TDrvErrorCodeTag)
 */
Int32 TDrvXcuSpiSetTimeout(TDrvXcuSpi *self, UInt32 timeout);

/**
 * 驱动打开
 * @param self: 对象实例指针
 * @return 当返回值 >= 0 时, 表示成功 \n
 *         当返回值 < 0 时, 表示失败, 参见[drv_error_code](@ref TDrvErrorCodeTag)
 */
Int32 TDrvXcuSpiOpen(TDrvXcuSpi *self);

/**
 * 驱动关闭
 * @param self: 对象实例指针
 * @return 当返回值 >= 0 时, 表示成功 \n
 *         当返回值 < 0 时, 表示失败, 参见[drv_error_code](@ref TDrvErrorCodeTag)
 */
Int32 TDrvXcuSpiClose(TDrvXcuSpi *self);

/**
 * 返回当前驱动是否有数据可以读取; 本函数可以跳过那些标记为错误的项
 * @param self: 对象实例指针
 * @return 返回是否有数据可以读取.如果可以读取返回true,否则返回false
 */
Bool TDrvXcuSpiReadable(TDrvXcuSpi *self);

/**
 * 从SPI驱动读取数据.如果多个条目中有可读取数据,且缓冲区足够的话,会从多个条目中读取数据
 * 注:
 *   1. 不支持多线程调用
 *   2. 本函数可以跳过那些标记为错误的项
 * @param self: 对象实例指针
 * @param buf: 存放数据的缓存区
 * @param size: 缓冲区长度
 * @return
 */
Int32 TDrvXcuSpiRead(TDrvXcuSpi *self, void *buf, Int32 size);

/**
 * 返回当前条目可以写入的字节数
 * 注: 返回值仅表示当前条目的空闲字节数,其它条目是否空闲需把当前条目提交后才能得到.
 * @param self: 对象实例指针
 * @return 返回可以写入的字节数
 */
Int32 TDrvXcuSpiFreeSize(TDrvXcuSpi *self);

/**
 * 写数据到SPI驱动
 * 注:
 *   1. 写入不会自动提交发送,需要主动调用TDrvXcuSpiCommit来发送
 *   2. 一次调用仅写入到一个条目.条目写满后可调用TDrvXcuSpiCommit来提交发送.提交发送后可以通过TDrvXcuSpiFreeSize来判断是否还可以写入
 *   3. 不支持多线程调用
 * @param self: 对象实例指针
 * @param data: 数据指针
 * @param size: 数据大小
 * @param free_size: 用于返回写入后还有当前item还有多少空闲缓冲区; 可以为NULL
 * @return 实际写入的数据数
 */
Int32 TDrvXcuSpiWrite(TDrvXcuSpi *self, const void *data, Int32 size, Int32 *free_size);

/**
 * 提交空闲状态的的条目,而不管条目中是否有数据.如果没有空闲条目,则不提交
 * 注: 不支持多线程调用
 * @param self: 对象实例指针
 * @return
 */
void TDrvXcuSpiCommit(TDrvXcuSpi *self);

/**
 * 传输功能复位.用于收发数据错误时调用.
 * 注:
 *   1. OnError事件中不得调用该函数,需在OnError中发送事件到线程中调用;
 *   2. 调用前需延迟1ms以上,确保主控制器端到MCU这里的包已经传输完毕
 *   3. 调用时第一次应当设置force为false,如果调用返回值为false,则延迟1ms后再次以参数force为true调用一次
 * @param self: 对象实例指针
 * @param force: 是否强制停止; true: 强制停止; false: 正常停止
 * @return 复位成功返回true,否则返回false,需休眠1ms后重新调用; force为true时返回值始终为true
 */
Bool TDrvXcuSpiReset(TDrvXcuSpi *self, Bool force);
/// --------------------------------- 必要接口 end ---------------------------------

/// --------------------------------- 特殊接口 start ---------------------------------
/// --------------------------------- 特殊接口 end ---------------------------------
#ifdef __cplusplus
}  // extern "C"
#endif

#endif // _DRV_XCU_SPI_H_
