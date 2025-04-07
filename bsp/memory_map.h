/**
\brief SCuM board's addresses of the memory-mapped registers.

\author Sahar Mesri     <smesri@berkeley.edu>       August 2016.
\author Tengfei Chang   <tengfei.chang@inria.fr>    August 2016.
*/

// ========================== AHB Peripheral ==================================

#define AHB_BOOTLOAD_BASE 0x01000000
#define AHB_DATAMEM_BASE 0x20000000
#define AHB_RF_BASE 0x40000000
#define AHB_DMA_BASE 0x41000000
#define AHB_RFTIMER_BASE 0x42000000

// ========================== APB Peripheral ==================================

#define APB_ADC_BASE 0x50000000
#define APB_UART_BASE 0x51000000
#define APB_ANALOG_CFG_BASE 0x52000000
#define APB_GPIO_BASE 0x53000000

// ========================== RFCONTRLLER Registers ===========================

#define RFCONTROLLER_REG__CONTROL *(volatile unsigned int*)(AHB_RF_BASE + 0x00)
#define RFCONTROLLER_REG__STATUS *(volatile unsigned int*)(AHB_RF_BASE + 0x04)
#define RFCONTROLLER_REG__TX_DATA_ADDR *(volatile char**)(AHB_RF_BASE + 0x08)
#define RFCONTROLLER_REG__TX_PACK_LEN *(volatile unsigned int*)(AHB_RF_BASE + 0x0C)
#define RFCONTROLLER_REG__INT *(volatile unsigned int*)(AHB_RF_BASE + 0x10)
#define RFCONTROLLER_REG__INT_CONFIG *(volatile unsigned int*)(AHB_RF_BASE + 0x14)
#define RFCONTROLLER_REG__INT_CLEAR *(volatile unsigned int*)(AHB_RF_BASE + 0x18)
#define RFCONTROLLER_REG__ERROR *(volatile unsigned int*)(AHB_RF_BASE + 0x1C)
#define RFCONTROLLER_REG__ERROR_CONFIG *(volatile unsigned int*)(AHB_RF_BASE + 0x20)
#define RFCONTROLLER_REG__ERROR_CLEAR *(volatile unsigned int*)(AHB_RF_BASE + 0x24)

// ==== RFCONTROLLER interruption bit configuration

#define TX_LOAD_DONE_INT_EN 0x0001
#define TX_SFD_DONE_INT_EN 0x0002
#define TX_SEND_DONE_INT_EN 0x0004
#define RX_SFD_DONE_INT_EN 0x0008
#define RX_DONE_INT_EN 0x0010
#define TX_LOAD_DONE_RFTIMER_PULSE_EN 0x0020
#define TX_SFD_DONE_RFTIMER_PULSE_EN 0x0040
#define TX_SEND_DONE_RFTIMER_PULSE_EN 0x0080
#define RX_SFD_DONE_RFTIMER_PULSE_EN 0x0100
#define RX_DONE_RFTIMER_PULSE_EN 0x0200

// ==== RFCONTROLLER error bit configuration

#define TX_OVERFLOW_ERROR_EN 0x001
#define TX_CUTOFF_ERROR_EN 0x002
#define RX_OVERFLOW_ERROR_EN 0x004
#define RX_CRC_ERROR_EN 0x008
#define RX_CUTOFF_ERROR_EN 0x010

// ==== RFCONTROLLER control operation

#define TX_LOAD 0x01
#define TX_SEND 0x02
#define RX_START 0x04
#define RX_STOP 0x08
#define RF_RESET 0x10

// ==== RFCONTROLLER interruption flag

#define TX_LOAD_DONE_INT 0x01
#define TX_SFD_DONE_INT 0x02
#define TX_SEND_DONE_INT 0x04
#define RX_SFD_DONE_INT 0x08
#define RX_DONE_INT 0x10

// ==== RFCONTROLLER error flag

#define TX_OVERFLOW_ERROR 0x01
#define TX_CUTOFF_ERROR 0x02
#define RX_OVERFLOW_ERROR 0x04
#define RX_CRC_ERROR 0x08
#define RX_CUTOFF_ERROR 0x10

