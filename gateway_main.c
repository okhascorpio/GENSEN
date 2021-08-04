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
	uint16_t measurements, tries;
	uint8_t *dptr;
	buffer_t *buf = 0;
	int16_t byte;
	int8_t rssi;
	socket_t *test_socket;
	unsigned char power;
	
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

	power = 100;
	power = rf_power_set(power);
	
	/* Start an endless task loop, we must sleep most of the time allowing execution of other tasks. */
	for (;;)
	{
		byte = debug_read_blocking(40);
		if (byte != -1)
		{
			switch(byte)
			{
				case 'r':
				{
					measurements = 0;
					tries = 0;
					debug("starti");
						
						
					while(measurements < 50 & tries < 250)
					{		
						/* Get a buffer for data */
						buf = socket_buffer_get(test_socket);
						if (buf) 
						{
						
							buf->buf_end=0;
							buf->buf_ptr=0;
							dptr = buf->buf + buf->buf_ptr;
							
							*dptr++ = 0x01;  /* Versio */
							
							buf->buf_end = (dptr - buf->buf);
							
							
							if (socket_sendto(test_socket, &datasensor_address, buf) == pdTRUE) {
								
							//	data_send_period = xTaskGetTickCount();
							}
							else
							{
								socket_buffer_free(buf);
								
							}
							
						}
						vTaskDelay(100 / portTICK_RATE_MS);
						
						/*Get answer*/
						buf = socket_read(test_socket, 1000 / portTICK_RATE_MS);
						if(buf)
						{
							measurements++;
							tries++;
							rssi = buf->options.rf_dbm;
							debug_int(rssi);	
							socket_buffer_free(buf);
						}
						else
						{
							tries++;
						}
						vTaskDelay(100 / portTICK_RATE_MS);
					}
					debug("Tries:");
					debug_int(tries);
					debug("\r\n");
					break;
				}
				case 'i':
				{
					if(power == 100)
					{
						power += 10;
					}
					else
					{
						power += 20;
					}
				}
				case 'd': /*Decrease power level*/
				{
					if(power == 10)
					{
						power = 10;
					}
					else
					{
					power -= 10;
					}
					/*increase power level*/
					buf = socket_buffer_get(test_socket);
					if (buf) 
					{
						buf->buf_end=0;
						buf->buf_ptr=0;
						dptr = buf->buf + buf->buf_ptr;
						
						*dptr++ = 0x03;  /* Versio */
						*dptr++ = power;
						
						buf->buf_end = (dptr - buf->buf);
						if (socket_sendto(test_socket, &datasensor_address, buf) == pdTRUE) {
														
						//	data_send_period = xTaskGetTickCount();
						}
						else
						{
							socket_buffer_free(buf);		
						}
					}
					debug("power:");
					debug_int(power);
					break;
				}
				default:
				{
					
				}
			}
		}
		
		/* Sleep for 500 ms */
		vTaskDelay( 500 / portTICK_RATE_MS );
		LED2_TOGGLE();
	}
}


