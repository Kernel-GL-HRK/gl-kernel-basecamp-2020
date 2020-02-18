#ifndef __TSL2580_REGS_H
#define __TSL2580_REGS_H

/* High 4 bits of device id */
#define TSL2580_LOW_ID 0b10000000
#define TSL2581_LOW_ID 0b10010000

/* TSL2580 registers */
#define TSL2580_CMD_REG 0x80
#define TSL2580_CTRL_REG 0x00
#define TSL2580_TIMING_REG 0x01
#define TSL2580_INT_REG 0x02
#define TSL2580_INT_THLLOW_REG 0x03
#define TSL2580_INT_THLHIGH_REG 0x04
#define TSL2580_INT_THHLOW_REG 0x05
#define TSL2580_INT_THHHIGH_REG 0x06
#define TSL2580_ANALOG_REG 0x07
#define TSL2580_ID_REG 0x12
#define TSL2580_CONSTANT_REG 0x13
#define TSL2580_DATA0LOW_REG 0x14
#define TSL2580_DATA0HIGH_REG 0x15
#define TSL2580_DATA1LOW_REG 0x16
#define TSL2580_DATA1HIGH_REG 0x17
#define TSL2580_TIMERLOW_REG 0x18
#define TSL2580_TIMERHIGH_REG 0x19

/* Type of transactions */
#define TSL2580_TRNS_BYTE 0x00
#define TSL2580_TRNS_WORD 0x20
#define TSL2580_TRNS_BLOCK 0x40
#define TSL2580_TRNS_SPECIAL 0x60

/* Control register commands */
#define TSL2580_CTRL_POWER 0x1
#define TSL2580_CTRL_ADC_EN 0x2
#define TSL2580_CTRL_ADC_VALID 0x10
#define TSL2580_CTRL_ADC_INTR 0x20

/* Timing register integration cycles (must be in 2's complement) */
#define TSL2580_TIMING_MANUAL 0
#define TSL2580_TIMING_1 (u8)(~(1) + 1)
#define TSL2580_TIMING_2 (u8)(~(2) + 1)
#define TSL2580_TIMING_19 (u8)(~(19) + 1)
#define TSL2580_TIMING_37 (u8)(~(37) + 1)
#define TSL2580_TIMING_74 (u8)(~(74) + 1)
#define TSL2580_TIMING_148 (u8)(~(148) + 1)
#define TSL2580_TIMING_255 (u8)(~(255) + 1)

/* Analog register gain options */
#define TSL2580_ANALOG_GAIN_1X 0x0
#define TSL2580_ANALOG_GAIN_8X 0x1
#define TSL2580_ANALOG_GAIN_16X 0x2
#define TSL2580_ANALOG_GAIN_111X 0x3

/* Interrupt register control select */
#define TSL2580_INT_CTRL_DISABLE 0x00
#define TSL2580_INT_CTRL_LEVEL (0b01 << 4)
#define TSL2580_INT_CTRL_SMB (0b10 << 4)
#define TSL2580_INT_CTRL_TEST (0b11 << 4)

/* Interrupt register stop ADC integration on interrupt */
#define TSL2580_INT_CTRL_INTR_STOP 1 << 6

/* Interrupt register persistance select */
#define TSL2580_INT_PRST_ADC 0x00
#define TSL2580_INT_PRST_TH 0x01
#define TSL2580_INT_PRST_2 0x02
#define TSL2580_INT_PRST_3 0x03
#define TSL2580_INT_PRST_4 0x04
#define TSL2580_INT_PRST_5 0x05
#define TSL2580_INT_PRST_6 0x06
#define TSL2580_INT_PRST_7 0x07
#define TSL2580_INT_PRST_8 0x08
#define TSL2580_INT_PRST_9 0x09
#define TSL2580_INT_PRST_10 0x0A
#define TSL2580_INT_PRST_11 0x0B
#define TSL2580_INT_PRST_12 0x0C
#define TSL2580_INT_PRST_13 0x0D
#define TSL2580_INT_PRST_14 0x0E
#define TSL2580_INT_PRST_15 0x0F

/* Command register special function */
#define TSL2580_CMD_INT_CLR 0x01
#define TSL2580_CMD_STOP_MI 0x02
#define TSL2580_CMD_START_MI 0x03

#endif
