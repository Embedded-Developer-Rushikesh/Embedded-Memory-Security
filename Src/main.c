#include <stdio.h>
#include "stm32f4xx.h"
#include "fpu.h"
#include "uart.h"
#include "timebase.h"
#include "bsp.h"
#include "adc.h"

#define SHCSR_REG				(*(volatile uint32_t *)0xE000ED24)


#define MPU_CTRL_REG				(*(volatile uint32_t *)0xE000ED94)

#define MPU_RAS_REG				(*(volatile uint32_t *)0xE000EDA0)

#define MPU_RBASE_ADDR_REG				(*(volatile uint32_t *)0xE000ED9C)
#define MPU_TYPE_REG				    (*(volatile uint32_t *)0xE000ED90)
#define START_OF_DREGION			    8

#define MEMFAULTENA					(1U<<16)
uint8_t num_of_regions;
uint32_t min_region_sz;


#define SIZE_TO_CONFIG				32

/*The maximum stack we have defined
 * in the linkerscript*/
#define MAX_STACK					128

uint32_t factorial(uint32_t n)
{
	if(n == 0  || n == 1)
	{
		return 1;
	}
	else{
		return n *factorial(n -1);
	}
}

uint32_t fact_result;

int main()
{
	/*Enable FPU*/
	fpu_enable();

	/*Initialize debug UART*/
	debug_uart_init();

	/*Enable MemFault Handler*/
	SHCSR_REG |= MEMFAULTENA;

	/*Check number of regions*/

	num_of_regions =  MPU_TYPE_REG >> START_OF_DREGION;


	/*Check minimum region size*/
	/*Set bits 4 to 32 to 1 in the MPU_RBASE_ADDR_REG
	 * and then read the MPU_RBASE_ADDR_REG. If the bits remain set then the
	 * minimum possible region size of 32bytes is supported */

	MPU_RBASE_ADDR_REG =  0xFFFFFFE0;   // 0B  1111 1111......1110 0000
	min_region_sz =  MPU_RBASE_ADDR_REG;

	/*Compute final base addr of region to config*/
	const uint32_t base_addr =  ((uint32_t)0x20000000 + MAX_STACK );

	/*Config MPU_RBASE_ADDR_REG*/
	/*Set Addrss,  VALID bit, and Region number to 0*/
	MPU_RBASE_ADDR_REG =  (base_addr | 1 << 4 | 0);


	/*Config MPU_RAS_REG*/
	MPU_RAS_REG = (0b000 <<24) |(0b000110 << 16)|(4 << 1)| 0x1;


	/*Config MPU_CTRL_REG*/
	MPU_CTRL_REG =  (1U<<2) | (1U<<0);




	while(1)
	{

		fact_result = factorial(5);
	}
}


void HardFault_Handler(void)
{

	printf("HARDFAULT \n\r");
}

void MemManage_Handler(void)
{
	printf("MEMFAULT \n\r");

}
