/*
 * FreeRTOS+TCP Labs Build 160919 (C) 2016 Real Time Engineers ltd.
 * Authors include Hein Tibosch and Richard Barry
 *
 *******************************************************************************
 ***** NOTE ******* NOTE ******* NOTE ******* NOTE ******* NOTE ******* NOTE ***
 ***                                                                         ***
 ***                                                                         ***
 ***   FREERTOS+TCP IS STILL IN THE LAB (mainly because the FTP and HTTP     ***
 ***   demos have a dependency on FreeRTOS+FAT, which is only in the Labs    ***
 ***   download):                                                            ***
 ***                                                                         ***
 ***   FreeRTOS+TCP is functional and has been used in commercial products   ***
 ***   for some time.  Be aware however that we are still refining its       ***
 ***   design, the source code does not yet quite conform to the strict      ***
 ***   coding and style standards mandated by Real Time Engineers ltd., and  ***
 ***   the documentation and testing is not necessarily complete.            ***
 ***                                                                         ***
 ***   PLEASE REPORT EXPERIENCES USING THE SUPPORT RESOURCES FOUND ON THE    ***
 ***   URL: http://www.FreeRTOS.org/contact  Active early adopters may, at   ***
 ***   the sole discretion of Real Time Engineers Ltd., be offered versions  ***
 ***   under a license other than that described below.                      ***
 ***                                                                         ***
 ***                                                                         ***
 ***** NOTE ******* NOTE ******* NOTE ******* NOTE ******* NOTE ******* NOTE ***
 *******************************************************************************
 *
 * FreeRTOS+TCP can be used under two different free open source licenses.  The
 * license that applies is dependent on the processor on which FreeRTOS+TCP is
 * executed, as follows:
 *
 * If FreeRTOS+TCP is executed on one of the processors listed under the Special
 * License Arrangements heading of the FreeRTOS+TCP license information web
 * page, then it can be used under the terms of the FreeRTOS Open Source
 * License.  If FreeRTOS+TCP is used on any other processor, then it can be used
 * under the terms of the GNU General Public License V2.  Links to the relevant
 * licenses follow:
 *
 * The FreeRTOS+TCP License Information Page: http://www.FreeRTOS.org/tcp_license
 * The FreeRTOS Open Source License: http://www.FreeRTOS.org/license
 * The GNU General Public License Version 2: http://www.FreeRTOS.org/gpl-2.0.txt
 *
 * FreeRTOS+TCP is distributed in the hope that it will be useful.  You cannot
 * use FreeRTOS+TCP unless you agree that you use the software 'as is'.
 * FreeRTOS+TCP is provided WITHOUT ANY WARRANTY; without even the implied
 * warranties of NON-INFRINGEMENT, MERCHANTABILITY or FITNESS FOR A PARTICULAR
 * PURPOSE. Real Time Engineers Ltd. disclaims all conditions and terms, be they
 * implied, expressed, or statutory.
 *
 * 1 tab == 4 spaces!
 *
 * http://www.FreeRTOS.org
 * http://www.FreeRTOS.org/plus
 * http://www.FreeRTOS.org/labs
 *
 */

/* Standard includes. */
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

/* FreeRTOS includes. */
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "semphr.h"

#include <DAVE.h>

/* FreeRTOS+TCP includes. */
#include "FreeRTOS_IP.h"
#include "FreeRTOS_Sockets.h"
#include "FreeRTOS_IP_Private.h"
#include "NetworkBufferManagement.h"
#include "NetworkInterface.h"

#include "xmc_gpio.h"
#include "xmc_eth_mac.h"
#include "xmc_eth_phy.h"
#include <string.h>

/**
 * RDES1 Descriptor RX Packet Control
 */
#define ETH_MAC_DMA_RDES1_RBS2 (0x1FFF0000U) /**< Receive buffer 2 size */
#define ETH_MAC_DMA_RDES1_RER  (0x00008000U) /**< Receive end of ring */
#define ETH_MAC_DMA_RDES1_RCH  (0x00004000U) /**< Second address chained */
#define ETH_MAC_DMA_RDES1_RBS1 (0x00001FFFU) /**< Receive buffer 1 size */
#define ETH_MAC_MMC_INTERRUPT_MSK  (0x03ffffffU) /**< Bit mask to disable MMMC transmit and receive interrupts*/

/* Define those to better describe your network interface. */
#define IFNAME0 'e'
#define IFNAME1 'n'

