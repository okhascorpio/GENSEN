/**
 *
 * \file main.c
 * \brief Example bare skeleton for starting a new Nano series application.
 *
 */



/* Standard includes. */
#include <stdlib.h>
#include <string.h>
#include <sys/inttypes.h>

/* Scheduler includes. */
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"

/* NanoStack includes */
#include "stack.h"
#include "socket.h"
#include "debug.h"
#include "control_message.h"

/* Platform includes */
#include "uart.h"
#include "rf.h"
#include "bus.h"
#include "dma.h"
#include "timer.h"
#include "gpio.h"

static void vAppTask( int8_t *pvParameters );


sockaddr_t datasensor_address = {
	ADDR_802_15_4_PAN_LONG,
		{ 0xFF, 0xFF, 0xFF, 0xFF, 
		  0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF },
		61619
};



/* Main task, initialize hardware and start the FreeRTOS scheduler */
int main( void )
{
	/* Initialize the Nano hardware */
	LED_INIT();
	bus_init();
	debug_init(115200); /*UART1*/
	
	stack_init();
	/* Setup the application task and start the scheduler */
	xTaskCreate( vAppTask, "App", configMAXIMUM_STACK_SIZE, NULL, (tskIDLE_PRIORITY + 2 ), ( xTaskHandle * )NULL );
	vTaskStartScheduler();
	
	/* Scheduler has taken control, next vAppTask starts executing. */

	return 0;
}

/** 
 * Skeleton application task 
 */
static void vAppTask( int8_t *pvParameters )
{
	uint8_t *dptr;
	buffer_t *buf;
	int8_t new_power = 100;
	socket_t *test_socket;
	uint8_t ptr = 0, header = 0, ind=0;

	
	LED1_ON();
	vTaskDelay( 500 / portTICK_RATE_MS );
	LED1_OFF();
	
	test_socket = socket(MODULE_CUDP, 0);
		if (test_socket)
			{
				/* Bind socket for port number 61619*/
				if (socket_bind(test_socket, &datasensor_address) != pdTRUE)
				{
					debug("socket bind failed");
				}							
			}

	
	
	/* Start an endless task loop, we must sleep most of the time allowing execution of other tasks. */
	for (;;)
	{
		
		buf = socket_read(test_socket, 100 / portTICK_RATE_MS);
		if(buf)
		{
			ptr = buf->buf_ptr;
			header = buf->buf[ptr++];
			if(header == 3) /*set power*/
			{
				header = buf->buf[ptr++];
				new_power = rf_power_set(header);
			}
			
			socket_buffer_free(buf);
			
			buf = socket_buffer_get(test_socket);
			if(buf)
			{
				
				buf->buf_end=0;
				buf->buf_ptr=0;
				dptr = buf->buf + buf->buf_ptr;
											
				*dptr++ = 0x01;  /* Versio */
				*dptr++ = 0x03;
				*dptr++ = 0x04;
				*dptr++ = 0x05;
				*dptr++ = 0x06;
				*dptr++ = 0x07;
				*dptr++ = 0x08;
				
				buf->buf_end = (dptr - buf->buf);
				if (socket_sendto(test_socket, &datasensor_address, buf) == pdTRUE) {
					//	data_send_period = xTaskGetTickCount();
					}
					else
					{
						socket_buffer_free(buf);
						debug("Socket send fail\r\n");
					}
				vTaskDelay(200 / portTICK_RATE_MS);
			}
		}
		
		/* Sleep for 500 ms */
		vTaskDelay( 200 / portTICK_RATE_MS );
		LED2_TOGGLE();
	}
}


