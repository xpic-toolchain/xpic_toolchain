#ifndef __COMMON_FUNC_APPL_NETX10_H
#define __COMMON_FUNC_APPL_NETX10_H

#include "defines.h" // for PEEK, POKE
#include "regdef_netx10.h"

/*****************************************************************************/
/* Definitions                                                               */
/*****************************************************************************/
#define NX10_DEV_FREQUENCY 100000000L // 100 Mhz


#ifdef __ARM_COMPILER__

  #define INTERRUPT __irq
  #define PACKED __packed

#else

  #define INTERRUPT __attribute__ ((interrupt ("IRQ")))
  #define PACKED __attribute__ ((packed))

#endif



/*
  _   _      _      ____    _____ 
 | | | |    / \    |  _ \  |_   _|
 | | | |   / _ \   | |_) |   | |  
 | |_| |  / ___ \  |  _ <    | |  
  \___/  /_/   \_\ |_| \_\   |_|  

*/

/*****************************************************************************/
/* Definitions                                                               */
/*****************************************************************************/
typedef unsigned long UART_BAUDRATE;
#define NX10_UART_BAUDRATE_300        3 
#define NX10_UART_BAUDRATE_600        6 
#define NX10_UART_BAUDRATE_1200      12 
#define NX10_UART_BAUDRATE_2400      24 
#define NX10_UART_BAUDRATE_4800      48 
#define NX10_UART_BAUDRATE_9600      96 
#define NX10_UART_BAUDRATE_19200    192 
#define NX10_UART_BAUDRATE_38400    384 
#define NX10_UART_BAUDRATE_57600    576 
#define NX10_UART_BAUDRATE_115200  1152 


#define NX10_DEV_BAUDRATE NX10_UART_BAUDRATE_115200

// DEV_BAUDRATE is 100 times to small -> multiply with 100 (or divide by DEV_FREQUENCY/100)
#define NX10_DEV_BAUDRATE_DIV_LO (((NX10_DEV_BAUDRATE*16*65536)/(NX10_DEV_FREQUENCY/100))&0xff)
#define NX10_DEV_BAUDRATE_DIV_HI (((NX10_DEV_BAUDRATE*16*65536)/(NX10_DEV_FREQUENCY/100))>>8) 

/*****************************************************************************/
/* Function prototypes                                                       */
/*****************************************************************************/
void NX10_UART_InitSerial(unsigned int ulUartNr);
void NX10_UART_CloseSerial(unsigned int ulUartNr);
void NX10_UART_PutCharacter(unsigned int ulUartNr, const unsigned char bChar);
int  NX10_UART_GetCharacter(unsigned int ulUartNr);
int  NX10_UART_Peek(unsigned int ulUartNr);
void NX10_UART_UPrintf(const char* pchFmt, ...);
void NX10_UART_UPrintfDec(const char* pchFmt, ...);

/*
 __     __  ___    ____ 
 \ \   / / |_ _|  / ___|
  \ \ / /   | |  | |    
   \ V /    | |  | |___ 
    \_/    |___|  \____|
                        
*/

/*****************************************************************************/
/* Definitions                                                               */
/*****************************************************************************/
typedef void (*NX10_pp_func)(void);

#define MAX_IRQ_VECTORS             16
#define ARM9_Vect_Undefined	0x00000004    
#define ARM9_Vect_SWI		    0x00000008    
#define ARM9_Vect_Prefetch	0x0000000c    
#define ARM9_Vect_Abort		  0x00000010    
#define ARM9_Vect_IRQ		    0x00000018
#define ARM9_Vect_FIQ		    0x0000001c    


#ifdef __ARM_COMPILER__

/* Interrupt attribute for ARM compiler */
#define NX10_IRQFIQ_LOCK     __asm { MSR CPSR_c, #0xdf }
#define NX10_IRQFIQ_UNLOCK   __asm { MSR CPSR_c, #0x1f }

#define NX10_lock_irqfiq_save(x) (x = NX10_LockIrqSave())
#define NX10_lock_irqfiq_restore(x)  (NX10_LockIrqRestore(x))

static unsigned int NX10_LockIrqSave()
{                                                
    register unsigned int cpsr_tmp;         
    register unsigned int bla;		  
    __asm ( "MRS cpsr_tmp, CPSR" );           
    __asm ( "AND bla, cpsr_tmp, #0xC0");      
    __asm ( "ORR cpsr_tmp, cpsr_tmp, #0xC0"); 
    __asm ( "MSR CPSR_c, cpsr_tmp");          
    
    return bla;
}

static void NX10_LockIrqRestore(unsigned int x)
{                                                  
    register unsigned int cpsr_tmp;            
    __asm ( "MRS cpsr_tmp, CPSR");             
    __asm ( "EOR cpsr_tmp, cpsr_tmp, #0xC0");  
    __asm ( "ORR cpsr_tmp, cpsr_tmp, (x)");      
    __asm ( "MSR CPSR_c, cpsr_tmp");           
}

#else

/* Interrupt attribute for GNU compiler */
#define NX10_IRQFIQ_LOCK     asm ( "msr cpsr_c, #0xdf" );
#define NX10_IRQFIQ_UNLOCK   asm ( "msr cpsr_c, #0x1f" );


#define NX10_lock_irqfiq_save(x)                             \
        ({                                              \
                register unsigned int cpsr_tmp;         \
                __asm__ __volatile__(                   \
                        "MRS %1, cpsr\n"                \
                        "AND %0, %1, #0xC0\n"           \
                        "ORR %1, %1, #0xC0\n"           \
                        "MSR CPSR_c, %1"                \
                        : "=r" (x), "=&r" (cpsr_tmp)    \
                );                                      \
        })

#define NX10_lock_irqfiq_restore(x)                          \
        ({                                              \
                register unsigned int cpsr_tmp;         \
                __asm__ __volatile__(                   \
                        "MRS %0, cpsr\n"                \
                        "EOR %0, %0, #0xC0\n"           \
                        "ORR %0, %0, %1\n"              \
                        "MSR CPSR_c, %0"                \
                        : "=&r" (cpsr_tmp)              \
                        : "r" (x)                       \
                );                                      \
        })



#endif



/*****************************************************************************/
/* Function prototypes                                                       */
/*****************************************************************************/
void NX10_VIC_InstallIsrVector( NX10_pp_func ulVector);
void NX10_VIC_InstallFiqIsrVector( NX10_pp_func ulVector );
void NX10_VIC_ClearVic(void);
void INTERRUPT NX10_VIC_IRQVectorHandler_C(void);
void NX10_VIC_ISR_Default(void);
void NX10_VIC_SetDefaultVector(NX10_pp_func default_handler);
void NX10_VIC_SetIrqVector(unsigned int vec, unsigned long src, NX10_pp_func irq_handler);
void NX10_VIC_EnableIrqSources(unsigned long src);
void NX10_VIC_DisableIrqSources( unsigned long src );
unsigned long NX10_VIC_GetIrqRaw( void );
unsigned long NX10_VIC_GetIrqStatus( void );
unsigned long NX10_VIC_GetFiqStatus( void );
void NX10_VIC_SetSoftInterrupt( unsigned long src );
void NX10_VIC_ClrSoftInterrupt( unsigned long src );
void NX10_VIC_SetProtection( unsigned long val );

/*
   ____   ____    ___    ___  
  / ___| |  _ \  |_ _|  / _ \ 
 | |  _  | |_) |  | |  | | | |
 | |_| | |  __/   | |  | |_| |
  \____| |_|     |___|  \___/ 
                              
*/

/*****************************************************************************/
/* Definitions                                                               */
/*****************************************************************************/
#define NX10_GPIO_MODE_NOINVERT                   0x0
#define NX10_GPIO_MODE_INVERT                    0x10
#define NX10_GPIO_MODE_INPUT_READ                 0x0
#define NX10_GPIO_MODE_OUTPUT_SET_TO_0            0x4
#define NX10_GPIO_MODE_OUTPUT_SET_TO_1            0x5
#define NX10_GPIO_MODE_OUTPUT_SET_TO_GPIO_LINE    0x6

/*****************************************************************************/
/* Function prototypes                                                       */
/*****************************************************************************/
int   NX10_GPIO_GetIrq (unsigned long ulGpioNum);
int   NX10_GPIO_GetInput (unsigned long ulGpioNum);
unsigned long NX10_GPIO_GetIn( void );
void  NX10_GPIO_SetupMode (unsigned long ulGpioNum, unsigned long ulMode, unsigned long ulInvert);
void  NX10_GPIO_SetOutput (unsigned long ulGpioNum, int fEnable);
unsigned long NX10_GPIO_GetLine( void );
void  NX10_GPIO_SetLine( unsigned long ulVal );
void  NX10_GPIO_IrqEnable (unsigned long ulGPIOnum);
void  NX10_GPIO_IrqReset (unsigned long ulGpioNum);
void  NX10_GPIO_Sleep (unsigned long ulCounter, unsigned int uiTimeout);
void NX10_GPIO_SetupTimer( unsigned long ulCounter, unsigned int uiTimeout );
int NX10_GPIO_IsTimerRunning( unsigned long ulCounter );


/*
  __  __   __  __   ___    ___  
 |  \/  | |  \/  | |_ _|  / _ \ 
 | |\/| | | |\/| |  | |  | | | |
 | |  | | | |  | |  | |  | |_| |
 |_|  |_| |_|  |_| |___|  \___/ 
                                
*/

/*****************************************************************************/
/* Definitions                                                               */
/*****************************************************************************/
#define NX10_MMIO_CONFIG_XM0_IO0            0x00
#define NX10_MMIO_CONFIG_XM0_IO1            0x01
#define NX10_MMIO_CONFIG_XM0_IO2            0x02
#define NX10_MMIO_CONFIG_XM0_IO3            0x03
#define NX10_MMIO_CONFIG_XM0_IO4            0x04
#define NX10_MMIO_CONFIG_XM0_IO5            0x05
#define NX10_MMIO_CONFIG_XM0_RX             0x06
#define NX10_MMIO_CONFIG_GPIO0              0x07
#define NX10_MMIO_CONFIG_GPIO1              0x08
#define NX10_MMIO_CONFIG_GPIO2              0x09
#define NX10_MMIO_CONFIG_GPIO3              0x0A
#define NX10_MMIO_CONFIG_GPIO4              0x0B
#define NX10_MMIO_CONFIG_GPIO5              0x0C
#define NX10_MMIO_CONFIG_GPIO6              0x0D
#define NX10_MMIO_CONFIG_GPIO7              0x0E
#define NX10_MMIO_CONFIG_PHY0_LED0          0x0F
#define NX10_MMIO_CONFIG_PHY0_LED1          0x10
#define NX10_MMIO_CONFIG_PHY0_LED2          0x11
#define NX10_MMIO_CONFIG_PHY0_LED3          0x12
#define NX10_MMIO_CONFIG_SPI0_CS1N          0x13
#define NX10_MMIO_CONFIG_SPI0_CS2N          0x14
#define NX10_MMIO_CONFIG_SPI1_CLK           0x15
#define NX10_MMIO_CONFIG_SPI1_CS0N          0x16
#define NX10_MMIO_CONFIG_SPI1_CS1N          0x17
#define NX10_MMIO_CONFIG_SPI1_CS2N          0x18
#define NX10_MMIO_CONFIG_SPI1_MISO          0x19
#define NX10_MMIO_CONFIG_SPI1_MOSI          0x1A
#define NX10_MMIO_CONFIG_I2C_SCL_MMIO       0x1B
#define NX10_MMIO_CONFIG_I2C_SDA_MMIO       0x1C
#define NX10_MMIO_CONFIG_UART0_CTS          0x1D
#define NX10_MMIO_CONFIG_UART0_RTS          0x1E
#define NX10_MMIO_CONFIG_UART0_RXD          0x1F
#define NX10_MMIO_CONFIG_UART0_TXD          0x20
#define NX10_MMIO_CONFIG_UART1_CTS          0x21
#define NX10_MMIO_CONFIG_UART1_RTS          0x22
#define NX10_MMIO_CONFIG_UART1_RXD          0x23
#define NX10_MMIO_CONFIG_UART1_TXD          0x24
#define NX10_MMIO_CONFIG_PWM_FAILURE_N      0x25
#define NX10_MMIO_CONFIG_POS_ENC0_A         0x26
#define NX10_MMIO_CONFIG_POS_ENC0_B         0x27
#define NX10_MMIO_CONFIG_POS_ENC0_N         0x28
#define NX10_MMIO_CONFIG_POS_ENC1_A         0x29
#define NX10_MMIO_CONFIG_POS_ENC1_B         0x2A
#define NX10_MMIO_CONFIG_POS_ENC1_N         0x2B
#define NX10_MMIO_CONFIG_POS_MP0            0x2C
#define NX10_MMIO_CONFIG_POS_MP1            0x2D
#define NX10_MMIO_CONFIG_IO_LINK0_IN        0x2E
#define NX10_MMIO_CONFIG_IO_LINK0_OUT       0x2F
#define NX10_MMIO_CONFIG_IO_LINK0_OE        0x30
#define NX10_MMIO_CONFIG_IO_LINK1_IN        0x31
#define NX10_MMIO_CONFIG_IO_LINK1_OUT       0x32
#define NX10_MMIO_CONFIG_IO_LINK1_OE        0x33
#define NX10_MMIO_CONFIG_IO_LINK2_IN        0x34
#define NX10_MMIO_CONFIG_IO_LINK2_OUT       0x35
#define NX10_MMIO_CONFIG_IO_LINK2_OE        0x36
#define NX10_MMIO_CONFIG_IO_LINK3_IN        0x37
#define NX10_MMIO_CONFIG_IO_LINK3_OUT       0x38
#define NX10_MMIO_CONFIG_IO_LINK3_OE        0x39
#define NX10_MMIO_CONFIG_PIO_MODE           0x3F