#if defined(SEISMO_BOARD)
	#define ETH_LWIP_0_CRS_DV  		XMC_GPIO_PORT15, 9U
	#define ETH_LWIP_0_RXER  		XMC_GPIO_PORT0, 11U
	#define ETH_LWIP_0_RXD0  		XMC_GPIO_PORT14, 8U
	#define ETH_LWIP_0_RXD1  		XMC_GPIO_PORT14, 9U
	#define ETH_LWIP_0_RMII_CLK 	XMC_GPIO_PORT15, 8U
	#define ETH_LWIP_0_MDIO  		XMC_GPIO_PORT1, 11U
	#define ETH_LWIP_0_TXEN  		XMC_GPIO_PORT1, 12U
	#define ETH_LWIP_0_TXD0  		XMC_GPIO_PORT2, 8U
	#define ETH_LWIP_0_TXD1  		XMC_GPIO_PORT2, 9U
	#define ETH_LWIP_0_MDC  		XMC_GPIO_PORT1, 10U
	
	#define PHY_ADDR 1
#else
	#define RXD1     P2_3
	#define RXD0     P2_2
	#define RXER     P2_4
	#define CLK_RMII P15_8
	#define TX_EN    P2_5
	#define TXD1     P2_9
	#define TXD0     P2_8
	#define CRS_DV   P15_9
	#define MDIO     P2_0
	#define MDC      P2_7
	
	#define PHY_ADDR 0
#endif//SEISMO_BOARD


#define XMC_ETH_MAC_NUM_RX_BUF (4)
#define XMC_ETH_MAC_NUM_TX_BUF (4)

/* MAC ADDRESS*/
#define MAC_ADDR0   configMAC_ADDR0
#define MAC_ADDR1   configMAC_ADDR1
#define MAC_ADDR2   configMAC_ADDR2
#define MAC_ADDR3   configMAC_ADDR3
#define MAC_ADDR4   configMAC_ADDR4
#define MAC_ADDR5   configMAC_ADDR5
#define MAC_ADDR    ((uint64_t)MAC_ADDR0 | \
                     ((uint64_t)MAC_ADDR1 << 8) | \
                     ((uint64_t)MAC_ADDR2 << 16) | \
                     ((uint64_t)MAC_ADDR3 << 24) | \
                     ((uint64_t)MAC_ADDR4 << 32) | \
                     ((uint64_t)MAC_ADDR5 << 40))


__attribute__((aligned(4))) XMC_ETH_MAC_DMA_DESC_t rx_desc[XMC_ETH_MAC_NUM_RX_BUF] __attribute__((section ("ETH_RAM")));
__attribute__((aligned(4))) XMC_ETH_MAC_DMA_DESC_t tx_desc[XMC_ETH_MAC_NUM_TX_BUF] __attribute__((section ("ETH_RAM")));
#if( ipconfigZERO_COPY_RX_DRIVER == 0 )
__attribute__((aligned(4))) uint8_t rx_buf[XMC_ETH_MAC_NUM_RX_BUF][XMC_ETH_MAC_BUF_SIZE] __attribute__((section ("ETH_RAM")));
#endif
#if( ipconfigZERO_COPY_TX_DRIVER == 0 )
__attribute__((aligned(4))) uint8_t tx_buf[XMC_ETH_MAC_NUM_TX_BUF][XMC_ETH_MAC_BUF_SIZE] __attribute__((section ("ETH_RAM")));
#endif
XMC_ETH_PHY_CONFIG_t eth_phy_config =
{
  .interface = XMC_ETH_LINK_INTERFACE_RMII,
  .enable_auto_negotiate = true
};

XMC_ETH_MAC_t eth_mac =
{
  .regs = ETH0,
  .address = MAC_ADDR,
  .rx_desc = rx_desc,
  .tx_desc = tx_desc,
#if( ipconfigZERO_COPY_RX_DRIVER == 1)
  .rx_buf = NULL,
#else
  .rx_buf = &rx_buf[0][0],
#endif
#if( ipconfigZERO_COPY_TX_DRIVER == 0 )
  .tx_buf = &tx_buf[0][0],
#else
  .tx_buf = NULL,
#endif
  .num_rx_buf = XMC_ETH_MAC_NUM_RX_BUF,
  .num_tx_buf = XMC_ETH_MAC_NUM_TX_BUF
};


/* The size of each buffer when BufferAllocation_1 is used:
http://www.freertos.org/FreeRTOS-Plus/FreeRTOS_Plus_TCP/Embedded_Ethernet_Buffer_Management.html */
#define niBUFFER_1_PACKET_SIZE		1536

///* Naming and numbering of PHY registers. */
//#define PHY_REG_01_BMSR			0x01	/* Basic mode status register */
//
//#ifndef iptraceEMAC_TASK_STARTING
//	#define iptraceEMAC_TASK_STARTING()	do { } while( 0 )
//#endif
//
/* Default the size of the stack used by the EMAC deferred handler task to twice
the size of the stack used by the idle task - but allow this to be overridden in
FreeRTOSConfig.h as configMINIMAL_STACK_SIZE is a user definable constant. */
#ifndef configEMAC_TASK_STACK_SIZE
	#define configEMAC_TASK_STACK_SIZE ( 2 * configMINIMAL_STACK_SIZE )
