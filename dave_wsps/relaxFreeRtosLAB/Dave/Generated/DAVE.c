
/**
 * @cond
 ***********************************************************************************************************************
 *
 * Copyright (c) 2015, Infineon Technologies AG
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without modification,are permitted provided that the
 * following conditions are met:
 *
 *   Redistributions of source code must retain the above copyright notice, this list of conditions and the  following
 *   disclaimer.
 *
 *   Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the
 *   following disclaimer in the documentation and/or other materials provided with the distribution.
 *
 *   Neither the name of the copyright holders nor the names of its contributors may be used to endorse or promote
 *   products derived from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES,
 * INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE  FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 * SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
 * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
 * WHETHER IN CONTRACT, STRICT LIABILITY,OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT  OF THE
 * USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 * To improve the quality of the software, users are encouraged to share modifications, enhancements or bug fixes
 * with Infineon Technologies AG (dave@infineon.com).
 ***********************************************************************************************************************
 *
 * Change History
 * --------------
 *
 * 2014-06-16:
 *     - Initial version<br>
 * 2015-08-28:
 *     - Added CLOCK_XMC1_Init conditionally
 *
 * @endcond
 *
 */

/***********************************************************************************************************************
 * HEADER FILES
 **********************************************************************************************************************/
#include "DAVE.h"

/***********************************************************************************************************************
 * API IMPLEMENTATION
 **********************************************************************************************************************/

/*******************************************************************************
 * @brief This function initializes the APPs Init Functions.
 *
 * @param[in]  None
 *
 * @return  DAVE_STATUS_t <BR>
 ******************************************************************************/
