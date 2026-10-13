/**
 * @file drv_flash.h
 * @brief 内部Flash驱动
 * @note
 * TODO: 根据目标MCU用户手册填写可编程区域、地址、容量、写入粒度和擦除粒度。
 * TODO: 根据目标MCU能力确认是否支持分区切换、安全区、保护和其它特殊功能。
 * @par 修改日志
 * <table>
 * <tr><th>Date             <th>Version          <th>Author          <th>Description
 * <tr><td>2025-12-09       <td>1.0              <td>软件工程院       <td>创建初始版本
 * <tr><td>2026-04-27       <td>2.0              <td>软件工程院       <td>增加信息安全相关函数
 * @copyright 杭州鸿泉物联网技术股份有限公司
 */
#ifndef _DRV_FLASH_H_
#define _DRV_FLASH_H_

#include <ofc_types.h>
#include <ofc_object.h>

#ifdef __cplusplus
extern "C" {
#endif

enum TDrvFlashAttrPartValTag
{
    TDrvFlashAttrPart_0Byte      = 0,
    TDrvFlashAttrPart_128Byte,
    TDrvFlashAttrPart_256Byte,
    TDrvFlashAttrPart_512Byte,
    TDrvFlashAttrPartVal_max,
};

enum TDrvFlashInfoTypeTag
{
    TDrvFlashInfoType0 = 0, ///0x1020000 - 0x1020800
    TDrvFlashInfoType1 = 1, ///0x1020800 - 0x1021000
};

/**
 * 芯片容量相关的信息, 扇区为基本擦除单位
 * 扇区的大小为 pages_per_sector * page_size
 * 总扇区数为 blocks * sectors_per_block
 */
struct TDrvFlashInfoTag
{
    UInt16 blocks;              ///< 芯片包含的块数
    UInt16 sectors_per_block;   ///< 每个块包含的扇区数
    UInt16 pages_per_sector;    ///< 每个扇区包含的页数
    UInt16 page_size;           ///< 编程时的页大小; 页编程时,写入的数据必须在同一个页的范围内
};
typedef struct TDrvFlashInfoTag TDrvFlashInfo;

enum TDrvFlashBlockTypeTag
{
    TDrvFlashBlockTypePflash = 0,  ///< P-Flash
    TDrvFlashBlockTypeDflash = 1,  ///< D-Flash
};
typedef enum TDrvFlashBlockTypeTag TDrvFlashBlockType;

enum TDrvFlashWriteModeTag
{
    TDrvFlashWriteModePage = 0,  ///< 页编程方式
    TDrvFlashWriteModeSection = 1,  ///< Section编程方式
};
typedef enum TDrvFlashWriteModeTag TDrvFlashWriteMode;

typedef struct TDrvFlashTag TDrvFlash;
/**
 * 构造一个Flash和EEPROM的读写对象
 * @par 示例
 * @code
   TDrvFlashCreate(&g_drv_flash);
   ...
   TDrvFlashOpen(&g_drv_flash);
   ...
   // TDrvFlashRead...
   // TDrvFlashErase...
   // TDrvFlashWrite...
   // ...
   TDrvFlashClose(&g_drv_flash);
   ...
   TDrvFlashDestroy(&g_drv_flash);
 * @endcode
*/
struct TDrvFlashTag
{
PRIVATE
    UInt32 cross_tag_;                      ///< 越界标记
    /// 2025.12.12 由于常见mcu均有pflash和dflash概念 且引用的都是同一个实例 所以将is_open_去掉 增加引用计数
    UInt8 c_ref_;                           ///< 构造函数的引用计数
    UInt8 o_ref_;                           ///< 打开函数的引用计数
    UInt8 write_mode_;                      ///< 写入模式, 0:页编程, 1:扇区编程
};

extern TDrvFlash g_drv_flash;               ///< 单例模式: 全局FLASH驱动对象

/// --------------------------------- 必要接口 start ---------------------------------
/**
 * 驱动创建
 * @param self: 对象实例指针
 * @return 当返回值 >= 0 时, 表示成功 \n
 *         当返回值 < 0 时, 表示失败, 参见[THccDrvSocCommon](@ref THccDrvSocCommonTag)
 */
Int32 TDrvFlashCreate(TDrvFlash *self);

/**
 * 驱动析构
 * @param self: 对象实例指针
 * @return 当返回值 >= 0 时, 表示成功 \n
 *         当返回值 < 0 时, 表示失败, 参见[THccDrvSocCommon](@ref THccDrvSocCommonTag)
 */
Int32 TDrvFlashDestroy(TDrvFlash *self);

/**
 * 驱动打开
 * @param self: 对象实例指针
 * @return 当返回值 >= 0 时, 表示成功 \n
 *         当返回值 < 0 时, 表示失败, 参见[THccDrvSocCommon](@ref THccDrvSocCommonTag)
 */
Int32 TDrvFlashOpen(TDrvFlash *self);

/**
 * 驱动关闭
 * @param self: 对象实例指针
 * @return 当返回值 >= 0 时, 表示成功 \n
 *         当返回值 < 0 时, 表示失败, 参见[THccDrvSocCommon](@ref THccDrvSocCommonTag)
 */
Int32 TDrvFlashClose(TDrvFlash *self);

/**
 * 函数返回Program flash的大小
 * @note 本函数返回一块block, 实际是不连续的两块, 通过不同的地址进行区分, 参考本文第4行
 * @param self: 对象实例指针
 * @param info: 信息缓存, 注意扇区是不连续的
 * @return 当返回值 >= 0 时, 表示成功 \n
 *         当返回值 < 0 时, 表示失败, 参见[THccDrvSocCommon](@ref THccDrvSocCommonTag)
 */
Int32 TDrvFlashGetInfo(TDrvFlash *self, TDrvFlashInfo *info);

/**
 * 读取Program Flash数据
 * @note 读数据深度可以 8bit 16bit 32bit读取
 * @param self: 对象实例指针
 * @param offset: 读取页的起始位置(实际物理地址), 随机地址读取, 参考本文第4行
 * @param buf: 存放数据的缓冲区
 * @param size: 缓冲区长度, 可任意长度
 * @return 当返回值 >= 0 时, 表示成功并实际读取的字节数 \n
 *         当返回值 < 0 时, 表示失败, 参见[THccDrvSocCommon](@ref THccDrvSocCommonTag)
 */
Int32 TDrvFlashRead(TDrvFlash *self, Int32 offset, void *buf, Int32 size);

/**
 * 写数据到Program Flash(页编程方式)
 * @note 编程失败时, 需要重新擦除
 * @param self: 对象实例指针
 * @param offset: 写入页的起始位置(实际物理地址),必须是页尺寸的整数倍, 参考本文第4行
 * @param data: 数据指针
 * @param size: 数据长度, 必须是页尺寸的整数倍
 * @return 当返回值 >= 0 时, 表示成功并实际写入的字节数 \n
 *         当返回值 < 0 时, 表示失败, 参见[THccDrvSocCommon](@ref THccDrvSocCommonTag)
 */
Int32 TDrvFlashWrite(TDrvFlash *self, Int32 offset, const void *data, Int32 size);

/**
 * 擦除扇区
 * @param self: 对象实例指针
 * @param offset: 擦除扇区的起始位置(实际物理地址), 必须扇区对齐, 参考本文件第4行
 * @param size: 擦除长度; 长度必须是扇区大小的整数倍
 * @return 当返回值 >= 0 时, 表示成功并实际擦除的字节数 \n
 *         当返回值 < 0 时, 表示失败, 参见[THccDrvSocCommon](@ref THccDrvSocCommonTag)
 */
Int32 TDrvFlashErase(TDrvFlash *self, Int32 offset, Int32 size);

/**
 * 验证指定区域是否全为0xff
 * @note 不能跨bank验证
 * @param self: 对象实例指针
 * @param offset: 起始位置(实际物理地址), 必须页对齐, 参考本文件第4行
 * @param size: 长度; 长度必须是页大小的整数倍
 * @return 当返回值 >= 0 时, 表示成功并实际全0xff的字节数 \n
 *         当返回值 < 0 时, 表示失败, 参见[THccDrvSocCommon](@ref THccDrvSocCommonTag)
 */
Int32 TDrvFlashVerify(TDrvFlash *self, Int32 offset, Int32 size);

/**
 * 擦除指定块
 * @param self: 对象实例指针
 * @param block: 块号 参见TDrvFlashBlockType
 * @return 当返回值 == 0 时, 表示成功
 *         当返回值 < 0 时, 表示失败, 参见[THccDrvSocCommon](@ref THccDrvSocCommonTag)
 */
Int32 TDrvFlashEraseBlock(TDrvFlash *self, TDrvFlashBlockType block);

/**
 * 擦除所有Flash
 * @param self: 对象实例指针
 * @return 当返回值 == 0 时, 表示成功
 *         当返回值 < 0 时, 表示失败, 参见[THccDrvSocCommon](@ref THccDrvSocCommonTag)
 */
Int32 TDrvFlashEraseAll(TDrvFlash *self);

/// --------------------------------- 必要接口 end ---------------------------------

/// --------------------------------- 特殊接口 start ---------------------------------
/// 平台特殊函数
Int32 TDrvFlashSetPartition(TDrvFlash *self, UInt8 partition);
Int32 TDrvFlashSetWriteMode(TDrvFlash *self, UInt8 mode);
Int32 TDrvFlashErasePartition(TDrvFlash *self);
Int32 TDrvFlashEraseInfo(TDrvFlash *self, UInt8 type);
Int32 TDrvFlashSetWriteProtect(TDrvFlash *self, Bool enable, UInt8 *protect);
Int32 TDrvFlashSetReadProtect(TDrvFlash *self);
Int32 TDrvFlashSetBackdoorKey(TDrvFlash *self, const UInt8 *backdoor_key);
Int32 TDrvFlashBackdoorAuthorize(TDrvFlash *self, const UInt8 *backdoor_key);
/// --------------------------------- 特殊接口 end ---------------------------------

#ifdef __cplusplus
}  // extern "C"
#endif

#endif //_DRV_FLASH_H_