#endif

/*-----------------------------------------------------------*/

///*
// * Look for the link to be up every few milliseconds until either xMaxTime time
// * has passed or a link is found.
// */
//static BaseType_t prvGMACWaitLS( TickType_t xMaxTime );

/*
 * A deferred interrupt handler for all MAC/DMA interrupt sources.
 */
static void prvEMACHandlerTask( void *pvParameters );


extern int phy_detected;

/* A copy of PHY register 1: 'PHY_REG_01_BMSR' */
static uint32_t ulPHYLinkStatus = 0;

#if( ipconfigUSE_LLMNR == 1 )
	static const uint8_t xLLMNR_MACAddress[] = { 0x01, 0x00, 0x5E, 0x00, 0x00, 0xFC };
#endif

/* ucMACAddress as it appears in main.c */
extern const uint8_t ucMACAddress[ 6 ];

/* Holds the handle of the task used as a deferred interrupt processor.  The
handle is used so direct notifications can be sent to the task for all EMAC/DMA
related interrupts. */

TaskHandle_t xEMACTaskHandle = NULL;
/*-----------------------------------------------------------*/

/**
 * In this function, the hardware should be initialized.
 * Called from ethernetif_init().
 *
 * @param netif the already initialized lwip network interface structure
 *        for this ethernetif
 */
