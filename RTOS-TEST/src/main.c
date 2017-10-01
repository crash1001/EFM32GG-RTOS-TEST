

#include "FreeRTOS.h"
#include "task.h"


#include "ITM.h"
#include "em_chip.h"
#include "em_gpio.h"
#include "em_cmu.h"
#include "em_core.h"
#include "em_adc.h"


/*-----------------------------------------------------------*/

/*Definations */
const uint16_t LED0 = 2;
const uint16_t LED1 = 3;

const uint16_t PB1 = 9;
const uint16_t PB2 = 10;

#define ADC_CLOCK               11000000        /* ADC conversion clock */
//Constant Values for Using Internal Temperature ADC
#define THERMOMETER_GRADIENT  (-6.3)
#define thermCalTemp ((DEVINFO->CAL & _DEVINFO_CAL_TEMP_MASK) >> _DEVINFO_CAL_TEMP_SHIFT)
#define thermCalValue ((DEVINFO->ADC0CAL2 & _DEVINFO_ADC0CAL2_TEMP1V25_MASK) >> _DEVINFO_ADC0CAL2_TEMP1V25_SHIFT)

//uint16_t counter = 0;
/*-----------------------------------------------------------*/

//Hardware Setup Functions
static void prvSetupHardware( void );
void GPIO_Setup( void );
void ADC_Setup( void );

//RTOS TASK Functions
void vBlinky1( void *pvParameters );
void vBlinky2( void *pvParameters );
void vDisplayTemp( void *pvParameters );

//Special Functions
void GPIO_ODD_IRQHandler( void );
void GPIO_EVEN_IRQHandler( void );
void xTimerCreateTimerTask(void);



/*-----------------------------------------------------------*/

int main( void )
{
	
	/* Configure the hardware ready to run the demo. */
	prvSetupHardware();
	
	//HeartBeat
	xTaskCreate(vBlinky2, "Blinky2", 1000, "HeartBeat", 1, NULL);
	
	vTaskStartScheduler();
	
	while(1) {
		
		}
	
	
}
/*-----------------------------------------------------------*/

static void prvSetupHardware( void )
{
	CHIP_Init();
	
	SystemCoreClockUpdate();
	
	SWO_Setup();				//Serial Wire Debug Out Setup
	
	GPIO_Setup();
	
	ADC_Setup();
	
//	vTaskDelay(1000);

}

/*-----------------------------------------------------------*/

void GPIO_Setup( void )	{
	
	//Enable Clock For GPIO
	CMU_ClockEnable(cmuClock_GPIO, true);
	
	//Set GPIO PIns PE2 and PE3 as Output Leds
	GPIO_PinModeSet( gpioPortE, LED0, gpioModePushPull, 1);
	GPIO_PinModeSet( gpioPortE, LED1, gpioModePushPull, 1);
	
	//Set GPIO PB9, PB10 as Button Inputs
	GPIO_PinModeSet( gpioPortB, PB1, gpioModeInput, 1);
	GPIO_PinModeSet( gpioPortB, PB2, gpioModeInput, 1);
	
	//Make PE2 and PE3 Low
	GPIO_PinOutClear( gpioPortE, LED0);
	GPIO_PinOutClear( gpioPortE, LED1);
	
	//Interrupt Enable for PB1(9)
	NVIC_ClearPendingIRQ(GPIO_ODD_IRQn);
	NVIC_EnableIRQ(GPIO_ODD_IRQn);
	
	//Intterrupt Enable for PB2(10)
	NVIC_ClearPendingIRQ(GPIO_EVEN_IRQn);
	NVIC_EnableIRQ(GPIO_EVEN_IRQn);
	
	//Set interrupt to rising edge for PB1(9)
	GPIO_IntConfig(gpioPortB, PB1, false, true, true);
	GPIO_IntConfig(gpioPortB, PB2, false, true, true);
}	

void ADC_Setup()	{
		
	ADC_Init_TypeDef init = ADC_INIT_DEFAULT;
	
	ADC_InitSingle_TypeDef singleInit = ADC_INITSINGLE_DEFAULT;
	
	//Enable Clock for ADC0
	CMU_ClockEnable(cmuClock_ADC0, true);
	
	//Lowpass or decupling capacitor filter select
	init.lpfMode = adcLPFilterDeCap;
	
	//Oversampling rate select in order to have an effect oversampling must be enabled
	init.ovsRateSel = adcOvsRateSel2;
	
	//PreScaler Setting
	init.prescale = ADC_PrescaleCalc(ADC_CLOCK, 0);
	
	//Enable/disable conversion Tailgating
	init.tailgate = false;
	
	//Timebase used for ADC warmup
	init.timebase = ADC_TimebaseCalc(0);
	
	//ADC-Warmup mode to use for ADC
	init.warmUpMode = adcWarmupNormal;
	
	//Initialize ADC
	ADC_Init(ADC0, &init);
	
	//Aquisition time (in ADC clock cycles)
	singleInit.acqTime = adcAcqTime8;
	
	//Single Ended or Differential
	singleInit.diff = false;
	
	//Single Sample Input Selection
	singleInit.input = adcSingleInputTemp;
	
	//Select if left adjustment should be done
	singleInit.leftAdjust = false;
	
	//Peripheral Reflex System Trigger Enable
	singleInit.prsEnable = false;
	
	//Sample Reference Voltage Selection
	singleInit.reference = adcRef1V25;
	
	//Select if continous conversion until explicit stop
	singleInit.rep = false;
	
	//ADC conversion resolution select
	singleInit.resolution = adcRes12Bit;
	
	//Initialize ADC
	ADC_InitSingle(ADC0, &singleInit);
	
	ADC_IntDisable(ADC0, 0x0);
	
}

/*-----------------------------------------------------------*/

void vBlinky1( void *pvParameters ) {
//	const char *pcTaskName = pvParameters;
	
	GPIO_PinOutToggle( gpioPortE, LED0);
	
	itmPrintln(pvParameters);
	
	vTaskDelete(NULL);
}

void vBlinky2( void *pvParameters ) {
//	const char *pcTaskName = pvParameters;
	while(1) {
		GPIO_PinOutToggle( gpioPortE, LED1);
		
		itmPrintln(pvParameters);
		
		vTaskDelay(1000);
		
	}

}

void vDisplayTemp( void *pvParameters ) {
	float adcResult;
	float temp;
	
	//Start ADC Conversion
	ADC_Start(ADC0, adcStartSingle);
	
	//Wait While Conversion is Running
	while(!(ADC0->STATUS & ADC_STATUS_SINGLEDV));
	
	adcResult = ADC_DataSingleGet(ADC0);
	
	temp = thermCalTemp - (thermCalValue - adcResult)*1250/(4096*THERMOMETER_GRADIENT);
	
	itmPrint("Temperature is ");
	itmPrintNum(temp);
	itmPrint("\n");
	
	vTaskDelete(NULL);
}

/*-----------------------------------------------------------*/

void GPIO_ODD_IRQHandler( void ) {
		
		//Cleared Interrupt generated by PB1(9)
		GPIO_IntClear(0x200);
	
		xTaskCreate(vBlinky1, "Blinky1", 1000, "Led0 Tooggled", 1, NULL);

}

void GPIO_EVEN_IRQHandler( void ) {
	GPIO_IntClear(0x400);
	
	xTaskCreate(vDisplayTemp, "Temperature", 2000, "Internal MCU Temp", 2, NULL);
}

void xTimerCreateTimerTask(void){
}






	