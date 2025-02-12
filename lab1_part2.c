/*
 * Lab 1, Part 2 - RGB LED Control with FreeRTOS
 *
 * ECE-315 WINTER 2025 - COMPUTER INTERFACING
 * Created on: January 9, 2025
 * Author(s):  Antonio Andara Lara
 *
 * Summary:
 * 1) Update BSP to 1000 ticks/sec.
 * 2) Toggle RGB LED at varying frequencies to find persistence of vision threshold.
 * 3) Implement a PWM-like task to smoothly transition LED brightness.
 *
 * Deliverables:
 * - Show flicker elimination frequency and period.
 * - Demonstrate smooth brightness transitions.
 */


// Include FreeRTOS Libraries
#include "FreeRTOS.h"
#include "task.h"

// Include Xilinx Libraries
#include "xgpio.h"

// RGB LED Colors
#define RGB_OFF     0b000
#define RGB_RED     0b100
#define RGB_GREEN   0b010
#define RGB_BLUE    0b001
#define RGB_YELLOW  0b110 // Red + Green
#define RGB_CYAN    0b011 // Green + Blue
#define RGB_MAGENTA 0b101 // Red + Blue
#define RGB_WHITE   0b111

// RGB LED Device ID
#define RGBLedInst_ID XPAR_AXI_LEDS_DEVICE_ID
#define RGB_CHANNEL 2

#define MAX_TICK_DELAY (TickType_t) 50
#define MIN_TICK (TickType_t) 10

/*************************** Enter your code here ****************************/
    // TODO: Declare RGB LED peripheral
XGpio RGBLedInst;

/*****************************************************************************/

/*************************** Enter your code here ****************************/
    // TODO: Task prototype
static void rgb_led_task(void *pvParameters);
static void other_led_task(void *pvParameters);

/*****************************************************************************/

int main(void)
{
    int status;
/*************************** Enter your code here ****************************/
	// TODO:
	// 1) Configure the RGB LED pins as output.
	// 2) Create the FreeRTOS task for the RGB LED.
	// 3) Start the scheduler.
    status = XGpio_Initialize(&RGBLedInst, RGBLedInst_ID);

    if (status != XST_SUCCESS) {
    	xil_printf("GPIO Initialization for RGB Led failed.\r\n");
    	return XST_FAILURE;
    }

    XGpio_SetDataDirection(&RGBLedInst, RGB_CHANNEL, 0x00);
    xil_printf("Initialization Complete, System Ready!\r\n");

    // Create tasks
//    xTaskCreate(rgb_led_task,
//    		"main task",
//			configMINIMAL_STACK_SIZE,
//			NULL,
//			tskIDLE_PRIORITY,
//			NULL);

    xTaskCreate(other_led_task,
     		"main task2",
 			configMINIMAL_STACK_SIZE,
 			NULL,
 			tskIDLE_PRIORITY,
 			NULL);

    vTaskStartScheduler();
/*****************************************************************************/


    while (1);
    return 0;
}


static void rgb_led_task(void *pvParameters)
{
/*************************** Enter your code here ****************************/
    // TODO: Declare a variable of type TickType_t named 'xDelay'.
	TickType_t xDelay = MIN_TICK;

/*****************************************************************************/
    while (1){

/*************************** Enter your code here ****************************/
    // TODO: Implement a loop that increments xDelay by 1 tick in each iteration,
    //       Allow the loop to run for 3 seconds for each xDelay value.
	//       Use xil_printf to display xDelay and its associated period and frequency
	//       Select a color for the RGB LED.
    // Min:
    // 10 ticks
    // freq = 50 Hz

   	u32 freq;
 	TickType_t period;

   	if (xDelay <= MAX_TICK_DELAY) {
    		period = xDelay*2;
    		freq = 1* 1000 / period; // ms to s
    		xil_printf("xDelay: %d ticks, Frequency: %d Hz\r\n", xDelay, freq);
    		for (u16 t = 0; t < 3000 / period; t++) {
					XGpio_DiscreteWrite(&RGBLedInst, RGB_CHANNEL, RGB_GREEN);
					vTaskDelay(xDelay);
					XGpio_DiscreteWrite(&RGBLedInst, RGB_CHANNEL, RGB_OFF);
					vTaskDelay(xDelay);
			}

    	}

  	xDelay++;
/*****************************************************************************/
    }
}

/*************************** Enter your code here ****************************/
// TODO: Write the second task to control the duty cycle of the RGB LED signal.

static void other_led_task(void *pvParameters)
{
	const TickType_t period = 10*2;  //same period as identified
	TickType_t onDelay, offDelay;

	// Each iteration is 2T
	while (1) {
    	// Ramp up brightness (duration of one T)
    	for (onDelay = 0; onDelay <= period; onDelay++) {
    		//decreasing offDelay as you go, but increasing onDelay up to the period established previously. Longer means full brigthness established
        	offDelay = period - onDelay;
        	XGpio_DiscreteWrite(&RGBLedInst, RGB_CHANNEL, RGB_BLUE);
        	//on de 0 a period = 20
        	vTaskDelay(onDelay);
        	XGpio_DiscreteWrite(&RGBLedInst, RGB_CHANNEL, RGB_OFF);
        	//off delay has 1 when onDelay es 19. Iteration time is period = 20 ticks.
        	vTaskDelay(offDelay);
    	}

    	//Decrease brightness (duration of one T)
    	for (onDelay = period; onDelay > 0; onDelay--) {
        	offDelay = period - onDelay;
        	XGpio_DiscreteWrite(&RGBLedInst, RGB_CHANNEL, RGB_BLUE);
        	//decrease onDelay to show brightness going down
        	vTaskDelay(onDelay);
        	XGpio_DiscreteWrite(&RGBLedInst, RGB_CHANNEL, RGB_OFF);
        	//offDelay increases as it goes
        	vTaskDelay(offDelay);
    	}
	}
}


/*****************************************************************************/
