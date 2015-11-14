/******************************************************************************
 * @file     main.c
 * @version  V1.00
 * $Revision: 4 $
 * $Date: 15/07/02 11:18a $
 * @brief    Software Development Template.
 * @note
 * Copyright (C) 2014~2015 Nuvoton Technology Corp. All rights reserved.
*****************************************************************************/
#include <stdio.h>
#include "NUC123.h"
#include "spiutil.h"
#include "flash.h"
#include "sram.h"
#include "fds.h"
#include "hid_transfer.h"

#define HCLK_CLOCK           72000000

void SYS_Init(void)
{
    /*---------------------------------------------------------------------------------------------------------*/
    /* Init System Clock                                                                                       */
    /*---------------------------------------------------------------------------------------------------------*/

    /* Enable XT1_OUT(PF0) and XT1_IN(PF1) */
    SYS->GPF_MFP |= SYS_GPF_MFP_PF0_XT1_OUT | SYS_GPF_MFP_PF1_XT1_IN;

    /* Enable Internal RC 22.1184MHz clock */
    CLK_EnableXtalRC(CLK_PWRCON_OSC22M_EN_Msk);

    /* Waiting for Internal RC clock ready */
    CLK_WaitClockReady(CLK_CLKSTATUS_OSC22M_STB_Msk);

    /* Switch HCLK clock source to Internal RC and HCLK source divide 1 */
    CLK_SetHCLK(CLK_CLKSEL0_HCLK_S_HIRC, CLK_CLKDIV_HCLK(1));

    /* Enable external XTAL 12MHz clock */
    CLK_EnableXtalRC(CLK_PWRCON_XTL12M_EN_Msk);

    /* Waiting for external XTAL clock ready */
    CLK_WaitClockReady(CLK_CLKSTATUS_XTL12M_STB_Msk);

    /* Set core clock as HCLK_CLOCK */
    CLK_SetCoreClock(HCLK_CLOCK);

    /* Enable module clocks */
    CLK_EnableModuleClock(UART0_MODULE);
    CLK_EnableModuleClock(SPI0_MODULE);
    CLK_EnableModuleClock(SPI1_MODULE);
    CLK_EnableModuleClock(TMR0_MODULE);
    CLK_EnableModuleClock(TMR1_MODULE);
    CLK_EnableModuleClock(TMR2_MODULE);
    CLK_EnableModuleClock(USBD_MODULE);

    /* Select HCLK as the clock source of SPI0 */
    CLK_SetModuleClock(UART0_MODULE, CLK_CLKSEL1_UART_S_HXT, CLK_CLKDIV_UART(1));
    CLK_SetModuleClock(SPI0_MODULE, CLK_CLKSEL1_SPI0_S_HCLK, MODULE_NoMsk);
    CLK_SetModuleClock(SPI1_MODULE, CLK_CLKSEL1_SPI1_S_HCLK, MODULE_NoMsk);
    CLK_SetModuleClock(TMR0_MODULE, CLK_CLKSEL1_TMR0_S_HCLK, 0);
    CLK_SetModuleClock(TMR1_MODULE, CLK_CLKSEL1_TMR1_S_HCLK, 0);
    CLK_SetModuleClock(TMR2_MODULE, CLK_CLKSEL1_TMR2_S_HCLK, 0);
    CLK_SetModuleClock(USBD_MODULE, 0, CLK_CLKDIV_USB(3));

    /* Select UART module clock source */

    /*---------------------------------------------------------------------------------------------------------*/
    /* Init I/O Multi-function                                                                                 */
    /*---------------------------------------------------------------------------------------------------------*/

    /* Set GPB multi-function pins for UART0 RXD(PB.0) and TXD(PB.1) */
    SYS->GPC_MFP &= ~(SYS_GPC_MFP_PC4_Msk | SYS_GPC_MFP_PC5_Msk);
    SYS->GPC_MFP = SYS_GPC_MFP_PC4_UART0_RXD | SYS_GPC_MFP_PC5_UART0_TXD;
	SYS->ALT_MFP = SYS_ALT_MFP_PC4_UART0_RXD | SYS_ALT_MFP_PC5_UART0_TXD;

    /* Setup SPI0 multi-function pins */
    SYS->GPC_MFP |= SYS_GPC_MFP_PC0_SPI0_SS0 | SYS_GPC_MFP_PC1_SPI0_CLK | SYS_GPC_MFP_PC2_SPI0_MISO0 | SYS_GPC_MFP_PC3_SPI0_MOSI0;
    SYS->ALT_MFP |= SYS_ALT_MFP_PC0_SPI0_SS0 | SYS_ALT_MFP_PC1_SPI0_CLK | SYS_ALT_MFP_PC2_SPI0_MISO0 | SYS_ALT_MFP_PC3_SPI0_MOSI0;

    /* Setup SPI1 multi-function pins */
    SYS->GPC_MFP |= SYS_GPC_MFP_PC8_SPI1_SS0 | SYS_GPC_MFP_PC9_SPI1_CLK | SYS_GPC_MFP_PC10_SPI1_MISO0 | SYS_GPC_MFP_PC11_SPI1_MOSI0;
    SYS->ALT_MFP |= SYS_ALT_MFP_PC8_SPI1_SS0 | SYS_ALT_MFP_PC9_SPI1_CLK | SYS_ALT_MFP_PC10_SPI1_MISO0 | SYS_ALT_MFP_PC11_SPI1_MOSI0;
	
	SYS->GPB_MFP |= SYS_GPB_MFP_PB9_SPI1_SS1;
	SYS->GPC_MFP |= SYS_GPC_MFP_PC12_SPI1_MISO1 | SYS_GPC_MFP_PC13_SPI1_MOSI1;
    SYS->ALT_MFP |= SYS_ALT_MFP_PB9_SPI1_SS1;
    SYS->ALT_MFP |= SYS_ALT_MFP_PC12_SPI1_MISO1 | SYS_ALT_MFP_PC13_SPI1_MOSI1;

	//int0
//    SYS->GPB_MFP |= SYS_GPB_MFP_PB14_INT0;

    /* Update System Core Clock */
    /* User can use SystemCoreClockUpdate() to calculate SystemCoreClock and cyclesPerUs automatically. */
    SystemCoreClockUpdate();

	//setup gpio pins for the fds
    GPIO_SetMode(PD, BIT5, GPIO_PMD_INPUT);
    GPIO_SetMode(PD, BIT4, GPIO_PMD_INPUT);
    GPIO_SetMode(PD, BIT3, GPIO_PMD_OUTPUT);
    GPIO_SetMode(PD, BIT2, GPIO_PMD_OUTPUT);
    GPIO_SetMode(PD, BIT1, GPIO_PMD_OUTPUT);
    GPIO_SetMode(PD, BIT0, GPIO_PMD_OUTPUT);

    GPIO_SetMode(PA, BIT11, GPIO_PMD_OUTPUT);
    GPIO_SetMode(PA, BIT12,GPIO_PMD_INPUT);
//    GPIO_SetMode(PA, BIT13,GPIO_PMD_INPUT);

    /* Configure PB.14 as EINT0 pin and enable interrupt by rising edge trigger */
    GPIO_SetMode(PB, BIT14, GPIO_PMD_INPUT);
    GPIO_EnableEINT0(PB, 14, GPIO_INT_RISING);

    /* Enable interrupt de-bounce function and select de-bounce sampling cycle time is 1024 clocks of LIRC clock */
	GPIO_SET_DEBOUNCE_TIME(GPIO_DBCLKSRC_HCLK, GPIO_DBCLKSEL_4);
	GPIO_ENABLE_DEBOUNCE(PB, BIT14);
}