// ========================== RFTIMER Registers ===============================

#define RFTIMER_REG__COMPARE0_ADDR (unsigned int*)(AHB_RFTIMER_BASE + 0x10)
#define RFTIMER_REG__COMPARE1_ADDR (unsigned int*)(AHB_RFTIMER_BASE + 0x14)
#define RFTIMER_REG__COMPARE2_ADDR (unsigned int*)(AHB_RFTIMER_BASE + 0x18)
#define RFTIMER_REG__COMPARE3_ADDR (unsigned int*)(AHB_RFTIMER_BASE + 0x1C)
#define RFTIMER_REG__COMPARE4_ADDR (unsigned int*)(AHB_RFTIMER_BASE + 0x20)
#define RFTIMER_REG__COMPARE5_ADDR (unsigned int*)(AHB_RFTIMER_BASE + 0x24)
#define RFTIMER_REG__COMPARE6_ADDR (unsigned int*)(AHB_RFTIMER_BASE + 0x28)
#define RFTIMER_REG__COMPARE7_ADDR (unsigned int*)(AHB_RFTIMER_BASE + 0x2C)

#define RFTIMER_REG__COMPARE0_CONTROL_ADDR \
    (unsigned int*)(AHB_RFTIMER_BASE + 0x30)
#define RFTIMER_REG__COMPARE1_CONTROL_ADDR \
    (unsigned int*)(AHB_RFTIMER_BASE + 0x34)
#define RFTIMER_REG__COMPARE2_CONTROL_ADDR \
    (unsigned int*)(AHB_RFTIMER_BASE + 0x38)
#define RFTIMER_REG__COMPARE3_CONTROL_ADDR \
    (unsigned int*)(AHB_RFTIMER_BASE + 0x3C)
#define RFTIMER_REG__COMPARE4_CONTROL_ADDR \
    (unsigned int*)(AHB_RFTIMER_BASE + 0x40)
#define RFTIMER_REG__COMPARE5_CONTROL_ADDR \
    (unsigned int*)(AHB_RFTIMER_BASE + 0x44)
#define RFTIMER_REG__COMPARE6_CONTROL_ADDR \
    (unsigned int*)(AHB_RFTIMER_BASE + 0x48)
#define RFTIMER_REG__COMPARE7_CONTROL_ADDR \
    (unsigned int*)(AHB_RFTIMER_BASE + 0x4C)

