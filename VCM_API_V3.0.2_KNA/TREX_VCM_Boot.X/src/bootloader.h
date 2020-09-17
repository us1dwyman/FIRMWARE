/**
 * Definitions for bootloader protocol
 *
 * See WP 18CS07-01 PIC UART Bootloader.docx for information about these values
 *
 * @private
 * @copyright Copyright (c) 2019 Control Solutions LLC.  All Rights Reserved.
 */

#ifndef _BOOTLOADER_H_
#define _BOOTLOADER_H_

/**
 * OP Codes for Bootloader messaging with Host
 */
#define BL_OP_ENQUIRE 0xF0
#define BL_OP_PASSTHRU_ON 0xF1
#define BL_OP_PASSTHRU_OFF 0xF2
#define BL_OP_SELECT 0xF3

// These commands all have bit 3 set (0x08)
// so we can ignore them until proper command sequences have happened
#define BL_OP_ERASE 0xF8
#define BL_OP_DATA 0xF9
#define BL_OP_VERIFY 0xFA
#define BL_OP_RESET 0xFB
#define BL_OP_ERASE_ALL 0xFC
#define BL_OP_FINISH 0xFD

// responses
#define BL_OP_ACK 0x00
#define BL_OP_NACK 0xFF

// masks to apply to incoming op codes to see if we should accept them
// when 'locked', the op code must not have bit 3 (0x08) set
#define BL_OP_MASK_LOCKED 0xF7
#define BL_OP_MASK_ALL 0xFF

/**
 * The processor type (low nibble/4 bits max)
 */
#define BL_PROC_PIC16 0x00
#define BL_PROC_PIC18 0x01
#define BL_PROC_PIC24 0x02

/**
 * The platform (high nibble/4 bits max)
 * This is reported throught the ENQ response.
 * In systems where there is only one device possibly connected to the
 * host, it's OK to use the BL_PLAT_GENERIC.  In other systems, it can
 * be helpful to use a different platform ID (either to identify the system
 * or the specific device in the system) to help the host application verify
 * that it is doing the right thing.  Because the use of this depends on agreement
 * between the Host and the Bootloader, the specific implementation details are left to the system
 * designer.  Generally the Host should throw an error if it discovers it is talking
 * to an unexpected platform
 */
#define BL_PLAT_CS1435 0x00
#define BL_PLAT_TREX 0x10
#define BL_PLAT_GENERIC 0x20
#define BL_PLAT_PHOENIX 0x30

#endif