BaseType_t xNetworkInterfaceInitialise( void )
{
	/* Guard against the init function being called more than once. */
	if( xEMACTaskHandle == NULL )
	{

#if defined(SEISMO_BOARD)
		{//=======	Init GPIO =========
			  XMC_ETH_MAC_PORT_CTRL_t port_control;
			  XMC_GPIO_CONFIG_t gpio_config;

			  gpio_config.output_level = XMC_GPIO_OUTPUT_LEVEL_LOW;
			  gpio_config.mode = XMC_GPIO_MODE_INPUT_TRISTATE;
			  XMC_GPIO_Init(ETH_LWIP_0_CRS_DV, &gpio_config);
			  XMC_GPIO_Init(ETH_LWIP_0_RXER, &gpio_config);
			  XMC_GPIO_Init(ETH_LWIP_0_RXD0, &gpio_config);
			  XMC_GPIO_Init(ETH_LWIP_0_RXD1, &gpio_config);
			  XMC_GPIO_Init(ETH_LWIP_0_RMII_CLK, &gpio_config);
			  XMC_GPIO_Init(ETH_LWIP_0_MDIO, &gpio_config);

			  gpio_config.output_strength = XMC_GPIO_OUTPUT_STRENGTH_STRONG_MEDIUM_EDGE;
			  gpio_config.mode = XMC_GPIO_MODE_OUTPUT_PUSH_PULL_ALT1;
			  XMC_GPIO_Init(ETH_LWIP_0_MDC, &gpio_config);
			  XMC_GPIO_SetHardwareControl(ETH_LWIP_0_MDIO, XMC_GPIO_HWCTRL_PERIPHERAL1);

			  gpio_config.output_strength = XMC_GPIO_OUTPUT_STRENGTH_STRONG_MEDIUM_EDGE;
			  gpio_config.mode = XMC_GPIO_MODE_OUTPUT_PUSH_PULL_ALT1;
		      XMC_GPIO_Init(ETH_LWIP_0_TXEN, &gpio_config);
		      gpio_config.mode = XMC_GPIO_MODE_OUTPUT_PUSH_PULL_ALT1;
		      XMC_GPIO_Init(ETH_LWIP_0_TXD0, &gpio_config);
		      gpio_config.mode = XMC_GPIO_MODE_OUTPUT_PUSH_PULL_ALT1;
		      XMC_GPIO_Init(ETH_LWIP_0_TXD1, &gpio_config);
			  
			  port_control.mode = XMC_ETH_MAC_PORT_CTRL_MODE_RMII;
			  port_control.rxd0 = (XMC_ETH_MAC_PORT_CTRL_RXD0_t)2U;
			  port_control.rxd1 = (XMC_ETH_MAC_PORT_CTRL_RXD1_t)2U;
			  port_control.clk_rmii = (XMC_ETH_MAC_PORT_CTRL_CLK_RMII_t)2U;
			  port_control.crs_dv = (XMC_ETH_MAC_PORT_CTRL_CRS_DV_t)2U;
			  port_control.rxer = (XMC_ETH_MAC_PORT_CTRL_RXER_t)1U;
			  port_control.mdio = (XMC_ETH_MAC_PORT_CTRL_MDIO_t)2U;
			  XMC_ETH_MAC_SetPortControl(&eth_mac, port_control);
		}//====	GPIO Initialaized	====================
#else
		{//=======	Init GPIO =========
			XMC_ETH_MAC_PORT_CTRL_t port_control;
			XMC_GPIO_CONFIG_t gpio_config;

			gpio_config.mode = XMC_GPIO_MODE_INPUT_TRISTATE;
			XMC_GPIO_Init(RXD0, &gpio_config);
			XMC_GPIO_Init(RXD1, &gpio_config);
			XMC_GPIO_Init(CLK_RMII, &gpio_config);
			XMC_GPIO_Init(CRS_DV, &gpio_config);
			XMC_GPIO_Init(RXER, &gpio_config);
			XMC_GPIO_Init(MDIO, &gpio_config);

			gpio_config.output_level = XMC_GPIO_OUTPUT_LEVEL_LOW;
			gpio_config.output_strength = XMC_GPIO_OUTPUT_STRENGTH_STRONG_SHARP_EDGE;
			gpio_config.mode = (XMC_GPIO_MODE_t)((uint32_t)XMC_GPIO_MODE_OUTPUT_PUSH_PULL |P2_8_AF_ETH0_TXD0);
			XMC_GPIO_Init(TXD0, &gpio_config);

			gpio_config.mode = (XMC_GPIO_MODE_t)((uint32_t)XMC_GPIO_MODE_OUTPUT_PUSH_PULL | P2_9_AF_ETH0_TXD1);
			XMC_GPIO_Init(TXD1, &gpio_config);

			gpio_config.mode = (XMC_GPIO_MODE_t)((uint32_t)XMC_GPIO_MODE_OUTPUT_PUSH_PULL | P2_5_AF_ETH0_TX_EN);
			XMC_GPIO_Init(TX_EN, &gpio_config);

			gpio_config.mode = (XMC_GPIO_MODE_t)((uint32_t)XMC_GPIO_MODE_OUTPUT_PUSH_PULL | P2_7_AF_ETH0_MDC);
			XMC_GPIO_Init(MDC, &gpio_config);

			XMC_GPIO_SetHardwareControl(MDIO, XMC_GPIO_HWCTRL_PERIPHERAL1);

			port_control.mode = XMC_ETH_MAC_PORT_CTRL_MODE_RMII;
			port_control.rxd0 = XMC_ETH_MAC_PORT_CTRL_RXD0_P2_2;
			port_control.rxd1 = XMC_ETH_MAC_PORT_CTRL_RXD1_P2_3;
			port_control.clk_rmii = XMC_ETH_MAC_PORT_CTRL_CLK_RMII_P15_8;
			port_control.crs_dv = XMC_ETH_MAC_PORT_CTRL_CRS_DV_P15_9;
			port_control.rxer = XMC_ETH_MAC_PORT_CTRL_RXER_P5_3;
			port_control.mdio = XMC_ETH_MAC_PORT_CTRL_MDIO_P2_0;
			XMC_ETH_MAC_SetPortControl(&eth_mac, port_control);
		}//====	GPIO Initialaized	====================
#endif
		XMC_ETH_MAC_Init(&eth_mac);

		//Init DMA RX desc
		{
		  uint32_t i;
		  uint32_t next;
		  size_t xSize = ipTOTAL_ETHERNET_FRAME_SIZE;

		  XMC_ASSERT("XMC_ETH_MAC_InitRxDescriptors: eth_mac is invalid", XMC_ETH_MAC_IsValidModule(eth_mac.regs));

		  /*
		   * Chained structure (ETH_MAC_DMA_RDES1_RCH), second address in the descriptor
		   * (buffer2) is the next descriptor address
		   */
		  for (i = 0U; i < eth_mac.num_rx_buf; ++i)
		  {
			  eth_mac.rx_desc[i].status = (uint32_t)ETH_MAC_DMA_RDES0_OWN;
			  eth_mac.rx_desc[i].length = (uint32_t)ETH_MAC_DMA_RDES1_RCH | (uint32_t)XMC_ETH_MAC_BUF_SIZE;
#if( ipconfigZERO_COPY_RX_DRIVER == 1)
			  xSize = ipTOTAL_ETHERNET_FRAME_SIZE;
			  eth_mac.rx_desc[i].buffer1 = (uint32_t*)pucGetNetworkBuffer( &xSize );
			  if(xSize < ipTOTAL_ETHERNET_FRAME_SIZE)
			  {
				  return pdFALSE;
			  }
#else
			  eth_mac.rx_desc[i].buffer1 = (uint32_t)&(eth_mac.rx_buf[i * XMC_ETH_MAC_BUF_SIZE]);
#endif
		    next = i + 1U;
		    if (next == eth_mac.num_rx_buf)
		    {
		      next = 0U;
		    }
		    eth_mac.rx_desc[i].buffer2 = (uint32_t)&(eth_mac.rx_desc[next]);
		  }
		  eth_mac.regs->RECEIVE_DESCRIPTOR_LIST_ADDRESS = (uint32_t)&(eth_mac.rx_desc[0]);
		  eth_mac.rx_index = 0U;
		}
		//Init DMA TX desc
		{
		  uint32_t i;
		  uint32_t next;

		  XMC_ASSERT("XMC_ETH_MAC_InitTxDescriptors: eth_mac is invalid", XMC_ETH_MAC_IsValidModule(eth_mac.regs));

		  /* Chained structure (ETH_MAC_DMA_TDES0_TCH), second address in the descriptor (buffer2) is the next descriptor address */
		  for (i = 0U; i < eth_mac.num_tx_buf; ++i)
		  {
			  eth_mac.tx_desc[i].status = ETH_MAC_DMA_TDES0_TCH | ETH_MAC_DMA_TDES0_LS | ETH_MAC_DMA_TDES0_FS;
#if( ipconfigZERO_COPY_TX_DRIVER == 0 )
			  eth_mac.tx_desc[i].buffer1 = (uint32_t)&(eth_mac.tx_buf[i * XMC_ETH_MAC_BUF_SIZE]);
#else
			  eth_mac.tx_desc[i].buffer1 = NULL;
#endif
		    next = i + 1U;
		    if (next == eth_mac.num_tx_buf)
		    {
		        next = 0U;
		    }
		    eth_mac.tx_desc[i].buffer2 = (uint32_t)&(eth_mac.tx_desc[next]);
		  }
		  eth_mac.regs->TRANSMIT_DESCRIPTOR_LIST_ADDRESS = (uint32_t)&(eth_mac.tx_desc[0]);
		  eth_mac.tx_index = 0U;
		}

		XMC_ETH_MAC_DisableJumboFrame(&eth_mac);

		XMC_ETH_PHY_Init(&eth_mac, PHY_ADDR, &eth_phy_config);

		while(XMC_ETH_PHY_GetLinkStatus(&eth_mac, PHY_ADDR) != XMC_ETH_LINK_STATUS_UP);

		XMC_ETH_LINK_SPEED_t speed = XMC_ETH_PHY_GetLinkSpeed(&eth_mac, PHY_ADDR);
		XMC_ETH_LINK_DUPLEX_t duplex = XMC_ETH_PHY_GetLinkDuplex(&eth_mac, PHY_ADDR);

		XMC_ETH_MAC_SetLink(&eth_mac, speed, duplex);

		/* Enable ethernet interrupts */
		XMC_ETH_MAC_EnableEvent(&eth_mac, XMC_ETH_MAC_EVENT_RECEIVE);

		NVIC_SetPriority(ETH0_0_IRQn, NVIC_EncodePriority(NVIC_GetPriorityGrouping(), 63, 0));
		NVIC_ClearPendingIRQ(ETH0_0_IRQn);
		NVIC_EnableIRQ(ETH0_0_IRQn);

		XMC_ETH_MAC_EnableTx(&eth_mac);
		XMC_ETH_MAC_EnableRx(&eth_mac);
		/* The deferred interrupt handler task is created at the highest
		possible priority to ensure the interrupt handler can return directly
		to it.  The task's handle is stored in xEMACTaskHandle so interrupts can
		notify the task when there is something to process. */
		xTaskCreate( prvEMACHandlerTask, "EMAC", configEMAC_TASK_STACK_SIZE, NULL, configMAX_PRIORITIES - 1, &xEMACTaskHandle );
	}
	else
	{
		/* Initialisation was already performed, just wait for the link. */
//		prvGMACWaitLS( xWaitRelinkDelay );
	}
//
//	/* Only return pdTRUE when the Link Status of the PHY is high, otherwise the
//	DHCP process and all other communication will fail. */
//	xLinkStatus = xGetPhyLinkStatus();
//
//	return ( xLinkStatus != pdFALSE );
	return pdTRUE;//TO DO!!!
}

