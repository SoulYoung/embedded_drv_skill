#include "drv_flash.h"
#include "drv_common.h"

TDrvFlash g_drv_flash;  ///< 单例模式: 全局FLASH驱动对象

static Int32 TDrvFlashDoOpen(TDrvFlash *self)
{
    Int32 result = kHccDrvSocCommonSuccessLine;
    if (self->o_ref_ == 0)
    {
        // TODO: 解锁Flash控制器，配置低压保护，等待控制器Ready后重新锁定。
        self->cross_tag_ = kDrvMemCrossTagOpen;
    }
    self->o_ref_++;
    return result;
}

static Int32 TDrvFlashDoClose(TDrvFlash *self)
{
    Int32 result = kHccDrvSocCommonSuccessLine;
    if (self->o_ref_ == 0)
        result = kHccDrvSocCommonOutOfRangeLine;
    else
    {
        self->o_ref_--;
        if (self->o_ref_ == 0)
        {
            // TODO: 按目标MCU要求锁定Flash控制器或关闭仅供Flash使用的资源。
            self->cross_tag_ = kDrvMemCrossTagClose;
        }
    }
    return result;
}

static Bool TDrvFlashCheckRange(Int32 offset, Int32 size)
{
    Bool result = False;
    if (offset >= 0 && size > 0)
    {
        // TODO: 按目标MCU可编程区域、地址边界和跨区规则检查范围。
        result = True;
    }
    return result;
}

static Bool TDrvFlashSectorLegal(Int32 offset, Int32 size)
{
    Bool result = False;
    if (offset >= 0 && size > 0)
    {
        // TODO: 检查目标MCU擦除扇区对齐。
        result = True;
    }
    return result;
}

Int32 TDrvFlashCreate(TDrvFlash *self)
{
    Int32 result = kHccDrvSocCommonSuccessLine;
    if (self == NULL)
        result = kHccDrvSocCommonParamLine;
    else
    {
        if (self->c_ref_ == 0)
        {
            self->cross_tag_ = kDrvMemCrossTagCreate;
            self->o_ref_ = 0;
            self->write_mode_ = TDrvFlashWriteModePage;
        }
        self->c_ref_++;
    }
    return result;
}

Int32 TDrvFlashDestroy(TDrvFlash *self)
{
    Int32 result = kHccDrvSocCommonSuccessLine;
    if (self == NULL || self->c_ref_ == 0)
        result = kHccDrvSocCommonParamLine;
    else
    {
        if (self->o_ref_ != 0)
            TDrvFlashClose(self);
        self->c_ref_--;
        if (self->c_ref_ == 0)
            self->cross_tag_ = kDrvMemCrossTagDestroy;
    }
    return result;
}

Int32 TDrvFlashOpen(TDrvFlash *self)
{
    Int32 result = kHccDrvSocCommonSuccessLine;
    if (self == NULL || self->c_ref_ == 0)
        result = kHccDrvSocCommonParamLine;
    else
        result = TDrvFlashDoOpen(self);
    return result;
}

Int32 TDrvFlashClose(TDrvFlash *self)
{
    Int32 result = kHccDrvSocCommonSuccessLine;
    if (self == NULL)
        result = kHccDrvSocCommonParamLine;
    else
        result = TDrvFlashDoClose(self);
    return result;
}

Int32 TDrvFlashGetInfo(TDrvFlash *self, TDrvFlashInfo *info)
{
    Int32 result = kHccDrvSocCommonSuccessLine;
    if (self == NULL || info == NULL)
        result = kHccDrvSocCommonParamLine;
    else if (self->o_ref_ == 0)
        result = kHccDrvSocCommonClosedLine;
    else
    {
        // TODO: 按手册填写块数、每块扇区数、每扇区页数和页编程粒度。
        result = kHccDrvSocCommonNotSupportLine;
    }
    return result;
}

Int32 TDrvFlashRead(TDrvFlash *self, Int32 offset, void *buf, Int32 size)
{
    Int32 result = kHccDrvSocCommonSuccessLine;
    if (self == NULL || buf == NULL || offset < 0 || size <= 0)
        result = kHccDrvSocCommonParamLine;
    else if (!TDrvFlashCheckRange(offset, size))
        result = kHccDrvSocCommonOutOfRangeLine;
    else if (self->o_ref_ == 0)
        result = kHccDrvSocCommonClosedLine;
    else
    {
        // TODO: 按目标MCU映射地址或读命令读取Flash，写入buf。
        result = kHccDrvSocCommonNotSupportLine;
    }
    return result;
}

