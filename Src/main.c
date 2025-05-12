#include <stdio.h>
#include <string.h>
#include "stm32f4xx.h"
#include "fpu.h"
#include "uart.h"
#include "timebase.h"
#include "bsp.h"
#include "adc.h"
#include "flash_protection.h"



#define BUFF_SIZE		10

char buff_rcv_char[BUFF_SIZE + 30];
char buff_rcv_char2[BUFF_SIZE + 30];

uint32_t buff_data_to_store[BUFF_SIZE] = {995,1025,3056,78,2020,143,1200,3300,4000,45};
uint32_t buff_rcv_data[BUFF_SIZE +30];
uint32_t buff_rcv_data2[BUFF_SIZE +30];

char *pbuff_test_string =  "Test string from firmware";
char *pbuff_test_string2 =  "Protection_protection!!!!!";

uint32_t g_curr_val =  3650;
uint32_t g_rcv_val;

static uint32_t get_word_length(char *buff);

uint8_t flash_setting;

FLASH_OBProgramInitTypeDef all_obytes_setting;
int main()
{
	/*Enable FPU*/
	fpu_enable();

	/*Initialize debug UART*/
	debug_uart_init();

	/*Initialize timebase*/
	timebase_init();

	 /*IMPORTANT :  Wait 10ms*/
	  delay(500);

    /*Write data*/
    flash_write_data(0x08060000,(uint32_t *)buff_data_to_store, BUFF_SIZE);

   /*Read data*/
    flash_read_data(0x08060000,buff_rcv_data,BUFF_SIZE);

    /*Write data*/
    flash_write_data(0x08040000,(uint32_t *)pbuff_test_string, get_word_length(pbuff_test_string));

    /*Read data*/
    flash_read_data(0x08040000,buff_rcv_data,get_word_length(pbuff_test_string));

    /*Convert to string*/
    get_str((uint32_t *)buff_rcv_data,buff_rcv_char);

    /*Test single word*/
    flash_write_num(0x08020000,g_curr_val);

    /*Get single word*/
    g_rcv_val = flash_read_num(0x08020000);


    /*Test WRP*/
    //flash_setting =  flash_obytes_get_wrp();

    /*Test RDP*/
    flash_setting =  flash_obytes_get_rdp();

    /*Test all obytes settings*/
    flash_obytes_get_config(&all_obytes_setting);

    /*Enable Write protection for SECTOR 4*/

    sector_set_wrp(SECTOR_4);

    /*Write data*/
    flash_write_data(0x08010000,(uint32_t *)pbuff_test_string2, get_word_length(pbuff_test_string2));

    /*Read data*/
    flash_read_data(0x08010000,buff_rcv_data2,get_word_length(pbuff_test_string2));

    /*Convert to string*/
    get_str((uint32_t *)buff_rcv_data2,buff_rcv_char2);


    /*Disaqble all write protection*/
    dis_wrp_all_sectors();


    /*Write data*/
    flash_write_data(0x08010000,(uint32_t *)pbuff_test_string2, get_word_length(pbuff_test_string2));

    /*Read data*/
    flash_read_data(0x08010000,buff_rcv_data2,get_word_length(pbuff_test_string2));

    /*Convert to string*/
    get_str((uint32_t *)buff_rcv_data2,buff_rcv_char2);
	while(1)
	{

	}
}


static uint32_t get_word_length(char *buff)
{
	uint32_t ret;

	ret =  (strlen(buff)/4) + ((strlen(buff)%4) !=0);

	return ret;

}

