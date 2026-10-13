/**
 * @file drv_dma.h
 * @brief DMA驱动
 * @note
 * @par          修改日志
 * <table>
 * <tr><th>Date             <th>Version         <th>Author              <th>Description
 * <tr><td>2025-12-09       <td>1.0             <td>软件工程院           <td>创建初始版本
 * @copyright 杭州鸿泉物联网技术股份有限公司
 */

#ifndef _DRV_DMA_H_
#define _DRV_DMA_H_

#include <ofc_types.h>
#include <ofc_object.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct TDrvDmaTag TDrvDma;

/**
 * DMA通道
 * 注1: 创建通道前必须先要创建TDrvDma的实例(仅需一次即可)
 * 注2: 启动DMA时,先调用TDrvDmaChEnableRequest,再使能外设的DMA请求
 * 注3: 关闭DMA时,先确保外设的DMA请求已经关闭,再调用TDrvDmaChDisableRequest,并检查该函数的返回值,如果返回false,需等待后重新调用直到成功
 *
 * DMA传输等价于下面的代码:
 * for (Int32 i = 0; i < majoy_loop_count; i++)
 * {
 *     Int32 size = minor_size;
 *
 *     while (size > 0)
 *     {
 *         Write(Read());
 *     }
 * }
 *
 * Read() / Write():
 * 这里的实现:
 * 如果src_transfer_size是1字节类型,dst_transfer_size是4字节类型,则为
 * for (Int32 i = 0; i < 4; i++)
 * {
 *     t[i] = *((char *)src_addr);
 *     src_addr += src_ofs;  // 一般情况下src_ofs = 1
 * }
 * *((UInt32*)dst_addr) = *((UInt32 *)t);
 * dst_addr += dst_ofs;  // 一般情况下dst_ofs = 4
 * size -= 4;
 *
 * 其它的字节类型以此类推
 *
 * @par 示例
 * @code
 TDrvDmaCh ch;
 TDrvDmaChCreate(&ch, &g_drv_dma, channel, source);

 ch.OnComplete = DoDmaRxComplete;
 ch.OnError = DoDmaRxError;

 TDrvDmaChOpen(&ch);

 TDrvDmaChSetAsReadPeriph(&ch, (uint32_t)&(spi_base->RDR), kDmaTransferSize1B);
 // 使能SPI接收的DMA功能
 ...
 TDrvDmaChSetDstAddr(&ch, 0x1000);
 TDrvDmaChSetMajorLoopCount(&ch, 128);
 TDrvDmaChEnableRequest(&ch);
 ...
 // 关闭SPI接收的DMA功能

 // 禁用DMA请求(直到成功)
 while (!(TDrvDmaChDisableRequest(&ch))
     SfSleep(1);

 TDrvDmaChClose(&ch);
 OfDestroyObject((TOfObject *)&ch);
 * @endcode
*/
typedef struct TDrvDmaChTag TDrvDmaCh;

/**
 * DMA驱动错误掩码的枚举
 */
typedef enum TDrvDmaErrorMaskTag TDrvDmaErrorMask;
enum TDrvDmaErrorMaskTag
{
    kDmaErrorDstBusMask = (1 << 2),     ///< 目标写入时总线错误的掩码
    kDmaErrorSrcBusMask = (1 << 3),     ///< 源读取时总线错误的掩码
    kDmaErrorDstOfsCfgMask = (1 << 4),  ///< 目标偏移配置错误的掩码
    kDmaErrorSrcOfsCfgMask = (1 << 5),  ///< 源偏移配置错误的掩码
    kDmaErrorDstAddrCfgMask = (1 << 6), ///< 目标地址配置错误的掩码
    kDmaErrorSrcAddrCfgMask = (1 << 7), ///< 源地址配置错误的掩码 
};

/**
 * DMA传输大小的枚举
 */
typedef enum TDmaTransferSizeTag TDmaTransferSize;
enum TDmaTransferSizeTag
{
    kDmaTransferSize1B = 0,   ///< 一次传输1个字节
    kDmaTransferSize2B = 1,   ///< 一次传输2个字节
    kDmaTransferSize4B = 2,   ///< 一次传输4个字节
    kDmaTransferSizeMax,
};

/**
 * DMA通知事件的函数类型
 * @param sender: 事件的产生者
 * @return
 */
typedef void (*TDrvDmaOnComplete)(TDrvDmaCh *sender);

