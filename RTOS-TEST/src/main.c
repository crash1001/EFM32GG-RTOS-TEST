

#include "FreeRTOS.h"
#include "task.h"


#include "em_device.h"
#include "em_system.h"
#include "em_chip.h"
#include "em_gpio.h"
#include "em_cmu.h"
#include "em_core.h"
//#include "gpiointerrupt.h"
/*-----------------------------------------------------------*/

/*Definations */
const uint16_t LED0 = 2;
const uint16_t LED1 = 3;

const uint16_t PB1 = 9;
const uint16_t PB2 = 10;

xTaskHandle xHandle;
/*-----------------------------------------------------------*/

/*
 * Configure the hardware as necessary to run this demo.
 */
static void prvSetupHardware( void );
void GPIOInitialize( void );
void vBlinky1( void *pvParameters);
void vBlinky2();
//void gpioCallBack(uint8_t pin);

//void vApplicationGetIdleTaskMemory(void);
//void vApplicationIdleHook(void);
//void vApplicationStackOverflowHook(void);
//void vApplicationTickHook(void);
void xTimerCreateTimerTask(void);
//void vApplicationMallocFailedHook(void);

void GPIO_ODD_IRQHandler( void );
/*-----------------------------------------------------------*/

int main( void )
{
	
	/* Configure the hardware ready to run the demo. */
	prvSetupHardware();
	
	xTaskCreate(vBlinky1, "Blinky1", 1, NULL, NULL, NULL);
	
//	vTaskSuspend(xHandle);
	
	vTaskStartScheduler();
	
	while(1) {
//		vTaskSuspend(xHandle);
		}
	
	
}
/*-----------------------------------------------------------*/

static void prvSetupHardware( void )
{
	/* Library initialisation routines. */
	CHIP_Init();
	SystemInit();
	GPIOInitialize();
	
//	xTaskCreate(vBlinky1, "Blinky1", 1000, NULL, 1, NULL);
//	xTaskCreate(vBlinky2, "Blinky2", 1000, NULL, 1, NULL);
}

/*-----------------------------------------------------------*/

void GPIOInitialize( void )	{
	
	//Enable Clock For GPIO
	CMU_ClockEnable(cmuClock_GPIO, true);
	
	//GPIO Interrupt Initialization
//	GPIOINT_Init();
	

	
	//Set GPIO PIns PE2 and PE3 as Output Leds
	GPIO_PinModeSet( gpioPortE, LED0, gpioModePushPull, 1);
	GPIO_PinModeSet( gpioPortE, LED1, gpioModePushPull, 1);
	
	//Set GPIO PB9, PB10 as Button Inputs
	GPIO_PinModeSet( gpioPortB, PB1, gpioModeInput, 1);
	GPIO_PinModeSet( gpioPortB, PB2, gpioModeInput, 1);
	
	//Make PE2 and PE3 High
	GPIO_PinOutClear( gpioPortE, LED0);
	GPIO_PinOutClear( gpioPortE, LED1);
	
//	GPIOINT_CallbackRegister(PB1, gpioCallBack);
//	GPIOINT_CallbackRegister(PB2, gpioCallBack);
		
	
//	GPIO_IntEnable(1<<PB1 | 1<<PB2);
	
//	NVIC_ClearPendingIRQ(GPIO_ODD_IRQn);
	NVIC_EnableIRQ(GPIO_ODD_IRQn);
	
//	NVIC_ClearPendingIRQ(GPIO_EVEN_IRQn);
//	NVIC_EnableIRQ(GPIO_EVEN_IRQn);
//	
//	GPIO_IntEnable(PB1);
//	GPIO_IntEnable(PB2);
	
	GPIO_IntConfig(gpioPortB, PB1, false, true, true);
//	GPIO_IntConfig(gpioPortB, PB2, false, true, true);
}	


/*-----------------------------------------------------------*/

void vBlinky1( void *pvParameters ) {
	const char *pcTaskName = "Blinky us running\r\n";
//	while(1) {
		
		GPIO_PinOutToggle( gpioPortE, LED0);
//		vTaskDelay(1000);
//	}
	vTaskDelete(NULL);
}


void vBlinky2() {
	const char *pcTaskName = "Blinky us running\r\n";
//	while(1) {
//		
		GPIO_PinOutToggle( gpioPortE, LED1);
//		vTaskDelay(1000);
//	}
	vTaskDelete(NULL);
}



void GPIO_ODD_IRQHandler( void ) {

		GPIO_IntClear(0x200);
		xTaskCreate(vBlinky1, "Blinky1", 1, NULL, NULL, NULL);

}


void xTimerCreateTimerTask(void){}
	