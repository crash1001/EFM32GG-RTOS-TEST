#ifndef ITM_H
#define ITM_H

#include <stdio.h>
#include "em_device.h"

#ifdef __cplusplus
extern "C" {
#endif
	
//struct __FILE {int handle;/* Add whatever you need here */};

//int fputc(int ch, FILE *f);

//SWO Setup
void SWO_Setup(void);

void itmPrintln(char *string);
void itmPrint(char *string);
void itmPrintInt(volatile int value);
	
#ifdef __cplusplus
}
#endif

#endif

