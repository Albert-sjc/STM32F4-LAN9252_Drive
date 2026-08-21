#ifndef STM32PDO_H
#define STM32PDO_H

#include "ecat_def.h"

/* Convert one logical PDO value to a single bit at the requested position. */
#define STM32_PDO_BIT(value, position) \
    ((UINT8)((((value) != 0U) ? 1U : 0U) << (position)))

/* Pack object 0x6000/0x7000 subindices 1-8 into one PDO byte. */
#define STM32_PDO_PACK_8_BITS(bit0, bit1, bit2, bit3, bit4, bit5, bit6, bit7) \
    ((UINT8)(STM32_PDO_BIT((bit0), 0U) |                                   \
             STM32_PDO_BIT((bit1), 1U) |                                   \
             STM32_PDO_BIT((bit2), 2U) |                                   \
             STM32_PDO_BIT((bit3), 3U) |                                   \
             STM32_PDO_BIT((bit4), 4U) |                                   \
             STM32_PDO_BIT((bit5), 5U) |                                   \
             STM32_PDO_BIT((bit6), 6U) |                                   \
             STM32_PDO_BIT((bit7), 7U)))

/* Extract one logical object value from a received PDO byte. */
#define STM32_PDO_GET_BIT(value, position) \
    ((UINT8)(((UINT8)(value) >> (position)) & 0x01U))

/* PE8-PE15 keys use pull-ups: GPIO low means the key is pressed. */
#define STM32_ACTIVE_LOW_TO_LOGICAL(level) \
    ((UINT8)(((level) == 0U) ? 1U : 0U))

/* PE0-PE7 LEDs are active-high: logical true means GPIO high. */
#define STM32_ACTIVE_HIGH_FROM_LOGICAL(value) \
    ((UINT8)(((value) != 0U) ? 1U : 0U))

#endif /* STM32PDO_H */