#define RFTIMER_REG__CONTROL *(volatile unsigned int*)(AHB_RFTIMER_BASE + 0x00)
#define RFTIMER_REG__COUNTER *(volatile unsigned int*)(AHB_RFTIMER_BASE + 0x04)
#define RFTIMER_REG__MAX_COUNT *(volatile unsigned int*)(AHB_RFTIMER_BASE + 0x08)
#define RFTIMER_REG__COMPARE0 *(volatile unsigned int*)(AHB_RFTIMER_BASE + 0x10)
#define RFTIMER_REG__COMPARE1 *(volatile unsigned int*)(AHB_RFTIMER_BASE + 0x14)
#define RFTIMER_REG__COMPARE2 *(volatile unsigned int*)(AHB_RFTIMER_BASE + 0x18)
#define RFTIMER_REG__COMPARE3 *(volatile unsigned int*)(AHB_RFTIMER_BASE + 0x1C)
#define RFTIMER_REG__COMPARE4 *(volatile unsigned int*)(AHB_RFTIMER_BASE + 0x20)
#define RFTIMER_REG__COMPARE5 *(volatile unsigned int*)(AHB_RFTIMER_BASE + 0x24)
#define RFTIMER_REG__COMPARE6 *(volatile unsigned int*)(AHB_RFTIMER_BASE + 0x28)
#define RFTIMER_REG__COMPARE7 *(volatile unsigned int*)(AHB_RFTIMER_BASE + 0x2C)
#define RFTIMER_REG__COMPARE0_CONTROL *(volatile unsigned int*)(AHB_RFTIMER_BASE + 0x30)
#define RFTIMER_REG__COMPARE1_CONTROL *(volatile unsigned int*)(AHB_RFTIMER_BASE + 0x34)
#define RFTIMER_REG__COMPARE2_CONTROL *(volatile unsigned int*)(AHB_RFTIMER_BASE + 0x38)
#define RFTIMER_REG__COMPARE3_CONTROL *(volatile unsigned int*)(AHB_RFTIMER_BASE + 0x3C)
#define RFTIMER_REG__COMPARE4_CONTROL *(volatile unsigned int*)(AHB_RFTIMER_BASE + 0x40)
#define RFTIMER_REG__COMPARE5_CONTROL *(volatile unsigned int*)(AHB_RFTIMER_BASE + 0x44)
#define RFTIMER_REG__COMPARE6_CONTROL *(volatile unsigned int*)(AHB_RFTIMER_BASE + 0x48)
#define RFTIMER_REG__COMPARE7_CONTROL *(volatile unsigned int*)(AHB_RFTIMER_BASE + 0x4C)
#define RFTIMER_REG__CAPTURE0 *(volatile unsigned int*)(AHB_RFTIMER_BASE + 0x50)
#define RFTIMER_REG__CAPTURE1 *(volatile unsigned int*)(AHB_RFTIMER_BASE + 0x54)
#define RFTIMER_REG__CAPTURE2 *(volatile unsigned int*)(AHB_RFTIMER_BASE + 0x58)
#define RFTIMER_REG__CAPTURE3 *(volatile unsigned int*)(AHB_RFTIMER_BASE + 0x5C)
#define RFTIMER_REG__CAPTURE0_CONTROL *(volatile unsigned int*)(AHB_RFTIMER_BASE + 0x60)
#define RFTIMER_REG__CAPTURE1_CONTROL *(volatile unsigned int*)(AHB_RFTIMER_BASE + 0x64)
#define RFTIMER_REG__CAPTURE2_CONTROL *(volatile unsigned int*)(AHB_RFTIMER_BASE + 0x68)
#define RFTIMER_REG__CAPTURE3_CONTROL *(volatile unsigned int*)(AHB_RFTIMER_BASE + 0x6C)
#define RFTIMER_REG__INT *(volatile unsigned int*)(AHB_RFTIMER_BASE + 0x70)
#define RFTIMER_REG__INT_CLEAR *(volatile unsigned int*)(AHB_RFTIMER_BASE + 0x74)

// ==== RFTIMER compare control bit

#define RFTIMER_COMPARE_ENABLE 0x01
#define RFTIMER_COMPARE_INTERRUPT_ENABLE 0x02
#define RFTIMER_COMPARE_TX_LOAD_ENABLE 0x04
#define RFTIMER_COMPARE_TX_SEND_ENABLE 0x08
#define RFTIMER_COMPARE_RX_START_ENABLE 0x10
#define RFTIMER_COMPARE_RX_STOP_ENABLE 0x20

// ==== RFTIMER capture control bit

#define RFTIMER_CAPTURE_INTERRUPT_ENABLE 0x01
#define RFTIMER_CAPTURE_INPUT_SEL_SOFTWARE 0x02
#define RFTIMER_CAPTURE_INPUT_SEL_TX_LOAD_DONE 0x04
#define RFTIMER_CAPTURE_INPUT_SEL_TX_SFD_DONE 0x08
#define RFTIMER_CAPTURE_INPUT_SEL_TX_SEND_DONE 0x10
#define RFTIMER_CAPTURE_INPUT_SEL_RX_SFD_DONE 0x20
#define RFTIMER_CAPTURE_INPUT_SEL_RX_DONE 0x40
#define RFTIMER_CAPTURE_NOW 0x80

// ==== RFTIMER control bit

#define RFTIMER_REG__CONTROL_ENABLE 0x01
#define RFTIMER_REG__CONTROL_INTERRUPT_ENABLE 0x02
#define RFTIMER_REG__CONTROL_COUNT_RESET 0x04