#define NX10_MMIO_MODE_IN  0
#define NX10_MMIO_MODE_OUT 1

/*****************************************************************************/
/* Function prototypes                                                       */
/*****************************************************************************/
void NX10_MMIO_SetCfg( unsigned int uMmioNum, unsigned char bFunction, int fInvertOut, int fInvertIn );
void NX10_MMIO_GetCfg( unsigned int uMmioNum, unsigned char* pbFunction, int* pfInvertOut, int* pfInvertIn );
void NX10_MMIO_SetMode( unsigned long ulMmioNum, unsigned long ulMode );
int NX10_MMIO_GetInput( unsigned long ulMmioNum );
unsigned long NX10_MMIO_GetIn( void );
int NX10_MMIO_GetOutput( unsigned long ulMmioNum );
void NX10_MMIO_SetOutput( unsigned long ulMmioNum, int fEnable );
unsigned long NX10_MMIO_GetLine( void );
void NX10_MMIO_SetLine( unsigned long ulVal );

/*
  ____    ____    __  __ 
 |  _ \  |  _ \  |  \/  |
 | | | | | |_) | | |\/| |
 | |_| | |  __/  | |  | |
 |____/  |_|     |_|  |_|
                         
*/
#define NX10_NUM_DPM_WINDOWS     4 /**< Number of DPM windows */

// DPM parameters (Hilscher standard layout as host sight)
  #define NX10_NETX_DPM_START_ADDR      Addr_NX10_intram5_dpm_mirror
  #define NX10_OFFSET_CH_SYSTEM         0
  #define NX10_SIZE_CH_SYSTEM           0x0200
  #define NX10_OFFSET_CH_HS             NX10_OFFSET_CH_SYSTEM + NX10_SIZE_CH_SYSTEM
  #define NX10_SIZE_CH_HS               0x0100
  #define NX10_OFFSET_CH_COM0           NX10_OFFSET_CH_HS + NX10_SIZE_CH_HS
  #define NX10_SIZE_CH_COM0             0x1000
  #define NX10_OFFSET_AREA_OUT0         NX10_OFFSET_CH_COM0 + NX10_SIZE_CH_COM0
  #define NX10_SIZE_AREA_OUT0           0x1680
  #define NX10_OFFSET_AREA_IN0          NX10_OFFSET_AREA_OUT0 + NX10_SIZE_AREA_OUT0
  #define NX10_SIZE_AREA_IN0            0x1680

// sizes for DPM areas as netX sight within INTRAM 5
  #define NX10_SIZE_CH_SYSTEM_NETX      0x0200
  #define NX10_SIZE_CH_HS_NETX          0x0100
  #define NX10_SIZE_CH_COM0_NETX        0x1000
  #define NX10_SIZE_AREA_OUT0_NETX      0x0600
  #define NX10_SIZE_AREA_IN0_NETX       0x0600

// DPM structure (Hilscher standard layout)
typedef struct NX10_DPM_STD_LAYOUT_Ttag
{
  volatile unsigned char abSystemCh[NX10_SIZE_CH_SYSTEM_NETX];
  volatile unsigned char abHandshakeCh[NX10_SIZE_CH_HS_NETX];
  volatile unsigned char abCom0Ch[NX10_SIZE_CH_COM0_NETX];
  volatile unsigned char abOutDataArea0[NX10_SIZE_AREA_OUT0_NETX];
  volatile unsigned char abInDataArea0[NX10_SIZE_AREA_IN0_NETX];
} NX10_DPM_STD_LAYOUT_T;

/*****************************************************************************/
/*! Configuration DPM access mode                                            */
/*****************************************************************************/
typedef unsigned long NX10_DPM_MODE;
#define NX10_DPM_MODE_8BIT_SRAM                0 /**< 8 bit SRAM mode      */
#define NX10_DPM_MODE_16BIT_SRAM               4 /**< 16 bit SRAM mode     */
#define NX10_DPM_MODE_16BIT_TIOMAP              6 /**< 16 bit TI OMAP mode  */


/*****************************************************************************/
/*! Configuration DPM endianess                                              */
/*****************************************************************************/
typedef unsigned long NX10_DPM_ENDIANESS;
#define NX10_DPM_ENDIANESS_LITTLE              0 /**< Little endian      */
#define NX10_DPM_ENDIANESS_16BIT_BIG           1 /**< 16 bit big endian  */
#define NX10_DPM_ENDIANESS_32BIT_BIG           2 /**< 32 bit big endian  */


/*****************************************************************************/
/*! Configuration DPM address range                                          */
/*****************************************************************************/
typedef unsigned long NX10_DPM_ADDRRANGE;
#define NX10_DPM_DPM_ADDRRANGE_RES0              0 /**< reserved     */
#define NX10_DPM_DPM_ADDRRANGE_RES1              1 /**< reserved     */
#define NX10_DPM_DPM_ADDRRANGE_2KB               2 /**< 2 kByte      */
#define NX10_DPM_DPM_ADDRRANGE_RES2              3 /**< reserved     */
#define NX10_DPM_DPM_ADDRRANGE_8KB               4 /**< 8 kByte      */
#define NX10_DPM_DPM_ADDRRANGE_16KB              5 /**< 16 kByte     */
#define NX10_DPM_DPM_ADDRRANGE_32KB              6 /**< 32 kByte     */
#define NX10_DPM_DPM_ADDRRANGE_64KB              7 /**< 64 kByte     */
#define NX10_DPM_DPM_ADDRRANGE_128KB             8 /**< 128 kByte    */


/*****************************************************************************/
/*! Configuration DPM external address range configuration                   */
/*****************************************************************************/
typedef unsigned long NX10_DPM_CFGWINADDRCFG;
#define NX10_DPM_CFGWINADDRCFG_FIRST             0 /**< Configuration window located at first 256 bytes of external DPM address range */
#define NX10_DPM_CFGWINADDRCFG_LAST              1 /**< Configuration window located at last 256 bytes of external DPM address range  */
#define NX10_DPM_CFGWINADDRCFG_RES               2 /**< reserved     */
#define NX10_DPM_CFGWINADDRCFG_DIS               3 /**< Configuration window disabled      */


/*****************************************************************************/
/*! Configuration DPM RDY signal mode                                        */
/*****************************************************************************/
typedef unsigned long NX10_DPM_RDYMODE;
#define NX10_DPM_RDYMODE_WAITBUSY                0 /**< Wait/Busy */
#define NX10_DPM_RDYMODE_RDYACK                  1 /**< Rdy/Ack   */


/*****************************************************************************/
/*! Configuration DPM DRV mode                                               */
/*****************************************************************************/
typedef unsigned long NX10_DPM_RDYDRV;
#define NX10_DPM_RDYMODE_DIS                     0 /**< Disabled */
#define NX10_DPM_RDYMODE_PUSH_PULL               1 /**< PushPull */
#define NX10_DPM_RDYMODE_SUSTAIN_TRISTATE        2 /**< Sustain Tristate */
#define NX10_DPM_RDYMODE_OPEN_DRAIN              3 /**< open-Drain */


/*****************************************************************************/
/*! Configuration for Window Alternative Address Mapping Configuration       */
/*****************************************************************************/
typedef unsigned long NX10_DPM_WINMAP_ALT;
#define NX10_DPM_WINMAP_ALT_DIS                 0 /**< 00: Alternative mapping disabled                                          */
#define NX10_DPM_WINMAP_ALT_EN_BUFMAN_0         1 /**< 01: Alternative mapping enabled: Use tripple buffer manager 0 from HANDSHAKE_CTRL unit. */
#define NX10_DPM_WINMAP_ALT_EN_BUFMAN_1         2 /**< 01: Alternative mapping enabled: Use tripple buffer manager 1 from HANDSHAKE_CTRL unit. */
#define NX10_DPM_WINMAP_ALT_RES                 3 /**< 11: reserved */


/*****************************************************************************/
/*! Interrupt status types                                                   */
/*****************************************************************************/
typedef unsigned long NX10_DPM_IRQ_STATUS;
#define NX10_DPM_IRQ_STATUS_RAW                 1 /**< dpm_irq_raw                     */
#define NX10_DPM_IRQ_STATUS_ARM_MASKED          2 /**< dpm_irq_arm_masked            . */
#define NX10_DPM_IRQ_STATUS_XPEC_MASKED         3 /**< dpm_irq_xpic_masked           . */
#define NX10_DPM_IRQ_STATUS_FIQ_MASKED          4 /**< dpm_irq_fiq_masked              */
#define NX10_DPM_IRQ_STATUS_IRQ_MASKED          5 /**< dpm_irq_irq_masked              */
#define NX10_DPM_IRQ_STATUS_FW_IRQ_RAW          6 /**< dpm_firmware_irq_raw            */


/*****************************************************************************/
/* Function prototypes                                                       */
/*****************************************************************************/
void NX10_DPM_CfgDpm( NX10_DPM_MODE tMode, NX10_DPM_ENDIANESS tEndianess, NX10_DPM_ADDRRANGE tAddrRange, NX10_DPM_CFGWINADDRCFG tCfgWinAddrCfg );
int  NX10_DPM_CfgTiming( unsigned int uTosa, int fFilter, unsigned int tRds, int fRdBurstEn );
void NX10_DPM_CfgRdy( int fRdypol, NX10_DPM_RDYDRV tRdyDrv, NX10_DPM_RDYMODE tRdyMode );
void NX10_DPM_CfgMisc( unsigned long ulMiscCfg, unsigned long ulIoMiscCfg );
int  NX10_DPM_CfgDpmWindow( unsigned int uWinNum, unsigned int uSize, unsigned long ulMapStartAddr, NX10_DPM_WINMAP_ALT tAltMapping );
int  NX10_DPM_SetupByteAccess( int uWinNum, int fByteAccess );
int  NX10_DPM_SetupReadAhead( int uWinNum, int fReadAhead );
void NX10_DPM_SetupIrq( unsigned long ulIrqMsk, unsigned long ulFiqMsk, unsigned long ulArmMsk, unsigned long ulXpicMsk, unsigned long ulFwIrqMask );
void NX10_DPM_GetStatusErr( unsigned long *pulStatErr, unsigned long *pulStatErrAddr );
void NX10_DPM_ClearErr( unsigned long ulClearMask );
int  NX10_DPM_GetIrq( NX10_DPM_IRQ_STATUS tIrqType, unsigned long *pulIrq );
void NX10_DPM_Reset( void );
void NX10_DPM_GetSysSta( unsigned long *pulSysSta, unsigned long *pulSysStaBigEnd16 );
void NX10_DPM_GetNetxVersion( unsigned long *pulNetxVersion, unsigned long *pulNetxVersionBigEnd16 );





//.##.....##....###....##....##.########...######..##.....##....###....##....##.########..........######..########.########..##......
//.##.....##...##.##...###...##.##.....##.##....##.##.....##...##.##...##...##..##...............##....##....##....##.....##.##......
//.##.....##..##...##..####..##.##.....##.##.......##.....##..##...##..##..##...##...............##..........##....##.....##.##......
//.#########.##.....##.##.##.##.##.....##..######..#########.##.....##.#####....######...........##..........##....########..##......
//.##.....##.#########.##..####.##.....##.......##.##.....##.#########.##..##...##...............##..........##....##...##...##......
//.##.....##.##.....##.##...###.##.....##.##....##.##.....##.##.....##.##...##..##...............##....##....##....##....##..##......
//.##.....##.##.....##.##....##.########...######..##.....##.##.....##.##....##.########.#######..######.....##....##.....##.########

#define NX10_NUM_HS_CELL    16 /**< Number of Handshake cells */
#define NX10_NUM_HS_BUFMAN   2 /**< Number of Handshake Buffer Managers */

// netX communication flags 16-bit mode
#define SRT_NX10_NCF_PD0_OUT_16BIT             6
#define SRT_NX10_NCF_PD0_IN_16BIT              7
#define MSK_NX10_NCF_PD0_OUT_16BIT             1<<SRT_NX10_NCF_PD0_OUT_16BIT
#define MSK_NX10_NCF_PD0_IN_16BIT              1<<SRT_NX10_NCF_PD0_IN_16BIT
// host communication flags 16-bit mode
#define SRT_NX10_HCF_PD0_OUT_16BIT            22
#define SRT_NX10_HCF_PD0_IN_16BIT             23
#define MSK_NX10_HCF_PD0_OUT_16BIT             1<<SRT_NX10_HCF_PD0_OUT_16BIT
#define MSK_NX10_HCF_PD0_IN_16BIT              1<<SRT_NX10_HCF_PD0_IN_16BIT
// netX communication flags 8-bit mode
#define SRT_NX10_NCF_PD0_OUT_8BIT             22
#define SRT_NX10_NCF_PD0_IN_8BIT              23
#define MSK_NX10_NCF_PD0_OUT_8BIT              1<<SRT_NX10_NCF_PD0_OUT_8BIT
#define MSK_NX10_NCF_PD0_IN_8BIT               1<<SRT_NX10_NCF_PD0_IN_8BIT
// host communication flags 8-bit mode
#define SRT_NX10_HCF_PD0_OUT_8BIT             30
#define SRT_NX10_HCF_PD0_IN_8BIT              31
#define MSK_NX10_HCF_PD0_OUT_8BIT              1<<SRT_NX10_HCF_PD0_OUT_8BIT
#define MSK_NX10_HCF_PD0_IN_8BIT               1<<SRT_NX10_HCF_PD0_IN_8BIT