/**
 * DMA错误事件的函数类型
 * @param sender: 事件的产生者
 * @param errors: 错误信息,每种错误是一个bit,可以通过TDrvDmaErrorMask的值的掩码来得到具体的错误.如if (errors & kDmaErrorDstBusMask) ...
 * @return
 */
typedef void (*TDrvDmaOnError)(TDrvDmaCh *sender, UInt32 errors);

struct TDrvDmaChTag
{
PRIVATE
    UInt32 cross_tag_;                  ///< 越界标记

PUBLIC
    void *receiver;                     ///< 事件的接收者
    TDrvDmaOnComplete OnComplete;       ///< DMA完成事件; 注: 该事件在中断中调用
    TDrvDmaOnError OnError;             ///< DMA错误事件; 注: 该事件在中断中调用.调用前已经禁用DMA请求

PRIVATE
    TDrvDma *dma_;                      ///< DMA管理器
    UInt8 channel_;                     ///< DMA通道号
    UInt8 source_;                      ///< 请求源, 参考目标MCU DMA请求映射表
    Bool is_open_;                      ///< 是否已经打开
    UInt8 reserved;                     ///< 对齐
};

/// --------------------------------- 必要接口 start ---------------------------------
/**
 * Dma通道构造函数
 * @param self: 对象实例指针
 * @param dma: DMA控制器
 * @param channel: DMA通道号, 最大16个通道
 * @param source: 该通道对应的输入源, 参见目标MCU DMA请求源定义
 * @return 当返回值 >= 0 时, 表示成功 \n
 *         当返回值 < 0 时, 表示失败, 参见[hcc_error_code_drv_soc_common](@ref THccDrvSocCommonTag)
 */
Int32 TDrvDmaChCreate(TDrvDmaCh *self, TDrvDma *dma, UInt8 channel, UInt8 source);

/**
 * 析构函数
 * @param self: 对象实例指针
 * @return 当返回值 >= 0 时, 表示成功 \n
 *         当返回值 < 0 时, 表示失败, 参见[hcc_error_code_drv_soc_common](@ref THccDrvSocCommonTag)
 */
Int32 TDrvDmaChDestroy(TDrvDmaCh *self);

/**
 * 打开DMA通道
 * @param self: 对象实例指针
 * @return 当返回值 >= 0 时, 表示成功 \n
 *         当返回值 < 0 时, 表示失败, 参见[hcc_error_code_drv_soc_common](@ref THccDrvSocCommonTag)
 */
Int32 TDrvDmaChOpen(TDrvDmaCh *self);

/**
 * 关闭DMA通道
 * @note: 关闭时会自动关闭所有打开的通道
 * @param self: 对象实例指针
 * @return 当返回值 >= 0 时, 表示成功 \n
 *         当返回值 < 0 时, 表示失败, 参见[hcc_error_code_drv_soc_common](@ref THccDrvSocCommonTag)
 */
Int32 TDrvDmaChClose(TDrvDmaCh *self);

/**
 * DMA通道配置为读取外设的通道
 * 默认配置为n字节传输,即minor loop一次循环总共传输n字节, 每次读/写n字节,minor loop循环次数为1;
 * n由transfer_size映射得到;如kDmaTransferSize4B时n=4
 * 外设到内存的传输调用顺序:
 * 1. 调用本函数
 * 2. 调用TDrvDmaChSetDstAddr设置目标内存地址(因内部一次传输n字节,故目标内存地址以n字节对齐)
 * 3. 调用TDrvDmaChSetMajorLoopCount设置要传输的字节数
 * 4. 调用TDrvDmaChEnableRequest来使能DMA请求功能
 * @param self: 对象实例指针
 * @param periph_addr: 外设地址
 * @param transfer_size: 一次传输的字节数对应的类型(注: 设置为源和目标都是这个类型,如需单独设置,需要调用TDrvDmaChSetAsCustom)
 *                    -# 参见[drv_dma](@ref TDmaTransferSize)
 * @return 当返回值 >= 0 时, 表示成功 \n
 *         当返回值 < 0 时, 表示失败, 参见[hcc_error_code_drv_soc_common](@ref THccDrvSocCommonTag)
 */
Int32 TDrvDmaChSetAsReadPeriph(TDrvDmaCh *self, UInt32 periph_addr, UInt8 transfer_size);

