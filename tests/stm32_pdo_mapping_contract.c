#include "../EtherCAT/STM32PDO.h"

/* DI0/DO0 must map to bit 0 and DI7/DO7 must map to bit 7. */
_Static_assert(
    STM32_PDO_PACK_8_BITS(1U, 0U, 1U, 0U, 0U, 1U, 0U, 1U) == 0xA5U,
    "PDO bit packing must preserve the SSC object-dictionary bit order");

_Static_assert(STM32_PDO_GET_BIT(0xA5U, 0U) == 1U, "bit 0 must be set");
_Static_assert(STM32_PDO_GET_BIT(0xA5U, 1U) == 0U, "bit 1 must be clear");
_Static_assert(STM32_PDO_GET_BIT(0xA5U, 7U) == 1U, "bit 7 must be set");

/* PE8-PE15 keys are active-low; PE0-PE7 LEDs are active-high. */
_Static_assert(STM32_ACTIVE_LOW_TO_LOGICAL(0U) == 1U, "low key level means pressed");
_Static_assert(STM32_ACTIVE_LOW_TO_LOGICAL(1U) == 0U, "high key level means released");
_Static_assert(STM32_ACTIVE_HIGH_FROM_LOGICAL(0U) == 0U, "false output turns LED off");
_Static_assert(STM32_ACTIVE_HIGH_FROM_LOGICAL(1U) == 1U, "true output turns LED on");