/*****************************************************************************/
/*! Configuration Handshake width                                            */
/*****************************************************************************/
typedef unsigned long NX10_HSCTRL_HSCELL_WIDTH;
#define NX10_HSCTRL_HSCELL_WIDTH_8BIT              0 /**< 8 bit   */
#define NX10_HSCTRL_HSCELL_WIDTH_16BIT              1 /**< 16 bit  */


/*****************************************************************************/
/*! Interrupt sources                                                        */
/*****************************************************************************/
typedef unsigned long NX10_HSCTRL_IRQ_SRC;
#define NX10_HSCTRL_IRQ_SRC_DPM                     1 /**< Interrupt source DPM   */
#define NX10_HSCTRL_IRQ_SRC_ARM                     2 /**< Interrupt source ARM   */
#define NX10_HSCTRL_IRQ_SRC_XPIC                    3 /**< Interrupt source xPIC  */


/*****************************************************************************/
/*! Handshake cell mode                                                      */
/*****************************************************************************/
typedef unsigned long NX10_HSCTRL_CELL_MODE;
#define NX10_HSCTRL_CELL_MODE_DIS           0 /**< Cell disabled                    */
#define NX10_HSCTRL_CELL_MODE_DPM_ARM       1 /**< Cell for handshaking DPM<->ARM . */
#define NX10_HSCTRL_CELL_MODE_DPM_XPIC      2 /**< Cell for handshaking DPM<->XPIC. */
#define NX10_HSCTRL_CELL_MODE_ARM_XPIC      3 /**< Cell for handshaking ARM<->XPIC. */


/*****************************************************************************/
/*! Handshake BufMan Address Mapping Configuration                           */
/*****************************************************************************/
typedef unsigned long NX10_HSCTRL_BUFMAN_ADDRMAPCFG;
#define NX10_HSCTRL_BUFMAN_ADDRMAPCFG_DPMWIN       0 /**< Use mapping from DPM window config    */
#define NX10_HSCTRL_BUFMAN_ADDRMAPCFG_ALTWIN1      1 /**< Use alternate mapping 1               */
#define NX10_HSCTRL_BUFMAN_ADDRMAPCFG_ALTWIN2      2 /**< Use alternate mapping 2               */
#define NX10_HSCTRL_BUFMAN_ADDRMAPCFG_CURBUF       3 /**< Mapping controlled by BufMan          */


/*****************************************************************************/
/*! Handshake buffer manager command master                                  */
/*****************************************************************************/
typedef unsigned long NX10_HSCTRL_BUFMAN_CMDM;
#define NX10_HSCTRL_BUFMAN_CMDM_NETX         0 /**< netX    */
#define NX10_HSCTRL_BUFMAN_CMDM_HOST         1 /**< host    */


/*****************************************************************************/
/*! Handshake buffer manager command                                         */
/*****************************************************************************/
typedef unsigned long NX10_HSCTRL_BUFMAN_CMD;
#define NX10_HSCTRL_BUFMAN_CMD_NOP           0 /**< Nop/Idle               */
#define NX10_HSCTRL_BUFMAN_CMD_REQ_READ_BUF  1 /**< Request read buffer    */
#define NX10_HSCTRL_BUFMAN_CMD_REQ_WRITE_BUF 2 /**< Request write buffer   */
#define NX10_HSCTRL_BUFMAN_CMD_RELEASE_CUR_BUF 3 /**< Release current buffer */


/*****************************************************************************/
/*! Handshake buffer manager status                                          */
/*****************************************************************************/
typedef unsigned long NX10_HSCTRL_BUFMAN_STAT;
#define NX10_HSCTRL_BUFMAN_STAT_BUF0_VALID   0 /**< Buffer 0 valid     */
#define NX10_HSCTRL_BUFMAN_STAT_BUF1_VALID   1 /**< Buffer 1 valid     */
#define NX10_HSCTRL_BUFMAN_STAT_BUF2_VALID   2 /**< Buffer 2 valid     */
#define NX10_HSCTRL_BUFMAN_STAT_NO_BUF_VALID 3 /**< No buffer valid  . */


/*****************************************************************************/
/* Function prototypes                                                       */
/*****************************************************************************/
int  NX10_HSCTRL_CfgHsBaseAddr( unsigned long ulHsBaseAddr );
int  NX10_HSCTRL_EnIrq( NX10_HSCTRL_IRQ_SRC tSrc, unsigned long ulIrqEn );
int  NX10_HSCTRL_DisIrq( NX10_HSCTRL_IRQ_SRC tSrc, unsigned long ulIrqDis );
int  NX10_HSCTRL_GetIrq( NX10_HSCTRL_IRQ_SRC tSrc, unsigned long *pulIrq );
int  NX10_HSCTRL_GetIrqRaw( NX10_HSCTRL_IRQ_SRC tSrc, unsigned long *pulIrq );
int  NX10_HSCTRL_ConfirmIrq( NX10_HSCTRL_IRQ_SRC tSrc, unsigned long ulConfirmIrqMask );
int  NX10_HSCTRL_CfgHsCell( unsigned int uInst, NX10_HSCTRL_CELL_MODE tHsCellmode, NX10_HSCTRL_HSCELL_WIDTH tHsCellWidth );
void NX10_HSCTRL_CfgBufmanPdOut( NX10_HSCTRL_BUFMAN_ADDRMAPCFG tAddrmapCfg, int fAutoMode, unsigned long ulWinMapBuf1, unsigned long ulWinMapBuf2 );
void NX10_HSCTRL_CfgBufmanPdIn( NX10_HSCTRL_BUFMAN_ADDRMAPCFG tAddrmapCfg, int fAutoMode, unsigned long ulWinMapBuf1, unsigned long ulWinMapBuf2 );
int  NX10_HSCTRL_ResetBufMan( unsigned int uInst );
int  NX10_HSCTRL_SetBufManCmd( unsigned int uInst, NX10_HSCTRL_BUFMAN_CMDM tCmdMaster, NX10_HSCTRL_BUFMAN_CMD tCmd );
int  NX10_HSCTRL_GetBufManStat( unsigned int uInst, NX10_HSCTRL_BUFMAN_STAT* ptBufManStatNetx, NX10_HSCTRL_BUFMAN_STAT* ptBufManStatHost );
int  NX10_HSCTRL_ReadHsCell( unsigned int uInst, unsigned long* pulHandshakeVal );
int  NX10_HSCTRL_WriteHsCell( unsigned int uInst, unsigned long ulHandshakeVal );
int  NX10_HSCTRL_ToggleHsCellFlag( unsigned int uInst, unsigned long ulHsCellBitNum );
int  NX10_HSCTRL_AreHsFlagsEqual( unsigned int uInst, unsigned long ulHsFlagNum1, unsigned long ulHsFlagNum2 );

/*
  ____        _           ____                  
 |  _ \    __| |  _   _  |  _ \   _   _   _ __  
 | |_) |  / _` | | | | | | |_) | | | | | | '_ \ 
 |  _ <  | (_| | | |_| | |  _ <  | |_| | | | | |
 |_| \_\  \__,_|  \__, | |_| \_\  \__,_| |_| |_|
                  |___/                         
*/


/*****************************************************************************/
/* Definitions                                                               */
/*****************************************************************************/
typedef enum {
  NX10_RDYRUN_LED_OFF = 0,
  NX10_RDYRUN_LED_GREEN,
  NX10_RDYRUN_LED_RED,
  NX10_RDYRUN_LED_INV
} NX10_RDYRUN_LED_E;

/*****************************************************************************/
/* Function prototypes                                                       */
/*****************************************************************************/
void NX10_RDYRUN_SetRdyRunLed( NX10_RDYRUN_LED_E tMode);

/*
  ___           _                                   _           ____    _   _  __   __
 |_ _|  _ __   | |_    ___   _ __   _ __     __ _  | |         |  _ \  | | | | \ \ / /
  | |  | '_ \  | __|  / _ \ | '__| | '_ \   / _` | | |         | |_) | | |_| |  \ V / 
  | |  | | | | | |_  |  __/ | |    | | | | | (_| | | |         |  __/  |  _  |   | |  
 |___| |_| |_|  \__|  \___| |_|    |_| |_|  \__,_| |_|  _____  |_|     |_| |_|   |_|  
                                                       |_____|                        
*/

/*****************************************************************************/
/* Definitions                                                               */
/*****************************************************************************/
#define NX10_INT_PHY1_ADDR                          0x00
#define NX10_PHY_CTRL_PHY_ADDR                       0x0 // bits 4..1: PHY address, bit 0 indicate internal PHY 0 or 1
#define NX10_PHY_CTRL_PHY_FX_MODE                      0
#define NX10_PHY_CTRL_PHY_AUTOMDIX                     1
#define NX10_PHY_CTRL_PHY_NP_MSG_CODE                  0
#define NX10_PHY_CTRL_PHY_ENABLE                       1
#define NX10_RESET_PHY_DELAY                    80000000 // reset delay in 100us units


// PHY Control Register
#define NX10_PHYCTRL_10BASE_T_HD_NOAUTONEG                       0x00
#define NX10_PHYCTRL_10BASE_T_FD_NOAUTONEG                       0x01
#define NX10_PHYCTRL_100BASE_TXFX_HD_NOAUTONEG_CRSTXRX           0x02
#define NX10_PHYCTRL_100BASE_TXFX_FD_NOAUTONEG_CRSRX             0x03
#define NX10_PHYCTRL_100BASE_TX_HD_ADV_AUTONEG_CRSRXTX           0x04
#define NX10_PHYCTRL_REPEATER_AUTONEG_100BASE_TX_HD_ADV_CRSRX    0x05
#define NX10_PHYCTRL_POWER_DOWN_MODE                             0x06
#define NX10_PHYCTRL_ALL_CAPABLE_AUTONEG_AUTOMDIXEN              0x07

/*****************************************************************************/
/* Function prototypes                                                       */
/*****************************************************************************/
void NX10_INTPHY_Init( NX10_PHY_CONTROL_T *ptPhyCtrlInit );
void NX10_INTPHY_CorrectAutoMdix100MBit( unsigned int uMiimuPhyAddr );

/*
  ____            _           _                           _____   _    __         
 |  _ \    ___   (_)  _ __   | |_    ___   _ __          |  ___| (_)  / _|   ___  
 | |_) |  / _ \  | | | '_ \  | __|  / _ \ | '__|         | |_    | | | |_   / _ \ 
 |  __/  | (_) | | | | | | | | |_  |  __/ | |            |  _|   | | |  _| | (_) |
 |_|      \___/  |_| |_| |_|  \__|  \___| |_|     _____  |_|     |_| |_|    \___/ 
                                                 |_____|                          
*/

/*****************************************************************************/
/*! ???
*   \param xxx            ???                                                */
/*****************************************************************************/
void NX10_PFIFO_Reset( void );
int NX10_PFIFO_SetBorders(const unsigned int *auPFifoDepth);
unsigned long NX10_PFIFO_GetFillLevel( unsigned int uFifoNum );
unsigned long NX10_PFIFO_GetFifoResetVector( void );
unsigned long NX10_PFIFO_GetFifoFullVector( void );
unsigned long NX10_PFIFO_GetFifoEmptyVector( void );
unsigned long NX10_PFIFO_GetFifoOverflowVector( void );
unsigned long NX10_PFIFO_GetFifoUnderrunVector( void );


/*
.########..##......##.##.....##
.##.....##.##..##..##.###...###
.##.....##.##..##..##.####.####
.########..##..##..##.##.###.##
.##........##..##..##.##.....##
.##........##..##..##.##.....##
.##.........###..###..##.....##
*/

/*****************************************************************************/
/*! Result codes for functions                                               */
/*****************************************************************************/
typedef unsigned long NX10_MOTION_RESULT;
#define NX10_MOTION_OKAY                     1 /**< Successful                                   */
#define NX10_MOTION_ERR_INIT_FAILED          2 /**< Initialization has failed                    */
#define NX10_MOTION_ERR_INVAL_PARAM          3 /**< Invalid parameter                            */
#define NX10_MOTION_ERR_NOT_READY            4 /**< Module not ready                               */



#define NX10_PWM_FAILURE       0 
#define NX10_PWM_NO_FAILURE    1 
#define NX10_PWM_CHANNELS      8 /**< Number of PWM pins */
#define NX10_PWM_NUM_CNT       2 /**< Number of PWM counters */
#define NX10_PWM_IRQ_SRC_NUM   2 /**< Number of Interrupt sources */