/**
 * DMA通道配置为写入外设的通道
 * 默认配置为n字节传输,即minor loop一次循环总共传输n字节, 每次读/写n字节,minor loop循环次数为1;
 * n由transfer_size映射得到;如kDmaTransferSize4B时n=4
 * 内存到外设的传输调用顺序:
 * 1. 调用本函数
 * 2. 调用TDrvDmaChSetSrcAddr设置源内存地址(因内部一次传输n字节,故源内存地址以n字节对齐)
 * 3. 调用TDrvDmaChSetMajorLoopCount设置要传输的字节数
 * 4. 调用TDrvDmaChEnableRequest来使能DMA请求功能
 * 5. 主循环结束或者错误后重复2-4
 * @param self: 对象实例指针
 * @param periph_addr: 外设地址
 * @param transfer_size: 一次传输的字节数对应的类型(注: 设置为源和目标都是这个类型,如需单独设置,需要调用TDrvDmaChSetAsCustom)
 *                    -# 参见[drv_dma](@ref TDmaTransferSize)
 * @return 当返回值 >= 0 时, 表示成功 \n
 *         当返回值 < 0 时, 表示失败, 参见[hcc_error_code_drv_soc_common](@ref THccDrvSocCommonTag)
 */
Int32 TDrvDmaChSetAsWritePeriph(TDrvDmaCh *self, UInt32 periph_addr, UInt8 transfer_size);

/**
 * DMA通道配置为用户自定义通道
 * 调用顺序:
 * 1. 调用本函数
 * 2. 调用TDrvDmaChSetSrcAddr设置源内存地址
 * 3. 调用TDrvDmaChSetDstAddr设置目标内存地址
 * 4. 调用TDrvDmaChSetMajorLoopCount设置要传输的字节数
 * 5. 调用TDrvDmaChEnableRequest来使能DMA请求功能
 * 6. 主循环结束或者错误后重复2-5
 * @param self: 对象实例指针
 * @param src_ofs: 源地址读取一次后源地址加上的偏移量,可以是负数.saddr += src_ofs; 注: ofs必须src_type所示的字节数的整数倍
 * @param dst_ofs: 目标地址写入一次后目标地址加上的偏移量,可以是负数.daddr += dst_ofs; 注: ofs必须dst_type所示的字节数的整数倍
 * @param src_type: 源地址类型,相当于Byte, Word, DWord, 16字节, 32字节. 源地址,src_ofs和minor_size都必须是src_type类型的整数倍
 *               -# 参见[drv_dma](@ref TDmaTransferSize)
 * @param dst_type: 目标地址类型,相当于Byte, Word, DWord, 16字节, 32字节. 目标地址,dst_ofs和minor_size都必须是dst_type类型的整数倍
 *               -# 参见[drv_dma](@ref TDmaTransferSize)
 * @return 当返回值 >= 0 时, 表示成功 \n
 *         当返回值 < 0 时, 表示失败, 参见[hcc_error_code_drv_soc_common](@ref THccDrvSocCommonTag)
 */
Int32 TDrvDmaChSetAsCustom(TDrvDmaCh *self, Int32 src_ofs, Int32 dst_ofs, UInt8 src_type, UInt8 dst_type);

/**
 * 设置源地址
 * @param self: 对象实例指针
 * @param addr: 源地址; 注: 必须按src_type类型对齐, 地址不能在SRAM_L内
 * @return 当返回值 >= 0 时, 表示成功 \n
 *         当返回值 < 0 时, 表示失败, 参见[hcc_error_code_drv_soc_common](@ref THccDrvSocCommonTag)
 */
Int32 TDrvDmaChSetSrcAddr(TDrvDmaCh *self, Int32 addr);

/**
 * 设置目标地址
 * @param self: 对象实例指针
 * @param addr: 目标地址; 注: 必须按dst_type类型对齐, 地址不能在SRAM_L内
 * @return 当返回值 >= 0 时, 表示成功 \n
 *         当返回值 < 0 时, 表示失败, 参见[hcc_error_code_drv_soc_common](@ref THccDrvSocCommonTag)
 */
Int32 TDrvDmaChSetDstAddr(TDrvDmaCh *self, Int32 addr);

/**
 * 设置主循环次数
 * @note:
 * > 1. 这里是指循环次数,而不是传输的字节数,具体字节数为count乘以minor loop一次传输的字节数
 * > 2. 该函数必须在TDrvDmaChSetAsReadPeriph或者TDrvDmaChSetAsWritePeriph或者TDrvDmaChSetAsCustom后调用
 * @param self: 对象实例指针
 * @param count: 循环次数
 * @return 当返回值 >= 0 时, 表示成功 \n
 *         当返回值 < 0 时, 表示失败, 参见[hcc_error_code_drv_soc_common](@ref THccDrvSocCommonTag)
 */