DAVE_STATUS_t DAVE_Init(void)
{
  DAVE_STATUS_t init_status;
  
  init_status = DAVE_STATUS_SUCCESS;
     /** @Initialization of APPs Init Functions */
     init_status = (DAVE_STATUS_t)CLOCK_XMC4_Init(&CLOCK_XMC4_0);

  if (init_status == DAVE_STATUS_SUCCESS)
  {
	 /**  Initialization of SPI_MASTER APP instance SPI4 */
	 init_status = (DAVE_STATUS_t)SPI_MASTER_Init(&SPI4); 
   } 
  if (init_status == DAVE_STATUS_SUCCESS)
  {
	 /**  Initialization of DIGITAL_IO APP instance PLL_CS */
	 init_status = (DAVE_STATUS_t)DIGITAL_IO_Init(&PLL_CS); 
   } 
  if (init_status == DAVE_STATUS_SUCCESS)
  {
	 /**  Initialization of DIGITAL_IO APP instance PLL_RST */
	 init_status = (DAVE_STATUS_t)DIGITAL_IO_Init(&PLL_RST); 
   } 
  if (init_status == DAVE_STATUS_SUCCESS)
  {
	 /**  Initialization of DIGITAL_IO APP instance DAC_CS */
	 init_status = (DAVE_STATUS_t)DIGITAL_IO_Init(&DAC_CS); 
   } 
  if (init_status == DAVE_STATUS_SUCCESS)
  {
	 /**  Initialization of DIGITAL_IO APP instance FLASH_CS */
	 init_status = (DAVE_STATUS_t)DIGITAL_IO_Init(&FLASH_CS); 
   } 
  if (init_status == DAVE_STATUS_SUCCESS)
  {
	 /**  Initialization of DIGITAL_IO APP instance GPS_RSTn */
	 init_status = (DAVE_STATUS_t)DIGITAL_IO_Init(&GPS_RSTn); 
   } 
  if (init_status == DAVE_STATUS_SUCCESS)
  {
	 /**  Initialization of DIGITAL_IO APP instance GPS_SAFEBOOTn */
	 init_status = (DAVE_STATUS_t)DIGITAL_IO_Init(&GPS_SAFEBOOTn); 
   } 
  if (init_status == DAVE_STATUS_SUCCESS)
  {
	 /**  Initialization of DIGITAL_IO APP instance GPS_1PPS */
	 init_status = (DAVE_STATUS_t)DIGITAL_IO_Init(&GPS_1PPS); 
   } 
  if (init_status == DAVE_STATUS_SUCCESS)
  {
	 /**  Initialization of DIGITAL_IO APP instance PLL_IRQ */
	 init_status = (DAVE_STATUS_t)DIGITAL_IO_Init(&PLL_IRQ); 
   } 
  if (init_status == DAVE_STATUS_SUCCESS)
  {
	 /**  Initialization of DIGITAL_IO APP instance GPS_IRQ */
	 init_status = (DAVE_STATUS_t)DIGITAL_IO_Init(&GPS_IRQ); 
   } 
  if (init_status == DAVE_STATUS_SUCCESS)
  {
	 /**  Initialization of DIGITAL_IO APP instance DAC_RSTn */
	 init_status = (DAVE_STATUS_t)DIGITAL_IO_Init(&DAC_RSTn); 
   } 
  if (init_status == DAVE_STATUS_SUCCESS)
  {
	 /**  Initialization of SPI_MASTER APP instance SPI1 */
	 init_status = (DAVE_STATUS_t)SPI_MASTER_Init(&SPI1); 
   } 
  if (init_status == DAVE_STATUS_SUCCESS)
  {
	 /**  Initialization of SPI_MASTER APP instance SPI2 */
	 init_status = (DAVE_STATUS_t)SPI_MASTER_Init(&SPI2); 
   } 
  if (init_status == DAVE_STATUS_SUCCESS)
  {
	 /**  Initialization of SPI_MASTER APP instance SPI3 */
	 init_status = (DAVE_STATUS_t)SPI_MASTER_Init(&SPI3); 
   } 
  if (init_status == DAVE_STATUS_SUCCESS)
  {
	 /**  Initialization of DIGITAL_IO APP instance ADC1_CS */
	 init_status = (DAVE_STATUS_t)DIGITAL_IO_Init(&ADC1_CS); 
   } 
  if (init_status == DAVE_STATUS_SUCCESS)
  {
	 /**  Initialization of DIGITAL_IO APP instance ADC2_CS */
	 init_status = (DAVE_STATUS_t)DIGITAL_IO_Init(&ADC2_CS); 
   } 
  if (init_status == DAVE_STATUS_SUCCESS)
  {
	 /**  Initialization of DIGITAL_IO APP instance ADC3_CS */
	 init_status = (DAVE_STATUS_t)DIGITAL_IO_Init(&ADC3_CS); 
   } 
  if (init_status == DAVE_STATUS_SUCCESS)
  {
	 /**  Initialization of DIGITAL_IO APP instance ADC_SYNC */
	 init_status = (DAVE_STATUS_t)DIGITAL_IO_Init(&ADC_SYNC); 
   } 
  if (init_status == DAVE_STATUS_SUCCESS)
  {
	 /**  Initialization of PIN_INTERRUPT APP instance ADC_DRDY */
	 init_status = (DAVE_STATUS_t)PIN_INTERRUPT_Init(&ADC_DRDY); 
   } 
  if (init_status == DAVE_STATUS_SUCCESS)
  {
	 /**  Initialization of DIGITAL_IO APP instance LED_0 */
	 init_status = (DAVE_STATUS_t)DIGITAL_IO_Init(&LED_0); 
   } 
  if (init_status == DAVE_STATUS_SUCCESS)
  {
	 /**  Initialization of DIGITAL_IO APP instance LED_1 */
	 init_status = (DAVE_STATUS_t)DIGITAL_IO_Init(&LED_1); 
   } 
  if (init_status == DAVE_STATUS_SUCCESS)
  {
	 /**  Initialization of DIGITAL_IO APP instance PUSH_BTN */
	 init_status = (DAVE_STATUS_t)DIGITAL_IO_Init(&PUSH_BTN); 
   }  
  return init_status;
} /**  End of function DAVE_Init */