/*****************************************************************************/
/*! Configuration for PWM Pin                                                */
/*****************************************************************************/
typedef unsigned long NX10_PWM_PINCFG;
#define NX10_PWM_PINCFG_SET_OUTPUT_1              0 /**< 000: set output to 1                                          */
#define NX10_PWM_PINCFG_SET_OUTPUT_0              1 /**< 001: set output to 0                                          */
#define NX10_PWM_PINCFG_SET_OUTPUT_PWM_SIG_CNT0   2 /**< 010: set output to PWM signal referenced on counter0          */
#define NX10_PWM_PINCFG_SET_OUTPUT_INV_PWM_SIG_CNT0 3 /**< 011: set output to invertet PWM signal referenced on counter0 */
#define NX10_PWM_PINCFG_SET_OUTPUT_HIGHZ           4 /**< 100: set output to High-Z                                     */
#define NX10_PWM_PINCFG_RESERVED                   5 /**< 101: reserved for future use                                  */
#define NX10_PWM_PINCFG_SET_OUTPUT_PWM_SIG_CNT1    6 /**< 110: set output to PWM signal referenced on counter1          */
#define NX10_PWM_PINCFG_SET_OUTPUT_INV_PWM_SIG_CNT1 7 /**< 111: set output to invertet PWM signal referenced on counter1 */


/*****************************************************************************/
/*! Configuration for PWM Pin in case of failure                             */
/*****************************************************************************/
typedef unsigned long NX10_PWM_FAILCFG;
#define NX10_PWM_FAILCFG_SET_OUTPUT_0              0 /**< 00: set output to 0                                          */
#define NX10_PWM_FAILCFG_SET_OUTPUT_1              1 /**< 01: set output to 1                                          */
#define NX10_PWM_FAILCFG_SET_OUTPUT_HIGHZ          2 /**< 10: set output to High-Z                                     */
#define NX10_PWM_FAILCFG_LEAVE_OUTPUT_AS_DEFINED   3 /**< 11: leave output as defined in mpwm_config_pins              */


/*****************************************************************************/
/*! Select PWM pin/condition for interrupt                                   */
/*****************************************************************************/
typedef unsigned long NX10_PWM_IRQCFG;
#define NX10_PWM_IRQCFG_DISABLE_IRQ                0 /**< 00000: disable interrupt                                     */
#define NX10_PWM_IRQCFG_GEN_IRQ_CNT0_EQ_0          1 /**< 00001: generate interrupt at pwm_cnt0 == 0                   */
#define NX10_PWM_IRQCFG_GEN_IRQ_CNT1_EQ_0          2 /**< 00010: generate interrupt at pwm_cnt1 == 0                   */
#define NX10_PWM_IRQCFG_GEN_IRQ_CNT0_EQ_MAX        3 /**< 00011: generate interrupt at pwm_cnt0 == max                 */
#define NX10_PWM_IRQCFG_GEN_IRQ_CNT1_EQ_MAX        4 /**< 00100: generate interrupt at pwm_cnt1 == max                 */
#define NX10_PWM_IRQCFG_RES0                       5 /**< 00101: reserved                                              */
#define NX10_PWM_IRQCFG_RES1                       6 /**< 00110: reserved                                              */
#define NX10_PWM_IRQCFG_RES2                       7 /**< 00111: reserved                                              */
#define NX10_PWM_IRQCFG_RES3                       8 /**< 01000: reserved                                              */
#define NX10_PWM_IRQCFG_RES4                       9 /**< 01001: reserved                                              */
#define NX10_PWM_IRQCFG_RES5                      10 /**< 01010: reserved                                              */
#define NX10_PWM_IRQCFG_RES6                      11 /**< 01011: reserved                                              */
#define NX10_PWM_IRQCFG_RES7                      12 /**< 01100: reserved                                              */
#define NX10_PWM_IRQCFG_RES8                      13 /**< 01101: reserved                                              */
#define NX10_PWM_IRQCFG_RES9                      14 /**< 01110: reserved                                              */
#define NX10_PWM_IRQCFG_RESA                      15 /**< 01111: reserved                                              */
#define NX10_PWM_IRQCFG_GEN_IRQ_POSEDGE_PIN0      16 /**< 10000: generate interrupt at posedge of PWM pin 0            */
#define NX10_PWM_IRQCFG_GEN_IRQ_POSEDGE_PIN1      17 /**< 10001: generate interrupt at posedge of PWM pin 1            */
#define NX10_PWM_IRQCFG_GEN_IRQ_POSEDGE_PIN2      18 /**< 10010: generate interrupt at posedge of PWM pin 2            */
#define NX10_PWM_IRQCFG_GEN_IRQ_POSEDGE_PIN3      19 /**< 10011: generate interrupt at posedge of PWM pin 3            */
#define NX10_PWM_IRQCFG_GEN_IRQ_POSEDGE_PIN4      20 /**< 10100: generate interrupt at posedge of PWM pin 4            */
#define NX10_PWM_IRQCFG_GEN_IRQ_POSEDGE_PIN5      21 /**< 10101: generate interrupt at posedge of PWM pin 5            */
#define NX10_PWM_IRQCFG_GEN_IRQ_POSEDGE_PIN6      22 /**< 10110: generate interrupt at posedge of PWM pin 6            */
#define NX10_PWM_IRQCFG_GEN_IRQ_POSEDGE_PIN7      23 /**< 10111: generate interrupt at posedge of PWM pin 7            */
#define NX10_PWM_IRQCFG_GEN_IRQ_NEGEDGE_PIN0      24 /**< 11000: generate interrupt at negedge of PWM pin 0            */
#define NX10_PWM_IRQCFG_GEN_IRQ_NEGEDGE_PIN1      25 /**< 11001: generate interrupt at negedge of PWM pin 1            */
#define NX10_PWM_IRQCFG_GEN_IRQ_NEGEDGE_PIN2      26 /**< 11010: generate interrupt at negedge of PWM pin 2            */
#define NX10_PWM_IRQCFG_GEN_IRQ_NEGEDGE_PIN3      27 /**< 11011: generate interrupt at negedge of PWM pin 3            */
#define NX10_PWM_IRQCFG_GEN_IRQ_NEGEDGE_PIN4      28 /**< 11100: generate interrupt at negedge of PWM pin 4            */
#define NX10_PWM_IRQCFG_GEN_IRQ_NEGEDGE_PIN5      29 /**< 11101: generate interrupt at negedge of PWM pin 5            */
#define NX10_PWM_IRQCFG_GEN_IRQ_NEGEDGE_PIN6      30 /**< 11110: generate interrupt at negedge of PWM pin 6            */
#define NX10_PWM_IRQCFG_GEN_IRQ_NEGEDGE_PIN7      31 /**< 11111: generate interrupt at negedge of PWM pin 7            */


/***********************************************************************************************/
/*! Parameters ID that can be set/get by Pwm_SetPrm()/Pwm_GetPrm()                             */
/***********************************************************************************************/
typedef unsigned long NX10_PWM_PRM;
#define NX10_PWM_PRM_RESERVED                      1 /**< Reserved                                                   */

  /* Config_Counter */
#define NX10_PWM_PRM_RUN_CNT0                      2 /**< Run counter0 (read-only)                                   */
#define NX10_PWM_PRM_WAVEFORM_CNT0                 3 /**< Waveform of counter0                                       */
#define NX10_PWM_PRM_DUAL_EDGE0                    4 /**< Dual Edge Mode for shadow registers references on counter0 */
#define NX10_PWM_PRM_RUN_CNT1                      5 /**< Run counter1 (read-only)                                   */
#define NX10_PWM_PRM_WAVEFORM_CNT1                 6 /**< Waveform of counter1                                       */
#define NX10_PWM_PRM_DUAL_EDGE1                    7 /**< Dual Edge Mode for shadow registers references on counter1 */

  /* Config_Pins */
#define NX10_PWM_PRM_CFG_PIN0                      8 /**< Configuration for PWM Pin0                                 */
#define NX10_PWM_PRM_SHADOW0                       9 /**< Shadow Mode t0                                             */
#define NX10_PWM_PRM_CFG_PIN1                     10 /**< Configuration for PWM Pin1                                 */
#define NX10_PWM_PRM_SHADOW1                      11 /**< Shadow Mode t1                                             */
#define NX10_PWM_PRM_CFG_PIN2                     12 /**< Configuration for PWM Pin2                                 */
#define NX10_PWM_PRM_SHADOW2                      13 /**< Shadow Mode t2                                             */
#define NX10_PWM_PRM_CFG_PIN3                     14 /**< Configuration for PWM Pin3                                 */
#define NX10_PWM_PRM_SHADOW3                      15 /**< Shadow Mode t3                                             */
#define NX10_PWM_PRM_CFG_PIN4                     16 /**< Configuration for PWM Pin4                                 */
#define NX10_PWM_PRM_SHADOW4                      17 /**< Shadow Mode t4                                             */
#define NX10_PWM_PRM_CFG_PIN5                     18 /**< Configuration for PWM Pin5                                 */
#define NX10_PWM_PRM_SHADOW5                      19 /**< Shadow Mode t5                                             */
#define NX10_PWM_PRM_CFG_PIN6                     20 /**< Configuration for PWM Pin6                                 */
#define NX10_PWM_PRM_SHADOW6                      21 /**< Shadow Mode t6                                             */
#define NX10_PWM_PRM_CFG_PIN7                     22 /**< Configuration for PWM Pin7                                 */
#define NX10_PWM_PRM_SHADOW7                      23 /**< Shadow Mode t7                                             */

  /* Config_Failure */
#define NX10_PWM_PRM_CFG_FAIL0                    24 /**< Configuration for PWM Pin0 in case of failure              */
#define NX10_PWM_PRM_CFG_FAIL1                    25 /**< Configuration for PWM Pin1 in case of failure              */
#define NX10_PWM_PRM_CFG_FAIL2                    26 /**< Configuration for PWM Pin2 in case of failure              */
#define NX10_PWM_PRM_CFG_FAIL3                    27 /**< Configuration for PWM Pin3 in case of failure              */
#define NX10_PWM_PRM_CFG_FAIL4                    28 /**< Configuration for PWM Pin4 in case of failure              */
#define NX10_PWM_PRM_CFG_FAIL5                    29 /**< Configuration for PWM Pin5 in case of failure              */
#define NX10_PWM_PRM_CFG_FAIL6                    30 /**< Configuration for PWM Pin6 in case of failure              */
#define NX10_PWM_PRM_CFG_FAIL7                    31 /**< Configuration for PWM Pin7 in case of failure              */
#define NX10_PWM_PRM_FAIL_INPUT_EN                32 /**< Failure input enable                                       */
#define NX10_PWM_PRM_FAIL_FAILURE                 33 /**< Failure bit can be set by software or by config_failure-enable, reset by software */

  /* Irq_Config */
#define NX10_PWM_PRM_IRQCFG0                      34 /**< select PWM pin/mode for interrupt 0                            */
#define NX10_PWM_PRM_IRQDELAY0                    35 /**< Delay from event to generation of interrupt 0 in steps of 20ns */
#define NX10_PWM_PRM_IRQCFG1                      36 /**< select PWM pin/mode for interrupt 1                            */
#define NX10_PWM_PRM_IRQDELAY1                    37 /**< Delay from event to generation of interrupt 1 in steps of 20ns */

  /* Irq_Msk_Set/Irq_Msk_Reset */
#define NX10_PWM_PRM_IRQMSKSET                    38 /**< interrupt mask enable                                          */
#define NX10_PWM_PRM_IRQMSKRST                    39 /**< interrupt mask disable                                         */

  /* Cnt0/Cnt1 */
#define NX10_PWM_PRM_CNT0PERIOD                   40 /**< PWM period of counter 0: Sawtooth mode: period length in steps of 10ns = cnt0_period[19:2], Triangle mode: period length in steps of 20ns = cnt0_period[19:3] */ 
#define NX10_PWM_PRM_CNT0                         41 /**< actual value of PWM counter 0 (read-only) */ 
#define NX10_PWM_PRM_CNT0SYSTIME                  42 /**< Captured Systime at cnt0==0 (read-only) */ 
#define NX10_PWM_PRM_CNT0WDG                      43 /**< Cnt0 watchdog (read-only) */ 
#define NX10_PWM_PRM_CNT1PERIOD                   44 /**< PWM period of counter 0: Sawtooth mode: period length in steps of 10ns = cnt0_period[19:2], Triangle mode: period length in steps of 20ns = cnt0_period[19:3] */ 
#define NX10_PWM_PRM_CNT1                         45 /**< actual value of PWM counter 1 (read-only) */ 
#define NX10_PWM_PRM_CNT1SYSTIME                  46 /**< Captured Systime at cnt1==0 (read-only) */ 
#define NX10_PWM_PRM_CNT1WDG                      47 /**< Cnt1 watchdog (read-only) */ 

  /* T0..T8 */
#define NX10_PWM_PRM_T0                           48 /**< Width of channel 0 low phase in steps of 2,5ns */ 
#define NX10_PWM_PRM_T1                           49 /**< Width of channel 1 low phase in steps of 2,5ns */ 
#define NX10_PWM_PRM_T2                           50 /**< Width of channel 2 low phase in steps of 2,5ns */ 
#define NX10_PWM_PRM_T3                           51 /**< Width of channel 3 low phase in steps of 2,5ns */ 
#define NX10_PWM_PRM_T4                           52 /**< Width of channel 4 low phase in steps of 2,5ns */ 
#define NX10_PWM_PRM_T5                           53 /**< Width of channel 5 low phase in steps of 2,5ns */ 
#define NX10_PWM_PRM_T6                           54 /**< Width of channel 6 low phase in steps of 2,5ns */ 
#define NX10_PWM_PRM_T7                           55 /**< Width of channel 7 low phase in steps of 2,5ns */ 

  /* T0_Shadow..T8_Shadow */