/*-----------------------------------------------------------*/

//void vNetworkInterfaceAllocateRAMToBuffers( NetworkBufferDescriptor_t pxNetworkBuffers[ ipconfigNUM_NETWORK_BUFFER_DESCRIPTORS ] )
//{
//__attribute__((aligned(4))) static uint8_t ucNetworkPackets[ ipconfigNUM_NETWORK_BUFFER_DESCRIPTORS * niBUFFER_1_PACKET_SIZE ] __attribute__((section ("ETH_RAM")));
//uint8_t *ucRAMBuffer = ucNetworkPackets;
//uint32_t ul;
//
//	for( ul = 0; ul < ipconfigNUM_NETWORK_BUFFER_DESCRIPTORS; ul++ )
//	{
//		pxNetworkBuffers[ ul ].pucEthernetBuffer = ucRAMBuffer + ipBUFFER_PADDING;
//		*( ( unsigned * ) ucRAMBuffer ) = ( unsigned ) ( &( pxNetworkBuffers[ ul ] ) );
//		ucRAMBuffer += niBUFFER_1_PACKET_SIZE;
//	}
//}

/*-----------------------------------------------------------*/
/**
 * This function should do the actual transmission of the packet. The packet is
 * contained in the pbuf that is passed to the function. This pbuf
 * might be chained.
 *
 * @param pxBuffer takes a pointer to the start of the data to be sent and the length of the
 *   		data to be sent as two separate parameters.  The start of the data is located
 *   		by pxBuffer->pucEthernetBuffer.  The length of the data is located
 *   		by pxBuffer->xDataLength.
 * @param bReleaseAfterSend pdTRUE if The Ethernet buffer is therefore no longer needed, and must be
        freed for re-use
 * @return pdTRUE if the packet could be sent
 *         an pdFALSE value if the packet couldn't be sent
 *
 * @note Returning ERR_MEM here if a DMA queue of your MAC is full can lead to
 *       strange results. You might consider waiting for space in the DMA queue
 *       to become availale since the stack doesn't retry to send a packet
 *       dropped because of memory failure (except for the TCP timers).
 */