// ==== RFTIMER interruption flag

#define RFTIMER_REG__INT_COMPARE0_INT 0x0001
#define RFTIMER_REG__INT_COMPARE1_INT 0x0002
#define RFTIMER_REG__INT_COMPARE2_INT 0x0004
#define RFTIMER_REG__INT_COMPARE3_INT 0x0008
#define RFTIMER_REG__INT_COMPARE4_INT 0x0010
#define RFTIMER_REG__INT_COMPARE5_INT 0x0020
#define RFTIMER_REG__INT_COMPARE6_INT 0x0040
#define RFTIMER_REG__INT_COMPARE7_INT 0x0080
#define RFTIMER_REG__INT_CAPTURE0_INT 0x0100
#define RFTIMER_REG__INT_CAPTURE1_INT 0x0200
#define RFTIMER_REG__INT_CAPTURE2_INT 0x0400
#define RFTIMER_REG__INT_CAPTURE3_INT 0x0800
#define RFTIMER_REG__INT_CAPTURE0_OVERFLOW_INT 0x1000
#define RFTIMER_REG__INT_CAPTURE1_OVERFLOW_INT 0x2000
#define RFTIMER_REG__INT_CAPTURE2_OVERFLOW_INT 0x4000
#define RFTIMER_REG__INT_CAPTURE3_OVERFLOW_INT 0x8000

// ========================== DMA Registers ===================================

#define DMA_REG__RF_RX_ADDR *(volatile char**)(AHB_DMA_BASE + 0x14)

// ========================== ADC Registers ===================================

#define ADC_REG__START *(volatile unsigned int*)(APB_ADC_BASE + 0x000000)
#define ADC_REG__DATA *(volatile unsigned int*)(APB_ADC_BASE + 0x040000)

// ========================== UART Registers ==================================

#define UART_REG__TX_DATA *(volatile unsigned int*)(APB_UART_BASE)
#define UART_REG__RX_DATA *(volatile unsigned int*)(APB_UART_BASE)

// ========================== GPIO Registers ==================================

#define GPIO_REG__INPUT *(volatile unsigned int*)(APB_GPIO_BASE + 0x000000)
#define GPIO_REG__OUTPUT *(volatile unsigned int*)(APB_GPIO_BASE + 0x040000)

// ========================== Analog Configure Registers ======================

#define ANALOG_CFG_REG__0 *(volatile unsigned int*)(APB_ANALOG_CFG_BASE + 0x000000)
#define ANALOG_CFG_REG__1 *(volatile unsigned int*)(APB_ANALOG_CFG_BASE + 0x040000)
#define ANALOG_CFG_REG__2 *(volatile unsigned int*)(APB_ANALOG_CFG_BASE + 0x080000)
#define ANALOG_CFG_REG__3 *(volatile unsigned int*)(APB_ANALOG_CFG_BASE + 0x0C0000)
#define ANALOG_CFG_REG__4 *(volatile unsigned int*)(APB_ANALOG_CFG_BASE + 0x100000)
#define ANALOG_CFG_REG__5                  \
    *(volatile unsigned int*)(APB_ANALOG_CFG_BASE + \
                     0x140000)  // contains 2.4 GHz divider control, see
                                // bucket_o_functions/divProgram()
#define ANALOG_CFG_REG__6                  \
    *(volatile unsigned int*)(APB_ANALOG_CFG_BASE + \
                     0x180000)  // contains 2.4 GHz divider control, see
                                // bucket_o_functions/divProgram()
#define ANALOG_CFG_REG__7                  \
    *(volatile unsigned int*)(APB_ANALOG_CFG_BASE + \
                     0x1C0000)  // contains 2.4 GHz oscillator control, see
                                // bucket_o_functions/LC_freqchange
#define ANALOG_CFG_REG__8                  \
    *(volatile unsigned int*)(APB_ANALOG_CFG_BASE + \
                     0x200000)  // contains 2.4 GHz oscillator control, see
                                // bucket_o_functions/LC_freqchange