#define NX10_PWM_PRM_T0_SHADOW                    56 /**< Value will be updated from mpwm_t0 at end of period if mpwm_config_pins-shadow0, This register is writable but can also be changed by hardware */ 
#define NX10_PWM_PRM_T1_SHADOW                    57 /**< Value will be updated from mpwm_t1 at end of period if mpwm_config_pins-shadow1, This register is writable but can also be changed by hardware */ 
#define NX10_PWM_PRM_T2_SHADOW                    58 /**< Value will be updated from mpwm_t2 at end of period if mpwm_config_pins-shadow2, This register is writable but can also be changed by hardware */ 
#define NX10_PWM_PRM_T3_SHADOW                    59 /**< Value will be updated from mpwm_t3 at end of period if mpwm_config_pins-shadow3, This register is writable but can also be changed by hardware */ 
#define NX10_PWM_PRM_T4_SHADOW                    60 /**< Value will be updated from mpwm_t4 at end of period if mpwm_config_pins-shadow4, This register is writable but can also be changed by hardware */ 
#define NX10_PWM_PRM_T5_SHADOW                    61 /**< Value will be updated from mpwm_t5 at end of period if mpwm_config_pins-shadow5, This register is writable but can also be changed by hardware */ 
#define NX10_PWM_PRM_T6_SHADOW                    62 /**< Value will be updated from mpwm_t6 at end of period if mpwm_config_pins-shadow6, This register is writable but can also be changed by hardware */ 
#define NX10_PWM_PRM_T7_SHADOW                    63 /**< Value will be updated from mpwm_t7 at end of period if mpwm_config_pins-shadow7, This register is writable but can also be changed by hardware */ 



/***********************************************************************************************/
/* PWM Service Class: Function Prototypes                                                      */
/***********************************************************************************************/
NX10_MOTION_RESULT NX10_PWM_ConfigCnt( unsigned int uInst, int fWaveForm, int fDualEdge );
NX10_MOTION_RESULT NX10_PWM_ConfigFailure( int fEn );
NX10_MOTION_RESULT NX10_PWM_SetSwFailure( int fEn );
NX10_MOTION_RESULT NX10_PWM_ConfigCh( unsigned int uInst, NX10_PWM_PINCFG ePinCfg, int fShadowEn, NX10_PWM_FAILCFG eFailureCfg );
NX10_MOTION_RESULT NX10_PWM_SetCntPeriod( unsigned int uInst, unsigned long ulPeriod );
NX10_MOTION_RESULT NX10_PWM_RetriggerCnt( unsigned int uInst, unsigned long ulWdgVal );
NX10_MOTION_RESULT NX10_PWM_SetChWidth( unsigned int uInst, unsigned long ulWidth );
NX10_MOTION_RESULT NX10_PWM_CfgIrqSrc( unsigned int uInst, NX10_PWM_IRQCFG eIrqCfg, unsigned int uDelay );
NX10_MOTION_RESULT NX10_PWM_GetCntStat( unsigned int uInst, unsigned long* pulValue, unsigned int* pulSystime );
NX10_MOTION_RESULT NX10_PWM_Start( int fCnt0En, int fCnt1En );
NX10_MOTION_RESULT NX10_PWM_Stop( int fCnt0Dis, int fCnt1Dis );
NX10_MOTION_RESULT NX10_PWM_EnIrq( int fSrc0IrqEn, int fSrc1IrqEn, int fFailureIrqEn );
NX10_MOTION_RESULT NX10_PWM_DisIrq( int fSrc0IrqDis, int fSrc1IrqDis, int fFailureIrqDis );
NX10_MOTION_RESULT NX10_PWM_GetIrq( unsigned long *pulIrq );
NX10_MOTION_RESULT NX10_PWM_GetIrqRaw( unsigned long *pulIrq );
NX10_MOTION_RESULT NX10_PWM_ConfirmIrq( unsigned long ulConfirmIrqMask );
NX10_MOTION_RESULT NX10_PWM_SetPrm( unsigned long ulPrmID, unsigned long ulPrmVal );
NX10_MOTION_RESULT NX10_PWM_GetPrm( unsigned long ulPrmID, unsigned long* pulPrmVal );

/*
.########.##....##..######...#######..########..########.########.
.##.......###...##.##....##.##.....##.##.....##.##.......##.....##
.##.......####..##.##.......##.....##.##.....##.##.......##.....##
.######...##.##.##.##.......##.....##.##.....##.######...########.
.##.......##..####.##.......##.....##.##.....##.##.......##...##..
.##.......##...###.##....##.##.....##.##.....##.##.......##....##.
.########.##....##..######...#######..########..########.##.....##
*/

/*****************************************************************************/
/*! Configuration for Encoder                                                */
/*****************************************************************************/
typedef unsigned long NX10_ENC_CFG_SAMPLERATE;
#define NX10_ENC_CFG_FILTER_SAMPLE_RATE_NONE      0 /**< 0: none */
#define NX10_ENC_CFG_FILTER_SAMPLE_RATE_10NS      1 /**< 1: 10 ns */
#define NX10_ENC_CFG_FILTER_SAMPLE_RATE_20NS      2 /**< 2: 20 ns */
#define NX10_ENC_CFG_FILTER_SAMPLE_RATE_50NS      3 /**< 3: 50 ns */
#define NX10_ENC_CFG_FILTER_SAMPLE_RATE_100NS     4 /**< 4: 100 ns */
#define NX10_ENC_CFG_FILTER_SAMPLE_RATE_200NS     5 /**< 5: 200 ns */
#define NX10_ENC_CFG_FILTER_SAMPLE_RATE_500NS     6 /**< 6: 500 ns */
#define NX10_ENC_CFG_FILTER_SAMPLE_RATE_1000NS    7 /**< 7: 1 us */


/*****************************************************************************/
/*! Configuration for Encoder Capture Source (what to capture)               */
/*****************************************************************************/
typedef unsigned long NX10_ENC_CFG_CAPTSRC;
#define NX10_ENC_CFG_CAPTSRC_SYSTIME_NS            0 /**< 0: system time ns         */    
#define NX10_ENC_CFG_CAPTSRC_POS_CH01              1 /**< 1: position channel 0/1   */
#define NX10_ENC_CFG_CAPTSRC_TA_CH01               2 /**< 2: Ta of channel 0/1      */
#define NX10_ENC_CFG_CAPTSRC_TE_CH01               3 /**< 3: Te of channel 0/1      */
#define NX10_ENC_CFG_CAPTSRC_TA_TE_CH01            4 /**< 4: Ta+Te of channel 0/1   */
#define NX10_ENC_CFG_CAPTSRC_PERIOD_CH01           5 /**< 5: period in clockcycles  */


/*****************************************************************************/
/*! Configuration for Encoder Capture Start Signal                           */
/*****************************************************************************/
typedef unsigned long NX10_ENC_CFG_CAPTSEL;
#define NX10_ENC_CFG_CAPSEL_OFF                   0 /**< off (no capture) */
#define NX10_ENC_CFG_CAPSEL_ENC0_N                1 /**< enc0_n */
#define NX10_ENC_CFG_CAPSEL_ENC1_N                2 /**< enc1_n */
#define NX10_ENC_CFG_CAPSEL_ENC0_EDGE             3 /**< enc0_edge */
#define NX10_ENC_CFG_CAPSEL_ENC1_EDGE             4 /**< enc1_edge */
#define NX10_ENC_CFG_CAPSEL_MP0                   5 /**< mp0 */
#define NX10_ENC_CFG_CAPSEL_MP1                   6 /**< mp1 */
#define NX10_ENC_CFG_CAPSEL_PWM_CNT0_MIN          7 /**< pwm_cnt0_min */
#define NX10_ENC_CFG_CAPSEL_PWM_CNT0_MAX          8 /**< pwm_cnt0_max */
#define NX10_ENC_CFG_CAPSEL_PWM_CNT1_MIN          9 /**< pwm_cnt1_min */
#define NX10_ENC_CFG_CAPSEL_PWM_CNT1_MAX         10 /**< pwm_cnt1_max */
#define NX10_ENC_CFG_CAPSEL_PWM_T0               11 /**< pwm_t0 */
#define NX10_ENC_CFG_CAPSEL_PWM_T1               12 /**< pwm_t1 */
#define NX10_ENC_CFG_CAPSEL_PWM_T2               13 /**< pwm_t2 */
#define NX10_ENC_CFG_CAPSEL_PWM_T3               14 /**< pwm_t3 */
#define NX10_ENC_CFG_CAPSEL_PWM_T4               15 /**< pwm_t4 */
#define NX10_ENC_CFG_CAPSEL_PWM_T5               16 /**< pwm_t5 */
#define NX10_ENC_CFG_CAPSEL_PWM_T6               17 /**< pwm_t6 */
#define NX10_ENC_CFG_CAPSEL_PWM_T7               18 /**< pwm_t7 */
#define NX10_ENC_CFG_CAPSEL_XPIC_TIMER0          19 /**< xpic_timer0 */
#define NX10_ENC_CFG_CAPSEL_XPIC_TIMER1          20 /**< xpic_timer1 */
#define NX10_ENC_CFG_CAPSEL_XPIC_TIMER2          21 /**< xpic_timer2 */
#define NX10_ENC_CFG_CAPSEL_ARM_TIMER0           22 /**< arm_timer0 */
#define NX10_ENC_CFG_CAPSEL_ARM_TIMER1           23 /**< arm_timer0 */


/*******************************************************************/
/*! ENC Service Class: Structure for encoder channel configuration */
/*******************************************************************/
typedef struct NX10_MENC_ENCCHCFGtag 
{
  /* Config */
  NX10_ENC_CFG_SAMPLERATE eFilterSampleRate;              /**< Encoder filter sample rate for A/B/N */ 
  int                     fCountDir;                      /**< Encoder count direction */ 

  /* Interrupt enable */
  int                fIrqEnEdge;                     /**< Interrupt enable event edge @AB*/ 
  int                fIrqEnPhaseErr;                 /**< Interrupt enable event phase error */ 
  int                fIrqEnOvflPos;                  /**< Interrupt enable event overflow position -1->0 */ 
  int                fIrqEnOvflNeg;                  /**< Interrupt enable event overflow position 0->-1 */ 
  int                fIrqEnN;                        /**< Interrupt enable event rising edge @N */ 

  /* Initial position */
  unsigned long      ulInitPos;                      /**< Initial position of encoder */

} NX10_MENC_ENCCHCFG;

/*******************************************************************/
/*! ENC Service Class: Structure for MP channel configuration      */
/*******************************************************************/
typedef struct NX10_MENC_MPCHCFGtag 
{
  /* Config */
  NX10_ENC_CFG_SAMPLERATE eFilterSampleRate;              /**< Encoder filter sample rate for MP */ 

  /* Interrupt enable */
  int                fIrqEnMP;                       /**< Interrupt enable MP event */ 

} NX10_MENC_MPCHCFG;

/*******************************************************************/
/*! ENC Service Class: Structure for capture channel configuration */
/*******************************************************************/
typedef struct NX10_MENC_CAPCHCFGtag 
{
  /* Config */
  NX10_ENC_CFG_CAPTSRC    eCapSrc;                        /**< Capture source (what to capture) */ 
  NX10_ENC_CFG_CAPTSEL    eCapSel;                        /**< Capture start signal */ 
  int                fCapInv;                        /**< Invert capture start signal, 0: positive edge, 1: negative edge */ 
  int                fSrcNum;                        /**< 0/1: encoder channel 0/1 */ 
  int                fCapOnce;                       /**< 0:continuos capture: each event overwrites old capture register, 1: capture once: capture only, if menc_status.cap0 = 0 */ 

  /* Interrupt enable */
  int                fIrqEnCap;                      /**< Interrupt enable capture event */ 

} NX10_MENC_CAPCHCFG;
            
/***********************************************************************************************/
/*! Parameters ID that can be set/get by Pwm_SetPrm()/Pwm_GetPrm()                             */
/***********************************************************************************************/
typedef unsigned long NX10_ENC_PRM;
#define NX10_ENC_PRM_RESERVED                     1 /**< Reserved                                                   */

  /* Config */
#define NX10_ENC_PRM_ENC0_EN                      2 /**< Encoder0 enable (read-only)                                */
#define NX10_ENC_PRM_ENC0_FILT_SAMPLE_RATE        3 /**< Encoder0 filter sample rate                                */
#define NX10_ENC_PRM_ENC0_CNT_DIR                 4 /**< Encoder0 count direction                                   */
#define NX10_ENC_PRM_ENC1_EN                      5 /**< Encoder1 enable (read-only)                                */
#define NX10_ENC_PRM_ENC1_FILT_SAMPLE_RATE        6 /**< Encoder1 filter sample rate                                */
#define NX10_ENC_PRM_ENC1_CNT_DIR                 7 /**< Encoder0 count direction                                   */
#define NX10_ENC_PRM_MP0_EN                       8 /**< MP0 enable (read-only)                                     */
#define NX10_ENC_PRM_MP0_FILT_SAMPLE_RATE         9 /**< MP0 filter sample rate                                     */
#define NX10_ENC_PRM_MP1_EN                      10 /**< MP1 enable (read-only)                                     */
#define NX10_ENC_PRM_MP1_FILT_SAMPLE_RATE        11 /**< MP1 filter sample rate                                     */

  /* Position */