BaseType_t xNetworkInterfaceOutput( NetworkBufferDescriptor_t * const pxBuffer, BaseType_t bReleaseAfterSend )
{
	uint32_t ulTransmitSize = 0;
	BaseType_t xReturn = pdTRUE;
	uint8_t *pbuffer;
	static uint32_t entry_counter=0;

	if(entry_counter++ >0)
	{
//		DIGITAL_IO_ToggleOutput(&LED_1);
	}

#if( ipconfigDRIVER_INCLUDED_TX_IP_CHECKSUM != 0 )
	{
		ProtocolPacket_t *pxPacket;

		/* If the peripheral must calculate the checksum, it wants
	the protocol checksum to have a value of zero. */
		pxPacket = ( ProtocolPacket_t * ) ( pxBuffer->pucEthernetBuffer );

		if( pxPacket->xICMPPacket.xIPHeader.ucProtocol == ipPROTOCOL_ICMP )
		{
			pxPacket->xICMPPacket.xICMPHeader.usChecksum = ( uint16_t )0u;
		}
	}
#endif

//	if( ( ulPHYLinkStatus & BMSR_LINK_STATUS ) != 0 )	//TODO !!!
	{
		/* This function does the actual transmission of the packet. The packet is
			contained in 'pxDescriptor' that is passed to the function. */

//		pxDmaTxDesc = xETH.TxDesc;

		/* Is this buffer available? */
		if( XMC_ETH_MAC_IsTxDescriptorOwnedByDma(&eth_mac) )
		{
			XMC_ETH_MAC_ResumeTx(&eth_mac);
			xReturn = pdFAIL;
		}
		else
		{
			pbuffer = XMC_ETH_MAC_GetTxBuffer(&eth_mac);
#if( ipconfigZERO_COPY_TX_DRIVER == 1 )
			if(pbuffer != NULL)
			{
				vReleaseNetworkBuffer( pbuffer );
			}
			(eth_mac.tx_desc[eth_mac.tx_index].buffer1) = pxBuffer->pucEthernetBuffer;
#endif
			/* Get bytes in current buffer. */
			ulTransmitSize = pxBuffer->xDataLength;

			if( ulTransmitSize > XMC_ETH_MAC_BUF_SIZE )
			{
				ulTransmitSize = XMC_ETH_MAC_BUF_SIZE;
			}
#if( ipconfigZERO_COPY_TX_DRIVER == 0 )
			/* Copy the remaining bytes */
			memcpy( ( void * ) pbuffer, pxBuffer->pucEthernetBuffer, ulTransmitSize );
#endif

			/* Prepare transmit descriptors to give to DMA. */
			XMC_ETH_MAC_SetTxBufferSize(&eth_mac, ulTransmitSize);

			XMC_ETH_MAC_ReturnTxDescriptor(&eth_mac);

			XMC_ETH_MAC_ResumeTx(&eth_mac);

			xReturn = pdPASS;
		}
	}
//	else
//	{
//		/* The PHY has no Link Status, packet shall be dropped. */
//		xReturn = pdFAIL;
//	}





//	if (pxBuffer->xDataLength > XMC_ETH_MAC_BUF_SIZE) {
//		xReturn = pdFALSE;
//	}
//	else
//	{
//		memcpy(&buffer[framelen], pxBuffer->pucEthernetBuffer, pxBuffer->xDataLength);
//		ulTransmitSize += pxBuffer->xDataLength;
//
//		status = XMC_ETH_MAC_SendFrame(&eth_mac, buffer, ulTransmitSize, 0);
//		if (status != XMC_ETH_MAC_STATUS_OK)
//		{
//			xReturn = pdFALSE;
//		}
//	}


#if( ipconfigZERO_COPY_TX_DRIVER == 0 )
	{
		/* The buffer has been sent so can be released. */
		if( bReleaseAfterSend != pdFALSE )
		{
			vReleaseNetworkBufferAndDescriptor( pxBuffer );
		}
	}
#else
    if( bReleaseAfterSend != pdFALSE )
    {
    	pxBuffer->pucEthernetBuffer = NULL;
        vReleaseNetworkBufferAndDescriptor( pxBuffer );
    }
#endif

    entry_counter--;

	return xReturn;
}
/*-----------------------------------------------------------*/