Int32 TDrvFlashWrite(TDrvFlash *self, Int32 offset, const void *data, Int32 size)
{
    Int32 result = kHccDrvSocCommonSuccessLine;
    if (self == NULL || data == NULL || offset < 0 || size <= 0)
        result = kHccDrvSocCommonParamLine;
    else if (!TDrvFlashCheckRange(offset, size))
        result = kHccDrvSocCommonOutOfRangeLine;
    else if (self->o_ref_ == 0)
        result = kHccDrvSocCommonClosedLine;
    else
    {
        // 按页进行编程。
        // TODO: 校验页对齐，逐页检查擦除状态，执行页/Section编程，等待完成并处理错误标志。
        result = kHccDrvSocCommonNotSupportLine;
    }
    return result;
}

Int32 TDrvFlashErase(TDrvFlash *self, Int32 offset, Int32 size)
{
    Int32 result = kHccDrvSocCommonSuccessLine;
    if (self == NULL || offset < 0 || size <= 0)
        result = kHccDrvSocCommonParamLine;
    else if (!TDrvFlashSectorLegal(offset, size))
        result = kHccDrvSocCommonNotAlignedLine;
    else if (!TDrvFlashCheckRange(offset, size))
        result = kHccDrvSocCommonOutOfRangeLine;
    else if (self->o_ref_ == 0)
        result = kHccDrvSocCommonClosedLine;
    else
    {
        // 按扇区进行擦除。
        // TODO: 逐扇区发起擦除命令，等待完成并处理错误标志。
        result = kHccDrvSocCommonNotSupportLine;
    }
    return result;
}

Int32 TDrvFlashVerify(TDrvFlash *self, Int32 offset, Int32 size)
{
    Int32 result = kHccDrvSocCommonSuccessLine;
    if (self == NULL || offset < 0 || size <= 0)
        result = kHccDrvSocCommonParamLine;
    else if (!TDrvFlashCheckRange(offset, size))
        result = kHccDrvSocCommonOutOfRangeLine;
    else if (self->o_ref_ == 0)
        result = kHccDrvSocCommonClosedLine;
    else
    {
        // TODO: 采用目标MCU校验命令或读取比较方式验证区域。
        result = kHccDrvSocCommonNotSupportLine;
    }
    return result;
}

static Int32 TDrvFlashDoSpecial(TDrvFlash *self)
{
    Int32 result = kHccDrvSocCommonSuccessLine;
    if (self == NULL)
        result = kHccDrvSocCommonParamLine;
    else if (self->o_ref_ == 0)
        result = kHccDrvSocCommonClosedLine;
    else
        result = kHccDrvSocCommonNotSupportLine;
    return result;
}

Int32 TDrvFlashEraseBlock(TDrvFlash *self, TDrvFlashBlockType block)
{
    (void)block;
    // TODO: 擦除目标块。
    return TDrvFlashDoSpecial(self);
}

Int32 TDrvFlashEraseAll(TDrvFlash *self)
{
    // TODO: 擦除所有允许擦除的块。
    return TDrvFlashDoSpecial(self);
}

Int32 TDrvFlashSetPartition(TDrvFlash *self, UInt8 partition)
{
    (void)partition;
    // TODO: 设置目标MCU分区。
    return TDrvFlashDoSpecial(self);
}

Int32 TDrvFlashErasePartition(TDrvFlash *self)
{
    // TODO: 擦除目标MCU分区信息。
    return TDrvFlashDoSpecial(self);
}

Int32 TDrvFlashEraseInfo(TDrvFlash *self, UInt8 type)
{
    (void)type;
    // TODO: 擦除目标MCU信息区。
    return TDrvFlashDoSpecial(self);
}

Int32 TDrvFlashSetWriteProtect(TDrvFlash *self, Bool enable, UInt8 *protect)
{
    (void)enable;
    (void)protect;
    // TODO: 设置写保护。
    return TDrvFlashDoSpecial(self);
}

Int32 TDrvFlashSetReadProtect(TDrvFlash *self)
{
    // TODO: 设置读保护。
    return TDrvFlashDoSpecial(self);
}

Int32 TDrvFlashSetBackdoorKey(TDrvFlash *self, const UInt8 *backdoor_key)
{
    (void)backdoor_key;
    // TODO: 设置后门密钥。
    return TDrvFlashDoSpecial(self);
}

Int32 TDrvFlashBackdoorAuthorize(TDrvFlash *self, const UInt8 *backdoor_key)
{
    (void)backdoor_key;
    // TODO: 执行后门认证。
    return TDrvFlashDoSpecial(self);
}

Int32 TDrvFlashSetWriteMode(TDrvFlash *self, UInt8 mode)
{
    Int32 result = kHccDrvSocCommonSuccessLine;
    if (self == NULL || mode > TDrvFlashWriteModeSection)
        result = kHccDrvSocCommonParamLine;
    else if (self->o_ref_ != 0)
        result = kHccDrvSocCommonOpenedLine;
    else
        self->write_mode_ = mode;
    return result;
}