#define NX10_ENC_PRM_ENC0_POS                    12 /**< Actual position of encoder 0, This register is writable but can also be changed by hardware */
#define NX10_ENC_PRM_ENC1_POS                    13 /**< Actual position of encoder 1, This register is writable but can also be changed by hardware */

  /* Capture */
#define NX10_ENC_PRM_CAPTURE0_CFG                14 /**< Capture 0 configuration register */
#define NX10_ENC_PRM_CAPTURE0_VAL                15 /**< Capture 0 Captured value (read-only) */
#define NX10_ENC_PRM_CAPTURE0_TA                 16 /**< Capture 0 Captured Ta (read-only) */
#define NX10_ENC_PRM_CAPTURE0_TE                 17 /**< Capture 0 Captured Te (read-only) */

#define NX10_ENC_PRM_CAPTURE1_CFG                18 /**< Capture 1 configuration register */
#define NX10_ENC_PRM_CAPTURE1_VAL                19 /**< Capture 1 Captured value (read-only) */
#define NX10_ENC_PRM_CAPTURE1_TA                 20 /**< Capture 1 Captured Ta (read-only) */
#define NX10_ENC_PRM_CAPTURE1_TE                 21 /**< Capture 1 Captured Te (read-only) */

#define NX10_ENC_PRM_CAPTURE2_CFG                22 /**< Capture 2 configuration register */
#define NX10_ENC_PRM_CAPTURE2_VAL                23 /**< Capture 2 Captured value (read-only) */
#define NX10_ENC_PRM_CAPTURE2_TA                 24 /**< Capture 2 Captured Ta (read-only) */
#define NX10_ENC_PRM_CAPTURE2_TE                 25 /**< Capture 2 Captured Te (read-only) */

#define NX10_ENC_PRM_CAPTURE3_CFG                26 /**< Capture 3 configuration register */
#define NX10_ENC_PRM_CAPTURE3_VAL                27 /**< Capture 3 Captured value (read-only) */
#define NX10_ENC_PRM_CAPTURE3_TA                 28 /**< Capture 3 Captured Ta (read-only) */
#define NX10_ENC_PRM_CAPTURE3_TE                 29 /**< Capture 3 Captured Te (read-only) */

  /* Interrupts */
#define NX10_ENC_PRM_IRQMASKED                   30 /**< Shows status of masked IRQs (as connected to ARM/xPIC)     */
#define NX10_ENC_PRM_IRQMSKSET                   31 /**< interrupt mask enable                                      */
#define NX10_ENC_PRM_IRQMSKRST                   32 /**< interrupt mask disable                                     */



/***********************************************************************************************/
/* ENC Service Class: Function Prototypes                                                      */
/***********************************************************************************************/
NX10_MOTION_RESULT NX10_Enc_Start( int fEnc0En, int fEnc1En, int fMp0En, int fMp1En );
NX10_MOTION_RESULT NX10_Enc_Stop( int fEnc0Dis, int fEnc1Dis, int fMp0En, int fMp1En );
NX10_MOTION_RESULT NX10_Enc_ConfigEncCh( unsigned int uEncCh, NX10_MENC_ENCCHCFG* ptEncChCfg );
NX10_MOTION_RESULT NX10_Enc_ConfigMpCh( unsigned int uMpCh, NX10_MENC_MPCHCFG* ptMpChCfg );
NX10_MOTION_RESULT NX10_Enc_ConfigCapCh( unsigned int uCapCh, NX10_MENC_CAPCHCFG* ptMpChCfg );
NX10_MOTION_RESULT NX10_Enc_ResetEncCh( unsigned int uEncCh );
NX10_MOTION_RESULT NX10_Enc_ResetMpCh( unsigned int uMpCh );
NX10_MOTION_RESULT NX10_Enc_ResetCapCh( unsigned int uCapCh );

NX10_MOTION_RESULT NX10_Enc_GetStat( unsigned long *pulStat );
NX10_MOTION_RESULT NX10_Enc_ConfirmStatIrq( unsigned long ulConfirmStatIrqMask );
NX10_MOTION_RESULT NX10_Enc_GetIrq( unsigned long *pulIrq );
NX10_MOTION_RESULT NX10_Enc_CaptureNowReq( int fCh0En, int fCh1En, int fCh2En, int fCh3En );
NX10_MOTION_RESULT NX10_Enc_SetPrm( unsigned long ulPrmID, unsigned long ulPrmVal );
NX10_MOTION_RESULT NX10_Enc_GetPrm( unsigned long ulPrmID, unsigned long* pulPrmVal );

/*
....###....########...######.
...##.##...##.....##.##....##
..##...##..##.....##.##......
.##.....##.##.....##.##......
.#########.##.....##.##......
.##.....##.##.....##.##....##
.##.....##.########...######.
*/

#define NX10_ADC_NUM             2 /**< Number of ADC units             */
#define NX10_ADC_CHANNELS        8 /**< Number of channels per ADC unit */
#define NX10_ADC_OPERATIONAL     1 /**< Operational mode                */
#define NX10_ADC_POWER_DOWN      0 /**< Power-Down mode                 */
#define NX10_ADC_POWERUP_DELAY  20 /**< PowerUp delay (>1µs)            */
#define NX10_ADC_SEL2CONV_DELAY  5 /**< Select2 Convert delay (>80ns)   */

/*****************************************************************************/
/*! Select Event for AD-conversion start                                     */
/*****************************************************************************/
typedef unsigned long NX10_ADC_AUTOCFG_EVENTSEL;
#define NX10_ADC_AUTOCFG_EVENTSEL_STARTIMM         0 /**< 0 : start immediately */
#define NX10_ADC_AUTOCFG_EVENTSEL_ENC0_N           1 /**< 1 : enc0_n */
#define NX10_ADC_AUTOCFG_EVENTSEL_ENC1_N           2 /**< 2 : enc1_n */
#define NX10_ADC_AUTOCFG_EVENTSEL_ENC0_EDGE        3 /**< 3 : enc0_edge */
#define NX10_ADC_AUTOCFG_EVENTSEL_ENC1_EDGE        4 /**< 4 : enc1_edge */
#define NX10_ADC_AUTOCFG_EVENTSEL_MP0              5 /**< 5 : mp0 */
#define NX10_ADC_AUTOCFG_EVENTSEL_MP1              6 /**< 6 : mp1 */
#define NX10_ADC_AUTOCFG_EVENTSEL_PWM_CNT0_MIN     7 /**< 7 : pwm_cnt0_min */
#define NX10_ADC_AUTOCFG_EVENTSEL_PWM_CNT1_MIN     8 /**< 8 : pwm_cnt0_max */
#define NX10_ADC_AUTOCFG_EVENTSEL_PWM_CNT0_MAX     9 /**< 9 : pwm_cnt1_min */
#define NX10_ADC_AUTOCFG_EVENTSEL_PWM_CNT1_MAX    10 /**< 10: pwm_cnt1_max */
#define NX10_ADC_AUTOCFG_EVENTSEL_PWM_T0          11 /**< 11: pwm_t0 */
#define NX10_ADC_AUTOCFG_EVENTSEL_PWM_T1          12 /**< 12: pwm_t1 */
#define NX10_ADC_AUTOCFG_EVENTSEL_PWM_T2          13 /**< 13: pwm_t2 */
#define NX10_ADC_AUTOCFG_EVENTSEL_PWM_T3          14 /**< 14: pwm_t3 */
#define NX10_ADC_AUTOCFG_EVENTSEL_PWM_T4          15 /**< 15: pwm_t4 */
#define NX10_ADC_AUTOCFG_EVENTSEL_PWM_T5          16 /**< 16: pwm_t5 */
#define NX10_ADC_AUTOCFG_EVENTSEL_PWM_T6          17 /**< 17: pwm_t6 */
#define NX10_ADC_AUTOCFG_EVENTSEL_PWM_T7          18 /**< 18: pwm_t7 */
#define NX10_ADC_AUTOCFG_EVENTSEL_XPIC_TIMER0     19 /**< 19: xpic_timer0 */
#define NX10_ADC_AUTOCFG_EVENTSEL_XPIC_TIMER1     20 /**< 20: xpic_timer1 */
#define NX10_ADC_AUTOCFG_EVENTSEL_XPIC_TIMER2     21 /**< 21: xpic_timer2 */
#define NX10_ADC_AUTOCFG_EVENTSEL_ARM_TIMER0      22 /**< 22: arm_timer0  */
#define NX10_ADC_AUTOCFG_EVENTSEL_ARM_TIMER1      23 /**< 23: arm_timer1  */


/***********************************************************************************************/
/* ADC Service Class: Function Prototypes                                                      */
/***********************************************************************************************/
NX10_MOTION_RESULT NX10_Adc_SetOpmode( unsigned int uAdcNum, int fOpMode );
NX10_MOTION_RESULT NX10_Adc_StartAuto( int fStartAdc0, int fStartAdc1, int fStart2ndAdc0, int fStart2ndAdc1 );
NX10_MOTION_RESULT NX10_Adc_ConfigAuto( unsigned int uAdcNum, unsigned int uChannel, unsigned int uPwrDown, unsigned int uInv, NX10_ADC_AUTOCFG_EVENTSEL eEventSel );
NX10_MOTION_RESULT NX10_Adc_StartMan( unsigned int uAdcNum, unsigned int uChannel );
NX10_MOTION_RESULT NX10_Adc_GetStatus( unsigned int uAdcNum, int* pfFin, int* pfEocb );
NX10_MOTION_RESULT NX10_Adc_GetAdcVal( unsigned int uAdcNum, unsigned short* pusVal );
NX10_MOTION_RESULT NX10_Adc_EnableIrq( unsigned long ulIrqMask );
NX10_MOTION_RESULT NX10_Adc_DisableIrq( unsigned long ulIrqMask );
NX10_MOTION_RESULT NX10_Adc_GetIrq( unsigned long *pulIrq );
NX10_MOTION_RESULT NX10_Adc_GetIrqRaw( unsigned long *pulIrq );
NX10_MOTION_RESULT NX10_Adc_ConfirmIrq( unsigned long ulConfirmIrqMask );



/*
.########..##.....##....###.....######.
.##.....##.###...###...##.##...##....##
.##.....##.####.####..##...##..##......
.##.....##.##.###.##.##.....##.##......
.##.....##.##.....##.#########.##......
.##.....##.##.....##.##.....##.##....##
.########..##.....##.##.....##..######.
*/

/*****************************************************************************/
/*! Result codes for functions                                               */
/*****************************************************************************/
typedef unsigned long NX10_DMAC_RESULT;
#define NX10_DMAC_OKAY                             1 /**< Successful                                   */
#define NX10_DMAC_ERR_INVAL_CHANNEL                2 /**< Invalid DMA channel specified                */


/*******************************************************************/
/*! DMA channel configuration                                      */
/*******************************************************************/
typedef struct NX10_DMAC_CHCFGtag 
{
  unsigned long ulDmacChSrcAd;                        /**< Channel source address registers */ 
  unsigned long ulDmacChDestAd;                       /**< Channel destination address registers */ 
  unsigned long ulDmacChLink;                         /**< Channel linked list item register */ 
  unsigned long ulDmacChCtrl;                         /**< Channel control registers */ 
  unsigned long ulDmacChCfg;                          /**< Channel configuration registers, Bits "Halt" and "Enable" of the channel 
                                                           configuration register are ignored within Dmac_ConfigChannel(). */ 
} NX10_DMAC_CHCFG;
 
/***********************************************************************************************/
/* DMAC Service Class: Function Prototypes                                                      */
/***********************************************************************************************/
NX10_DMAC_RESULT NX10_Dmac_Config( int fEnable );
NX10_DMAC_RESULT NX10_Dmac_GetIrq( unsigned long* pulIrqTc, unsigned long* pulIrqErr );
NX10_DMAC_RESULT NX10_Dmac_ConfirmIrq( unsigned long ulConfirmIrqTcMask, unsigned long ulConfirmIrqErrMask );
NX10_DMAC_RESULT NX10_Dmac_ConfigChannel( unsigned long ulChannel, NX10_DMAC_CHCFG* ptDmaChCfg );
NX10_DMAC_RESULT NX10_Dmac_EnableChannel( unsigned long ulChannel );
NX10_DMAC_RESULT NX10_Dmac_DisableChannel( unsigned long ulChannel );


/*
....###....########..##.....##.........########.####.##.....##.########.########.
...##.##...##.....##.###...###............##.....##..###...###.##.......##.....##
..##...##..##.....##.####.####............##.....##..####.####.##.......##.....##
.##.....##.########..##.###.##............##.....##..##.###.##.######...########.
.#########.##...##...##.....##............##.....##..##.....##.##.......##...##..
.##.....##.##....##..##.....##............##.....##..##.....##.##.......##....##.
.##.....##.##.....##.##.....##.#######....##....####.##.....##.########.##.....##
*/

/*****************************************************************************/
/*! Result codes for functions                                               */
/*****************************************************************************/
typedef unsigned long NX10_ARMTIMER_RESULT;
#define NX10_ARMTIMER_OKAY                         1 /**< Successful          */
#define NX10_ARMTIMER_ERR_INVAL_PARAM              2 /**< Invalid parameter   */


#define NX10_ARMTIMER_CHANNELS      2 /**< Number of timers */