BaseType_t xGetPhyLinkStatus( void )
{
	return XMC_ETH_PHY_GetLinkStatus(&eth_mac, PHY_ADDR);
//	BaseType_t xReturn;
//
//	if( ( ulPHYLinkStatus & BMSR_LINK_STATUS ) == 0 )
//	{
//		xReturn = pdFALSE;
//	}
//	else
//	{
//		xReturn = pdTRUE;
//	}
//
//	return xReturn;
}
/*-----------------------------------------------------------*/

#if (ipconfigZERO_COPY_RX_DRIVER == 1)
static void prvEMACHandlerTask( void *pvParameters )
{
	NetworkBufferDescriptor_t *pxBufferDescriptor;
	size_t xBytesReceived;
	/* Used to indicate that xSendEventStructToIPTask() is being called because
	of an Ethernet receive event. */
	IPStackEvent_t xRxEvent;
	uint8_t *pucTemp;

	for( ;; )
	{
		/* Wait for the Ethernet MAC interrupt to indicate that another packet
	        has been received.  The task notification is used in a similar way to a
	        counting semaphore to count Rx events, but is a lot more efficient than
	        a semaphore. */
		ulTaskNotifyTake( pdFALSE, portMAX_DELAY );

		pxBufferDescriptor = pxGetNetworkBufferWithDescriptor( ipTOTAL_ETHERNET_FRAME_SIZE, 0 );
		pucTemp = pxBufferDescriptor->pucEthernetBuffer;

		pxBufferDescriptor->pucEthernetBuffer = XMC_ETH_MAC_GetRxBuffer(&eth_mac);
		pxBufferDescriptor->xDataLength = XMC_ETH_MAC_GetRxFrameSize(&eth_mac);

		eth_mac.rx_desc[eth_mac.rx_index].buffer1 = pucTemp;

		XMC_ETH_MAC_ReturnRxDescriptor(&eth_mac);

		if (eth_mac.regs->STATUS & ETH_STATUS_RU_Msk)
		{
			/* Receive buffer unavailable, resume DMA */
			eth_mac.regs->STATUS = (uint32_t)ETH_STATUS_RU_Msk;
			eth_mac.regs->RECEIVE_POLL_DEMAND = 0U;
		}

//		/* A pointer to the descriptor is stored at the front of the buffer, so
//		    swap these too. */
//		*( ( NetworkBufferDescriptor_t ** )
//				( pxDescriptor->pucEthernetBuffer - ipBUFFER_PADDING ) ) = pxDescriptor;
//
//		*( ( NetworkBufferDescriptor_t ** )
//				( eth_mac.rx_desc[eth_mac.rx_index]->buffer1 - ipBUFFER_PADDING ) ) = pxDMARxDescriptor;

		if( eConsiderFrameForProcessing( pxBufferDescriptor->pucEthernetBuffer )
				== eProcessBuffer )
		{
			/* The event about to be sent to the TCP/IP is an Rx event. */
			xRxEvent.eEventType = eNetworkRxEvent;

			/* pvData is used to point to the network buffer descriptor that
			                    now references the received data. */
			xRxEvent.pvData = ( void * ) pxBufferDescriptor;

			/* Send the data to the TCP/IP stack. */
			if( xSendEventStructToIPTask( &xRxEvent, 0 ) == pdFALSE )
			{
				/* The buffer could not be sent to the IP task so the buffer
			                        must be released. */
				vReleaseNetworkBufferAndDescriptor( pxBufferDescriptor );

				/* Make a call to the standard trace macro to log the
			                        occurrence. */
				iptraceETHERNET_RX_EVENT_LOST();
			}
			else
			{
				/* The message was successfully sent to the TCP/IP stack.
			                        Call the standard trace macro to log the occurrence. */
				iptraceNETWORK_INTERFACE_RECEIVE();
			}
		}
		else
		{
			/* The Ethernet frame can be dropped, but the Ethernet buffer
			                    must be released. */
			vReleaseNetworkBufferAndDescriptor( pxBufferDescriptor );
		}

	}
}
#else
static void prvEMACHandlerTask( void *pvParameters )
{
	NetworkBufferDescriptor_t *pxBufferDescriptor;
	size_t xBytesReceived;
	/* Used to indicate that xSendEventStructToIPTask() is being called because
	of an Ethernet receive event. */
	IPStackEvent_t xRxEvent;

	for( ;; )
	{
		/* Wait for the Ethernet MAC interrupt to indicate that another packet
	        has been received.  The task notification is used in a similar way to a
	        counting semaphore to count Rx events, but is a lot more efficient than
	        a semaphore. */
		ulTaskNotifyTake( pdFALSE, portMAX_DELAY );

		/* See how much data was received.  Here it is assumed ReceiveSize() is
	        a peripheral driver function that returns the number of bytes in the
	        received Ethernet frame. */
		xBytesReceived = XMC_ETH_MAC_GetRxFrameSize(&eth_mac);

		if( xBytesReceived > 0 )
		{
			/* Allocate a network buffer descriptor that points to a buffer
	            large enough to hold the received frame.  As this is the simple
	            rather than efficient example the received data will just be copied
	            into this buffer. */
			pxBufferDescriptor = pxGetNetworkBufferWithDescriptor( xBytesReceived, 0 );

			if( pxBufferDescriptor != NULL )
			{
				/* pxBufferDescriptor->pucEthernetBuffer now points to an Ethernet
	                buffer large enough to hold the received data.  Copy the
	                received data into pcNetworkBuffer->pucEthernetBuffer.  Here it
	                is assumed ReceiveData() is a peripheral driver function that
	                copies the received data into a buffer passed in as the function's
	                parameter.  Remember! While is is a simple robust technique -
	                it is not efficient.  An example that uses a zero copy technique
	                is provided further down this page. */
				pxBufferDescriptor->xDataLength = XMC_ETH_MAC_ReadFrame(&eth_mac, pxBufferDescriptor->pucEthernetBuffer, xBytesReceived);
//				pxBufferDescriptor->xDataLength = xBytesReceived;


				/* See if the data contained in the received Ethernet frame needs
	                to be processed.  NOTE! It is preferable to do this in
	                the interrupt service routine itself, which would remove the need
	                to unblock this task for packets that don't need processing. */
				if( eConsiderFrameForProcessing( pxBufferDescriptor->pucEthernetBuffer )
						== eProcessBuffer )
				{
					/* The event about to be sent to the TCP/IP is an Rx event. */
					xRxEvent.eEventType = eNetworkRxEvent;

					/* pvData is used to point to the network buffer descriptor that
	                    now references the received data. */
					xRxEvent.pvData = ( void * ) pxBufferDescriptor;

					/* Send the data to the TCP/IP stack. */
					if( xSendEventStructToIPTask( &xRxEvent, 0 ) == pdFALSE )
					{
						/* The buffer could not be sent to the IP task so the buffer
	                        must be released. */
						vReleaseNetworkBufferAndDescriptor( pxBufferDescriptor );

						/* Make a call to the standard trace macro to log the
	                        occurrence. */
						iptraceETHERNET_RX_EVENT_LOST();
					}
					else
					{
						/* The message was successfully sent to the TCP/IP stack.
	                        Call the standard trace macro to log the occurrence. */
						iptraceNETWORK_INTERFACE_RECEIVE();
					}
				}
				else
				{
					/* The Ethernet frame can be dropped, but the Ethernet buffer
	                    must be released. */
					vReleaseNetworkBufferAndDescriptor( pxBufferDescriptor );
				}
			}
			else
			{
				/* The event was lost because a network buffer was not available.
	                Call the standard trace macro to log the occurrence. */
				iptraceETHERNET_RX_EVENT_LOST();
			}
		}
	}
}
#endif
/*-----------------------------------------------------------*/

void ETH0_0_IRQHandler(void)
{
	uint32_t status;
	BaseType_t xHigherPriorityTaskWoken = pdFALSE;

	status = XMC_ETH_MAC_GetEventStatus(&eth_mac);
	if (status & XMC_ETH_MAC_EVENT_RECEIVE)
	{
		if( xEMACTaskHandle != NULL )
		{
			vTaskNotifyGiveFromISR( xEMACTaskHandle, &xHigherPriorityTaskWoken );
			portYIELD_FROM_ISR( xHigherPriorityTaskWoken );
		}
	}
	XMC_ETH_MAC_ClearEventStatus(&eth_mac, status);
}