void UART0_Init()
{
    /*---------------------------------------------------------------------------------------------------------*/
    /* Init UART                                                                                               */
    /*---------------------------------------------------------------------------------------------------------*/
    /* Reset UART0 module */
    SYS_ResetModule(UART0_RST);

    /* Init UART0 to 115200-8n1 for print message */
    UART_Open(UART0, 115200);
}

void SPI_Init(void)
{
    /*---------------------------------------------------------------------------------------------------------*/
    /* Init SPI                                                                                                */
    /*---------------------------------------------------------------------------------------------------------*/
    /* Configure as a master, clock idle low, 32-bit transaction, drive output on falling clock edge and latch input on rising edge. */
    /* Set IP clock divider. SPI clock rate = 2MHz */
    SPI_Open(SPI0, SPI_MASTER, SPI_MODE_0, 8, 30000000);
    SPI_Open(SPI1, SPI_MASTER, SPI_MODE_0, 8, 8000000);

    /* Enable the automatic hardware slave select function. Select the SS pin and configure as low-active. */
//    SPI_EnableAutoSS(SPI0, SPI_SS0, SPI_SS_ACTIVE_LOW);
}

static flash_header_t header2;

static void print_block_info(int block)
{
	flash_read_disk_header(block,&header2);
	if(header2.name[0] == 0xFF) {
		printf("block %X: empty\r\n",block);
	}
	else {
		printf("block %X: id = %02d, '%s'\r\n",block,header2.id,header2.name);
	}
}