/*****************************************************************************/
/*! Configuration for ARM timer                                              */
/*****************************************************************************/
typedef unsigned long NX10_ARMTIMER_CFG_MODE;
#define NX10_ARMTIMER_CFG_MODE_STOP_AT_0           0 /**< 0: Timer stops at 0                                       */    
#define NX10_ARMTIMER_CFG_MODE_PRELOAD_AT_0        1 /**< 1: Timer is preload with value from preload register at 0 */
#define NX10_ARMTIMER_CFG_MODE_SYSTIME             2 /**< 2: Timer (value) compare with systime (once)              */
#define NX10_ARMTIMER_CFG_MODE_RESERVED            3 /**< 3: reserved                                               */


/***********************************************************************************************/
/*! Parameters ID that can be get by ArmTimer_GetPrm()                                         */
/***********************************************************************************************/
typedef unsigned long NX10_ARMTIMER_PRM;
#define NX10_ARMTIMER_PRM_CONFIG_TIMER0            1 /**< Register arm_timer_config_timer0      */
#define NX10_ARMTIMER_PRM_CONFIG_TIMER1            2 /**< Register arm_timer_config_timer1      */
#define NX10_ARMTIMER_PRM_PRELOAD_TIMER0           3 /**< Register arm_timer_preload_timer0     */
#define NX10_ARMTIMER_PRM_PRELOAD_TIMER1           4 /**< Register arm_timer_preload_timer1     */
#define NX10_ARMTIMER_PRM_TIMER0                   5 /**< Register arm_timer_timer0             */
#define NX10_ARMTIMER_PRM_TIMER1                   6 /**< Register arm_timer_timer1             */
#define NX10_ARMTIMER_PRM_SYSTIME_NS_COMPARE       7 /**< Register arm_timer_systime_ns_compare */
#define NX10_ARMTIMER_PRM_SYSTIME_S_COMPARE        8 /**< Register arm_timer_systime_s_compare  */
#define NX10_ARMTIMER_PRM_IRQMSKSET                9 /**< interrupt mask enable                 */
#define NX10_ARMTIMER_PRM_IRQMSKRST               10 /**< interrupt mask disable                */

 
/***********************************************************************************************/
/* ARMTIMER Service Class: Function Prototypes                                                 */
/***********************************************************************************************/
NX10_ARMTIMER_RESULT NX10_ArmTimer_Start( unsigned int uInst, NX10_ARMTIMER_CFG_MODE eMode, unsigned long ulVal );
NX10_ARMTIMER_RESULT NX10_ArmTimer_Stop( unsigned int uInst  );
NX10_ARMTIMER_RESULT NX10_ArmTimer_StartSystimeNsCmp( unsigned long ulVal );
NX10_ARMTIMER_RESULT NX10_ArmTimer_StartSystimeSecCmp( unsigned long ulVal );
NX10_ARMTIMER_RESULT NX10_ArmTimer_EnIrq( int fTimer0IrqEn, int fTimer1IrqEn, int fSystimeNsIrqEn, int fSystimeSecIrqEn );
NX10_ARMTIMER_RESULT NX10_ArmTimer_DisIrq( int fTimer0IrqDis, int fTimer1IrqDis, int fSystimeNsIrqDis, int fSystimeSecIrqDis );
NX10_ARMTIMER_RESULT NX10_ArmTimer_GetIrq( unsigned long *pulIrq );
NX10_ARMTIMER_RESULT NX10_ArmTimer_GetIrqRaw( unsigned long *pulIrq );
NX10_ARMTIMER_RESULT NX10_ArmTimer_ConfirmIrq( unsigned long ulConfirmIrqMask );
NX10_ARMTIMER_RESULT NX10_ArmTimer_GetPrm( unsigned long ulPrmID, unsigned long* pulPrmVal );

/*
.##.....##.########..####..######..........########.####.##.....##.########.########.
..##...##..##.....##..##..##....##............##.....##..###...###.##.......##.....##
...##.##...##.....##..##..##..................##.....##..####.####.##.......##.....##
....###....########...##..##..................##.....##..##.###.##.######...########.
...##.##...##.........##..##..................##.....##..##.....##.##.......##...##..
..##...##..##.........##..##....##............##.....##..##.....##.##.......##....##.
.##.....##.##........####..######..#######....##....####.##.....##.########.##.....##
*/

/*****************************************************************************/
/*! Result codes for functions                                               */
/*****************************************************************************/
typedef unsigned long NX10_XPICTIMER_RESULT;
#define NX10_XPICTIMER_OKAY                        1 /**< Successful          */
#define NX10_XPICTIMER_ERR_INVAL_PARAM             2 /**< Invalid parameter   */


#define NX10_XPICTIMER_CHANNELS      3   /**< Number of timers */

/*****************************************************************************/
/*! Configuration for XPIC timer                                             */
/*****************************************************************************/
typedef unsigned long NX10_XPICTIMER_CFG_MODE;
#define NX10_XPICTIMER_CFG_MODE_STOP_AT_0           0 /**< 0: Timer stops at 0                                       */    
#define NX10_XPICTIMER_CFG_MODE_PRELOAD_AT_0        1 /**< 1: Timer is preload with value from preload register at 0 */
#define NX10_XPICTIMER_CFG_MODE_SYSTIME             2 /**< 2: Timer (value) compare with systime (once)              */
#define NX10_XPICTIMER_CFG_MODE_RESERVED            3 /**< 3: reserved                                               */


/***********************************************************************************************/
/*! Parameters ID that can be get by ArmTimer_GetPrm()                                         */
/***********************************************************************************************/
typedef unsigned long NX10_XPICTIMER_PRM;
#define NX10_XPICTIMER_PRM_CONFIG_TIMER0            1 /**< Register xpic_timer_config_timer0      */
#define NX10_XPICTIMER_PRM_CONFIG_TIMER1            2 /**< Register xpic_timer_config_timer1      */
#define NX10_XPICTIMER_PRM_CONFIG_TIMER2            3 /**< Register xpic_timer_config_timer2      */
#define NX10_XPICTIMER_PRM_PRELOAD_TIMER0           4 /**< Register xpic_timer_preload_timer0     */
#define NX10_XPICTIMER_PRM_PRELOAD_TIMER1           5 /**< Register xpic_timer_preload_timer1     */
#define NX10_XPICTIMER_PRM_PRELOAD_TIMER2           6 /**< Register xpic_timer_preload_timer2     */
#define NX10_XPICTIMER_PRM_TIMER0                   7 /**< Register xpic_timer_timer0             */
#define NX10_XPICTIMER_PRM_TIMER1                   8 /**< Register xpic_timer_timer1             */
#define NX10_XPICTIMER_PRM_TIMER2                   9 /**< Register xpic_timer_timer1             */
#define NX10_XPICTIMER_PRM_SYSTIME_NS              10 /**< Register xpic_timer_systime_ns         */
#define NX10_XPICTIMER_PRM_SYSTIME_S               11 /**< Register xpic_timer_systime_s          */
#define NX10_XPICTIMER_PRM_IRQMSKSET               12 /**< interrupt mask enable                  */
#define NX10_XPICTIMER_PRM_IRQMSKRST               13 /**< interrupt mask disable                 */

 
/***********************************************************************************************/
/* XPICTIMER Service Class: Function Prototypes                                                 */
/***********************************************************************************************/
NX10_XPICTIMER_RESULT NX10_XpicTimer_Start( unsigned int uInst, NX10_XPICTIMER_CFG_MODE eMode, unsigned long ulVal );
NX10_XPICTIMER_RESULT NX10_XpicTimer_Stop( unsigned int uInst );
NX10_XPICTIMER_RESULT NX10_XpicTimer_EnIrq( int fTimer0IrqEn, int fTimer1IrqEn, int fTimer2IrqEn );
NX10_XPICTIMER_RESULT NX10_XpicTimer_DisIrq( int fTimer0IrqDis, int fTimer1IrqDis, int fTimer2IrqDis );
NX10_XPICTIMER_RESULT NX10_XpicTimer_GetIrq( unsigned long *pulIrq );
NX10_XPICTIMER_RESULT NX10_XpicTimer_GetIrqRaw( unsigned long *pulIrq );
NX10_XPICTIMER_RESULT NX10_XpicTimer_ConfirmIrq( unsigned long ulConfirmIrqMask );
NX10_XPICTIMER_RESULT NX10_XpicTimer_GetPrm( unsigned long ulPrmID, unsigned long* pulPrmVal );
NX10_XPICTIMER_RESULT NX10_XpicTimer_GetSystime( unsigned long* pulSystime_s, unsigned long* pulSystime_ns );



//                                                                  
// ######  #    #  #    #   ####   #####  #   ####   #    #   ####  
// #       #    #  ##   #  #    #    #    #  #    #  ##   #  #      
// #####   #    #  # #  #  #         #    #  #    #  # #  #   ####  
// #       #    #  #  # #  #         #    #  #    #  #  # #       # 
// #       #    #  #   ##  #    #    #    #  #    #  #   ##  #    # 
// #        ####   #    #   ####     #    #   ####   #    #   ####  
//                                                                  
//                                                        #     #     #     #       
// #    #   ####   ######  #####       #####   #   #      #     #    # #    #       
// #    #  #       #       #    #      #    #   # #       #     #   #   #   #       
// #    #   ####   #####   #    #      #####     #        #######  #     #  #       
// #    #       #  #       #    #      #    #    #        #     #  #######  #       
// #    #  #    #  #       #    #      #    #    #        #     #  #     #  #       
//  ####    ####   ######  #####       #####     #        #     #  #     #  ####### 
//                                                                                  


// __  __   ____           _                           _               
// \ \/ /  / ___|         | |       ___     __ _    __| |   ___   _ __ 
//  \  /  | |             | |      / _ \   / _` |  / _` |  / _ \ | '__|
//  /  \  | |___          | |___  | (_) | | (_| | | (_| | |  __/ | |   
// /_/\_\  \____|  _____  |_____|  \___/   \__,_|  \__,_|  \___| |_|   
//                |_____|                                              

/*****************************************************************************/
/* Definitions                                                               */
/*****************************************************************************/
#define NX10_XMAC_RPU_DWORD_RAMSIZE 0x0100
#define NX10_XMAC_TPU_DWORD_RAMSIZE 0x0100
#define NX10_XPEC_DWORD_RAMSIZE     0x0800

typedef enum {
  NX10_XC_TYPE_XPEC,
  NX10_XC_TYPE_RPU,
  NX10_XC_TYPE_TPU
} NX10_XC_TYPE_E;

/*****************************************************************************/
/* Function prototypes                                                       */
/*****************************************************************************/
int NX10_XC_Reset(unsigned int uPortNo, void* pvUser);
int NX10_XC_Load(unsigned int uPortNo, NX10_XC_TYPE_E eXcType, const unsigned long* pulXcPrg, void* pvUser);
int NX10_XC_Start(unsigned int uPortNo, void* pvUser);

//  __  __   ___   ___   __  __   _   _ 
// |  \/  | |_ _| |_ _| |  \/  | | | | |
// | |\/| |  | |   | |  | |\/| | | | | |
// | |  | |  | |   | |  | |  | | | |_| |
// |_|  |_| |___| |___| |_|  |_|  \___/ 
//                                      

/*****************************************************************************/
/* Definitions                                                               */
/*****************************************************************************/
#define NX10_MIIMU_SUPPRESS_PREAMBLE              (0)             
#define NX10_MIIMU_SEND_PREAMBLE                  (1)             
#define NX10_MIIMU_MDCFREQ_HIGH                   (0)             
#define NX10_MIIMU_MDCFREQ_LOW                    (1)             
#define NX10_MIIMU_RTA_0_BITS                     (0)             
#define NX10_MIIMU_RTA_1_BITS                     (1)             

/* NEC CB12 registers */
#define NX10_DRV_CB12_CONTROL                     (0)             /* Control                                         */
#define NX10_DRV_CB12_STATUS                      (1)             /* Status #1                                       */
#define NX10_DRV_CB12_PHY_ID_1                    (2)             /* PHY Identification 1                            */
#define NX10_DRV_CB12_PHY_ID_2                    (3)             /* PHY Identification 2                            */
#define NX10_DRV_CB12_AUTO_NEG_ADVERTISEMENT      (4)             /* Auto-Negotiation Advertisement                  */
#define NX10_DRV_CB12_AUTO_NEG_LINK_PARTNER_ABIL  (5)             /* Auto-Negotiation Link Partner Ability           */
#define NX10_DRV_CB12_AUTO_NEG_EXPANSION          (6)             /* Auto-Negotiation Expansion                      */
#define NX10_DRV_CB12_AUTO_NEG_NEXT_PAGE_TX       (7)             /* Auto-Negotiation Next Page Transmit             */
#define NX10_DRV_CB12_SILICON_REVISION            (16)            /* Silicon Revision                                */
#define NX10_DRV_CB12_MODE_CONTROL_STATUS         (17)            /* Mode Control/Status                             */
#define NX10_DRV_CB12_SPECIAL_MODES               (18)            /* Special Modes                                   */
#define NX10_DRV_CB12_SMII_CONFIGURATION_STATUS   (19)            /* SMII configuration status                       */
#define NX10_DRV_CB12_CONTROL_STATUS_INDICATION   (27)            /* Control/Status Indication                       */
#define NX10_DRV_CB12_SPECIAL_INT_TEST_CONTROLS   (28)            /* Special internal testability controls           */
#define NX10_DRV_CB12_INTERRUPT_SOURCE            (29)            /* Interrupt source                                */
#define NX10_DRV_CB12_INTERRUPT_MASK              (30)            /* Interrupt mask                                  */
#define NX10_DRV_CB12_PHY_SPECIAL_CONTROL_STATUS  (31)            /* PHY special control/status                      */