Int32 TDrvDmaChSetMajorLoopCount(TDrvDmaCh *self, Int32 count);

/**
 * 获取主循环次数和剩余循环次数
 * @note
 * > 1. 已经执行的循环次数 = *count - *remain
 * > 2. 未传输的字节数 = *remain * 单次传输字节数
 * @param self: 对象实例指针
 * @param count: 返回主循环次数; 可以为NULL
 * @param remain: 返回剩余循环次数; 可以为NULL;
 * @return 当返回值 >= 0 时, 表示成功 \n
 *         当返回值 < 0 时, 表示失败, 参见[hcc_error_code_drv_soc_common](@ref THccDrvSocCommonTag)
 */
Int32 TDrvDmaChGetMajorLoopCount(TDrvDmaCh *self, UInt32 *count, UInt32 *remain);

/**
 * 使能DMA通道请求功能
 * @note
 * > 1. 默认已经设置为主循环结束后自动禁用DMA通道请求功能;即每次主循环结束后,需重新调用本函数
 * > 2. 如果通过TDrvDmaSetAutoDisableReq禁用后,则调用一次本函数后,无需再次调用
 * @param self: 对象实例
 * @return 当返回值 >= 0 时, 表示成功 \n
 *         当返回值 < 0 时, 表示失败, 参见[hcc_error_code_drv_soc_common](@ref THccDrvSocCommonTag)
 */
Int32 TDrvDmaChEnableRequest(TDrvDmaCh *self);

/**
 * 禁用DMA通道请求功能
 * @param self: 对象实例
 * @return 当返回值 >= 0 时, 表示成功 \n
 *         当返回值 < 0 时, 表示失败, 其它参见[hcc_error_code_drv_soc_common](@ref THccDrvSocCommonTag)
 */
Int32 TDrvDmaChDisableRequest(TDrvDmaCh *self);

// DMA控制器~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

/**
 * DMA驱动. 该对象负责初始化DMA控制器.创建DMA通道时也需要用到该实例
 * note: 在使用DMA通道前需要先创建该类的实例,只需要创建一次即可.
 * @par 示例
 * @code
   TDrvDmaCreate(&g_drv_dma);
   ...
   TDrvDmaOpen(&g_drv_dma);
   ...
   // Do Channel Operation
   ...
   TDrvDmaClose(&g_drv_dma);
   ...
   TDrvDmaDestroy(&g_drv_dma);
 * @endcode
*/
struct TDrvDmaTag
{
PRIVATE
    UInt32 cross_tag_;          ///< 越界标记
    UInt8 c_ref_;               ///< 构造函数的引用计数
    UInt8 o_ref_;               ///< 打开函数的引用计数
};

extern TDrvDma g_drv_dma;  ///< DMA驱动对象

/**
 * Dma控制器构造函数
 * @param self: 对象实例指针
 * @return 当返回值 >= 0 时, 表示成功 \n
 *         当返回值 < 0 时, 表示失败, 参见[hcc_error_code_drv_soc_common](@ref THccDrvSocCommonTag)
 */
Int32 TDrvDmaCreate(TDrvDma *self);

/**
 * 析构函数
 * @param self: 对象实例指针
 * @return 当返回值 >= 0 时, 表示成功 \n
 *         当返回值 < 0 时, 表示失败, 参见[hcc_error_code_drv_soc_common](@ref THccDrvSocCommonTag)
 */
Int32 TDrvDmaDestroy(TDrvDma *self);

/**
 * 打开DMA驱动
 * @param self: 对象实例指针
 * @return 当返回值 >= 0 时, 表示成功 \n
 *         当返回值 < 0 时, 表示失败, 参见[hcc_error_code_drv_soc_common](@ref THccDrvSocCommonTag)
 */
Int32 TDrvDmaOpen(TDrvDma *self);

/**
 * 关闭DMA驱动
 * @note 释放当前控制器打开引用；已打开的 DMA 通道必须先由其所有者关闭。
 * @param self: 对象实例指针
 * @return 当返回值 >= 0 时, 表示成功 \n
 *         当返回值 < 0 时, 表示失败, 参见[hcc_error_code_drv_soc_common](@ref THccDrvSocCommonTag)
 */
Int32 TDrvDmaClose(TDrvDma *self);
/// --------------------------------- 必要接口 end ---------------------------------

/// --------------------------------- 特殊接口 start ---------------------------------
/// --------------------------------- 特殊接口 end ---------------------------------

#ifdef __cplusplus
}  // extern "C"
#endif

#endif // _DRV_DMA_H_