#define ANALOG_CFG_REG__9 *(volatile unsigned int*)(APB_ANALOG_CFG_BASE + 0x240000)
#define ANALOG_CFG_REG__10 *(volatile unsigned int*)(APB_ANALOG_CFG_BASE + 0x280000)
#define ANALOG_CFG_REG__11                 \
    *(volatile unsigned int*)(APB_ANALOG_CFG_BASE + \
                     0x2C0000)  // contains control bits for the arbitrary TX
                                // FIFO, apparently
#define ANALOG_CFG_REG__12 *(volatile unsigned int*)(APB_ANALOG_CFG_BASE + 0x300000)
#define ANALOG_CFG_REG__13 *(volatile unsigned int*)(APB_ANALOG_CFG_BASE + 0x340000)
#define ANALOG_CFG_REG__14 *(volatile unsigned int*)(APB_ANALOG_CFG_BASE + 0x380000)
#define ANALOG_CFG_REG__15 *(volatile unsigned int*)(APB_ANALOG_CFG_BASE + 0x3C0000)
#define ANALOG_CFG_REG__16 *(volatile unsigned int*)(APB_ANALOG_CFG_BASE + 0x400000)
#define ANALOG_CFG_REG__17 *(volatile unsigned int*)(APB_ANALOG_CFG_BASE + 0x440000)
#define ANALOG_CFG_REG__18 *(volatile unsigned int*)(APB_ANALOG_CFG_BASE + 0x480000)
#define ANALOG_CFG_REG__19 *(volatile unsigned int*)(APB_ANALOG_CFG_BASE + 0x4C0000)
#define ANALOG_CFG_REG__20 *(volatile unsigned int*)(APB_ANALOG_CFG_BASE + 0x500000)
#define ANALOG_CFG_REG__21 *(volatile unsigned int*)(APB_ANALOG_CFG_BASE + 0x540000)
#define ANALOG_CFG_REG__22 *(volatile unsigned int*)(APB_ANALOG_CFG_BASE + 0x580000)
#define ANALOG_CFG_REG__23 *(volatile unsigned int*)(APB_ANALOG_CFG_BASE + 0x5C0000)
#define ANALOG_CFG_REG__24 *(volatile unsigned int*)(APB_ANALOG_CFG_BASE + 0x600000)
#define ANALOG_CFG_REG__25 *(volatile unsigned int*)(APB_ANALOG_CFG_BASE + 0x640000)
#define ANALOG_CFG_REG__26 *(volatile unsigned int*)(APB_ANALOG_CFG_BASE + 0x680000)
#define ANALOG_CFG_REG__27 *(volatile unsigned int*)(APB_ANALOG_CFG_BASE + 0x6C0000)
#define ANALOG_CFG_REG__28 *(volatile unsigned int*)(APB_ANALOG_CFG_BASE + 0x700000)
#define ANALOG_CFG_REG__29 *(volatile unsigned int*)(APB_ANALOG_CFG_BASE + 0x740000)
#define ANALOG_CFG_REG__30 *(volatile unsigned int*)(APB_ANALOG_CFG_BASE + 0x780000)

#define ACFG_LO__ADDR *(volatile unsigned int*)(APB_ANALOG_CFG_BASE + 0x1C0000)
#define ACFG_LO__ADDR_2 *(volatile unsigned int*)(APB_ANALOG_CFG_BASE + 0x200000)

// Interrupt clear/set enable register
#define ISER *(volatile unsigned int*)(0xE000E100)
#define ICER *(volatile unsigned int*)(0xE000E180)
// Interrupt clear/set pending register
#define ICPR *(volatile unsigned int*)(0xE000E280)
#define ISPR *(volatile unsigned int*)(0xE000E200)

// =========================== Priority Registers =============================

#define IPR0 *(volatile unsigned int*)(0xE000E400)
#define IPR6 *(volatile unsigned int*)(0xE000E418)
#define IPR7 *(volatile unsigned int*)(0xE000E41C)