/* Register 0 - Basic Control Register Bit Definition */
#define NX10_DRV_CB12_CONTROL_RESET                  (0x8000)        /* PHY reset                          */
#define NX10_DRV_CB12_CONTROL_LOOPBACK               (0x4000)        /* Enable loopback                    */
#define NX10_DRV_CB12_CONTROL_SPEED_SELECT_100       (0x2000)        /* Select Speed 100MBit               */
#define NX10_DRV_CB12_CONTROL_AUTO_NEG_ENABLE        (0x1000)        /* Auto-Negotiation Enable            */
#define NX10_DRV_CB12_CONTROL_POWER_DOWN             (0x0800)        /* Power-down                         */
#define NX10_DRV_CB12_CONTROL_ISOLATE                (0x0400)        /* Electrically Isolate PHY from MII  */
#define NX10_DRV_CB12_CONTROL_AUTO_NEG_RESTART       (0x0200)        /* Restart Auto-Negotiation           */
#define NX10_DRV_CB12_CONTROL_FULL_DUPLEX            (0x0100)        /* Full Duplex Mode                   */
#define NX10_DRV_CB12_CONTROL_COL_TEST               (0x0080)        /* Enable COL signal test             */

/* Register 1 - Basic Status Register Bit Definition */
#define NX10_DRV_CB12_STATUS_100_BASE_T4             (0x8000)        /* 100BASE-T4 support                 */
#define NX10_DRV_CB12_STATUS_100_BASE_X_FDX          (0x4000)        /* 100BASE-X full duplex support      */
#define NX10_DRV_CB12_STATUS_100_BASE_X_HDX          (0x2000)        /* 100BASE-X half duplex support      */
#define NX10_DRV_CB12_STATUS_10_MBPS_FDX             (0x1000)        /* 10 Mbps full duplex support        */
#define NX10_DRV_CB12_STATUS_10_MBPS_HDX             (0x0800)        /* 10 Mbps half duplex support        */
#define NX10_DRV_CB12_STATUS_AUTO_NEG_COMPLETE       (0x0020)        /* Auto-Negotiation complete          */
#define NX10_DRV_CB12_STATUS_REMOTE_FAULT            (0x0010)        /* Remote fault detected              */
#define NX10_DRV_CB12_STATUS_AUTO_NEG_ABILITY        (0x0008)        /* Auto-Negotiation ability           */
#define NX10_DRV_CB12_STATUS_LINK_UP                 (0x0004)        /* Link status                        */
#define NX10_DRV_CB12_STATUS_JABBER_DETECT           (0x0002)        /* Jabber detected                    */
#define NX10_DRV_CB12_STATUS_EXTENDED_CAPABILITY     (0x0001)        /* Basic/extended register capability */

/* Register 4 - Auto Negotiation Advertisement Register Bit Definition */
#define NX10_DRV_CB12_ADV_NEXT_PAGE                  (0x8000)        /* Ability to send multiple pages        */
#define NX10_DRV_CB12_ADV_REMOTE_FAULT               (0x2000)        /* Remote fault                          */
#define NX10_DRV_CB12_ADV_PAUSE_1                    (0x0800)        /* <11> = Symmetric and Asymmetric pause */
#define NX10_DRV_CB12_ADV_PAUSE_0                    (0x0400)        /* <10> = Symmetric pause                */
                                                                /* <01> = Asymmetric PAUSE               */                                                               /* <00> = NO PAUSE                       */
#define NX10_DRV_CB12_ADV_100_BASE_T4                (0x0200)        /* 100BASE-T4 capability (not supp.)     */
#define NX10_DRV_CB12_ADV_100_BASE_TX_FDX            (0x0100)        /* 100BASE-TX full-duplex capability     */
#define NX10_DRV_CB12_ADV_100_BASE_TX                (0x0080)        /* 100BASE-TX capability                 */
#define NX10_DRV_CB12_ADV_10_BASE_T_FDX              (0x0040)        /* 10BASE-T full-duplex capability       */
#define NX10_DRV_CB12_ADV_10_BASE_T                  (0x0020)        /* 10BASE-T capability                   */
#define NX10_DRV_CB12_ADV_SELECT_4                   (0x0010)        /* Selector Field:                       */
#define NX10_DRV_CB12_ADV_SELECT_3                   (0x0008)        /*  <00001> = IEEE 802.3                 */
#define NX10_DRV_CB12_ADV_SELECT_2                   (0x0004)        /*  <00010> = IEEE 802.9 ISLAN-16T       */
#define NX10_DRV_CB12_ADV_SELECT_1                   (0x0002)        /*  <00000> = Reserved                   */
#define NX10_DRV_CB12_ADV_SELECT_0                   (0x0001)        /*  <11111> = Reserved                   */

/* Register 17 - Mode Control/Status */
#define NX10_DRV_CB12_MODE_CTRL_STAT_FASTRIP                    (0x4000)        /* 10BASE-T fast mode                       */
#define NX10_DRV_CB12_MODE_CTRL_STAT_EDPWRDOWN                  (0x2000)        /* Enable the Energy Detect Power-Down mode */
#define NX10_DRV_CB12_MODE_CTRL_STAT_LOWSQEN                    (0x0800)        /* Low squelch signal                       */
#define NX10_DRV_CB12_MODE_CTRL_STAT_MDPREBP                    (0x0400)        /* Management Data Preamble bypass          */
#define NX10_DRV_CB12_MODE_CTRL_STAT_FARLOOPBACK                (0x0200)        /* Remote Loopback enable                   */
#define NX10_DRV_CB12_MODE_CTRL_STAT_FASTEST                    (0x0100)        /* Auto-Negotiation Test mode               */
#define NX10_DRV_CB12_MODE_CTRL_STAT_AUTO_MDIX_EN               (0x0080)        /* AutoMDIX enable bit                      */
#define NX10_DRV_CB12_MODE_CTRL_STAT_MDI_MODE                   (0x0040)        /* Manual set of MDIX/MDI mode <1> = MDIX   */
#define NX10_DRV_CB12_MODE_CTRL_STAT_DCD_PAT_GEN                (0x0010)        /* Enables DCD measuring pattern generation */
#define NX10_DRV_CB12_MODE_CTRL_STAT_PHYADBP                    (0x0008)        /* PYH disregards PHY address in SMI access */
#define NX10_DRV_CB12_MODE_CTRL_STAT_FORCE_GOOD_LINK_STATUS     (0x0004)        /* Force 100BASE-X link active              */
#define NX10_DRV_CB12_MODE_CTRL_STAT_ENERGYON                   (0x0002)        /* Energy detected on line                  */

/* Register 18 - Special Modes */
#define NX10_DRV_CB12_SPECIAL_MODE_MIIMODE_1                    (0x8000)      /* <11> = res, <10> = SMII                              */
#define NX10_DRV_CB12_SPECIAL_MODE_MIIMODE_0                    (0x4000)      /* <01> = RMII, <00> = MII                              */                       */
#define NX10_DRV_CB12_SPECIAL_MODE_CLKSELFREQ                   (0x2000)      /* Clock frequency selection <0> = 25 MHz, <1> = 50 MHz */
#define NX10_DRV_CB12_SPECIAL_MODE_FX_MODE                      (0x0400)      /* Enable 100BASE-FX mode                      */
#define NX10_DRV_CB12_SPECIAL_MODE_FARLOOPBACK                  (0x0200)      /* Remote Loopback enable                      */
#define NX10_DRV_CB12_SPECIAL_MODE_PYH_MODE_2                   (0x0080)      /* PHY mode of operation Refer to 7.2 in spec  */
#define NX10_DRV_CB12_SPECIAL_MODE_PYH_MODE_1                   (0x0040)      /* PHY mode of operation Refer to 7.2 in spec  */
#define NX10_DRV_CB12_SPECIAL_MODE_PYH_MODE_0                   (0x0020)      /* PHY mode of operation Refer to 7.2 in spec  */
#define NX10_DRV_CB12_SPECIAL_MODE_PHY_ADD_4                    (0x0010)      /* PHY address Refer to 7.1 in spec            */
#define NX10_DRV_CB12_SPECIAL_MODE_PHY_ADD_3                    (0x0008)      /* PHY address Refer to 7.1 in spec            */
#define NX10_DRV_CB12_SPECIAL_MODE_PHY_ADD_2                    (0x0004)      /* PHY address Refer to 7.1 in spec            */
#define NX10_DRV_CB12_SPECIAL_MODE_PHY_ADD_1                    (0x0002)      /* PHY address Refer to 7.1 in spec            */
#define NX10_DRV_CB12_SPECIAL_MODE_PHY_ADD_0                    (0x0001)      /* PHY address Refer to 7.1 in spec            */

/* Register 27 - Special Control/Status indications */
#define NX10_DRV_CB12_SPECIAL_CTRL_STAT_SWRST_FAST              (0x1000)      /* SW reset counter testing                           */                          
#define NX10_DRV_CB12_SPECIAL_CTRL_STAT_SQEOFF                  (0x0800)      /* Disable SQE (Heartbeat) test                       */                      
#define NX10_DRV_CB12_SPECIAL_CTRL_STAT_FEFIEN                  (0x0020)      /* Far end fault indication enable                    */                   
#define NX10_DRV_CB12_SPECIAL_CTRL_STAT_XPOL                    (0x0010)      /* Polarity state of 10BASE-T <1> = reversed polarity */

/* Register 31 - PHY Special Control/Status indications */
#define NX10_DRV_CB12_PYH_SPECIAL_CTRL_STAT_AUTODONE            (0x1000)      /* Auto-Negotiation done         */
#define NX10_DRV_CB12_PYH_SPECIAL_CTRL_STAT_4B5BEN              (0x0040)      /* Enable 4B5B Encoding/Decoding */
#define NX10_DRV_CB12_PYH_SPECIAL_CTRL_STAT_DATA_SCRAMBLING_DIS (0x0001)      /* Scramble disabling            */

/*****************************************************************************/
/* Function prototypes                                                       */
/*****************************************************************************/
int NX10_MIIMU_ReadPhyReg( unsigned int uMiimuPreamble, unsigned int uMiimuMdcFreq, unsigned int uMiimuRtaField, unsigned int uMiimuPhyAddr, unsigned int uMiimuReqAddr, unsigned short* pusData, void* pvUser);
int NX10_MIIMU_WritePhyReg( unsigned int uMiimuPreamble, unsigned int uMiimuMdcFreq, unsigned int uMiimuPhyAddr, unsigned int uMiimuReqAddr, unsigned short usData, void* pvUser);

/*
  ____                  _     _                    
 / ___|   _   _   ___  | |_  (_)  _ __ ___     ___ 
 \___ \  | | | | / __| | __| | | | '_ ` _ \   / _ \
  ___) | | |_| | \__ \ | |_  | | | | | | | | |  __/
 |____/   \__, | |___/  \__| |_| |_| |_| |_|  \___|
          |___/                                    

*/

/*****************************************************************************/
/* Function prototypes                                                       */
/*****************************************************************************/
void NX10_SYSTIME_SetBorder( unsigned long ulBorder, void* pvUser );
void NX10_SYSTIME_SetSpeed( unsigned long ulSpeed, void* pvUser );
void NX10_SYSTIME_SetSystime( unsigned long ulSystime_s, unsigned long ulSystime_ns, void* pvUser );
void NX10_SYSTIME_GetSystime( unsigned long* pulSystime_s, unsigned long* pulSystime_ns, void* pvUser );


/*
    _        _          ____ _        _
   / \   ___(_) ___    / ___| |_ _ __| |
  / _ \ / __| |/ __|  | |   | __| '__| |
 / ___ \\__ \ | (__   | |___| |_| |  | |
/_/   \_\___/_|\___|___\____|\__|_|  |_|
                  |_____|
*/

/*****************************************************************************/
/* Function prototypes                                                       */
/*****************************************************************************/
void NX10_AsicCtrl_SetIoConfig(unsigned long ulVal);
void NX10_AsicCtrl_GetIoConfig(unsigned long* pulVal);
void NX10_AsicCtrl_SetClockEnable(unsigned long ulVal);
void NX10_AsicCtrl_GetClockEnable(unsigned long* pulVal);
void NX10_AsicCtrl_SetFb0Clk( unsigned long ulFb0ClkRateMulAdd, unsigned long ulFb0ClkDiv );

/*
.##.....##.########..####..######.
..##...##..##.....##..##..##....##
...##.##...##.....##..##..##......
....###....########...##..##......
...##.##...##.........##..##......
..##...##..##.........##..##....##
.##.....##.##........####..######.
*/

/*****************************************************************************/
/* Function prototypes                                                       */
/*****************************************************************************/
void NX10_XPIC_Reset(void* pvUser);
void NX10_XPIC_Load(const unsigned long *pulXpicPrg, void* pvUser);
void NX10_XPIC_Start(void* pvUser);
void NX10_XPIC_Stop(void* pvUser);

#endif // #define __COMMON_FUNC_NETX10_H
                                  
