#include "FreeRTOS.h"
#include "task.h"
#include "semphr.h"

void delay_ms(uint32_t ms) {
	vTaskDelay(ms);
}

void freertos_semaphore_delete(void * semphr) {
	vSemaphoreDelete(semphr);
}

void * freertos_semaphore_create_binary(void) {
	return xSemaphoreCreateBinary();
}

uint32_t freertos_give_semaphore_from_isr(void * semaphore, signed long * higher_priority) {
	return xSemaphoreGiveFromISR(semaphore, higher_priority);
}

uint32_t freertos_take_semaphore(void * semaphore, uint32_t wait) {
	return xSemaphoreTake(semaphore, wait);
}

void * freertos_semaphore_create_mutex(void) {
	return xSemaphoreCreateMutex();
}

void freertos_give_semaphore(void * s) {
	xSemaphoreGive(s);
}

void * freertos_memory_allocate(size_t size) {
	return pvPortMalloc(size);
}

void freertos_memory_free(void * ptr) {
	vPortFree(ptr);
}

void freertos_suspend_all(void) {
	vTaskSuspendAll();
}

void freertos_resume_all(void) {
	xTaskResumeAll();
}
