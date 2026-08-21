#include "../EtherCAT/SPIDriver/STM32F407/STM32SPIDriver.h"

void Test_ClientCannotUseRemovedSPIPut(void)
{
    SPIPut(0U);
}