void hexdump(char *desc, void *addr, int len)
{
	int i;
	unsigned char buff[17];
	unsigned char *pc = (unsigned char *)addr;

	// Output description if given.
	if (desc != NULL)
	printf("%s:\r\n", desc);

	// Process every byte in the data.
	for (i = 0; i < len; i++) {
		// Multiple of 16 means new line (with line offset).

		if ((i % 16) == 0) {
			// Just don't print ASCII for the zeroth line.
			if (i != 0)
			printf("  %s\r\n", buff);

			// Output the offset.
			printf("  %04x ", i);
		}
		// Now the hex code for the specific character.
		printf(" %02x", pc[i]);

		// And store a printable ASCII character for later.
		if ((pc[i] < 0x20) || (pc[i] > 0x7e))
		buff[i % 16] = '.';
		else
		buff[i % 16] = pc[i];
		buff[(i % 16) + 1] = '\0';
	}

	// Pad out last line if not exactly 16 characters.
	while ((i % 16) != 0) {
		printf("   ");
		i++;
	}

	// And print the final ASCII bit.
	printf("  %s\r\n", buff);
}

int read_char(int *ch)
{
	if((DEBUG_PORT->FSR & UART_FSR_RX_EMPTY_Msk) == 0 ) {
		*ch = (int)(uint8_t)DEBUG_PORT->DATA;
		return (0);
	}
	return(-1);
}

static void console_tick(void)
{
	int ch = 0;

//	if(usart_read_char(USART_DEBUG,&ch) == USART_SUCCESS) {
//	if(kbhit() && (ch = GetChar())) {
	if(read_char(&ch) == 0) {
		int n;
		char help[] =
		"help:\r\n"
		"  0-F : select block to read disk data from\r\n"
		"  i   : insert disk\r\n"
		"  r   : remove disk\r\n"
		"  f   : flip disk to next side/disk\r\n"
		"  p   : print disks stored in flash\r\n"
		"  t   : test the system\r\n"
		"\r\n";

		switch((char)ch) {
		case '?':
			printf("%s",help);
			printf("currently selected disk in block is %d.\r\n\r\n",diskblock);
			break;
		case '0':
		case '1':
		case '2':
		case '3':
		case '4':
		case '5':
		case '6':
		case '7':
		case '8':
		case '9':
		case 'A':
		case 'B':
		case 'C':
		case 'D':
		case 'E':
		case 'F':
			if(ch >= 'A' && ch <= 'F') {
				diskblock = 10 + (ch - 'A');
			}
			else {
				diskblock = ch - '0';
			}
			printf("selected disk in block %X.\r\n",diskblock);
			break;
		case 'i':
			fds_insert_disk(diskblock);
//			hexdump("disk + 3500",SDRAM + 3500,256);
			break;
		case 'r':
			fds_remove_disk();
			break;
		case 'p':
			for(n=0;n<0x10;n++) {
				print_block_info(n);
			}
			break;
		case 't':
			flash_init();
			sram_init();
//			memcheck();
			break;
		case 'd':
 			break;
		case 'c':
			flash_copy_block(0xB,0x9);
			break;
		}
	}
}

int main()
{
    /* Unlock protected registers */
    SYS_UnlockReg();

    SYS_Init();

    /* Lock protected registers */
    SYS_LockReg();

    UART0_Init();
	SPI_Init();

	TIMER_Open(TIMER0, TIMER_CONTINUOUS_MODE, 6000000);
	TIMER_Open(TIMER1, TIMER_PERIODIC_MODE, 96400 * 2);
	TIMER_EnableInt(TIMER1);

	/* Open USB controller */
    USBD_Open(&gsInfo, HID_ClassRequest, NULL);

    /*Init Endpoint configuration for HID */
    HID_Init();

    /* Start USB device */
    USBD_Start();

    /* Enable USB device interrupt */
    NVIC_EnableIRQ(USBD_IRQn);

    printf("\n\nnuc123-fdsemu started.\n");
    printf("--CPU @ %d MHz\n", SystemCoreClock / 1000000);
    printf("--SPI0 @ %d MHz\n", SPI_GetBusClock(SPI0) / 1000000);
    printf("--SPI1 @ %d MHz\n", SPI_GetBusClock(SPI1) / 1000000);

	flash_init();
	sram_init();

	print_block_info(0);
	while(1) {
		console_tick();
		fds_tick();
	}
}

/*** (C) COPYRIGHT 2014~2015 Nuvoton Technology Corp. ***/
