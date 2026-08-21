#include "../EtherCAT/SPIDriver/SPIDriver.h"

/*
 * 通用 SPI 驱动头文件必须独立提供 SPIDriver.c 使用的数据拆分类型，
 * 不能要求调用者额外包含某个 MCU 平台的专用头文件。
 */
void Test_SPIDriverCommonTypesAreAvailable(void)
{
    UINT16_VAL value16 = {0};
    UINT32_VAL value32 = {0};
    UINT64_VAL value64 = {0};

    (void)value16;
    (void)value32;
    (void)value64;
}
