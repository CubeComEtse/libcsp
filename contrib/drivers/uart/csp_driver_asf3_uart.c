/*
 * csp_usart_asf3.c
 *
 * Created: 9/22/2025 6:10:11 PM
 *  Author: Adriaan van der West
 */

#include <stdlib.h>
#include <string.h>

#include "FreeRTOS.h"
#include "semphr.h"

#include "csp/drivers/usart.h"
#include "ccd_uart_driver.h"

typedef struct usart_context_s {
    char name[CSP_IFLIST_NAME_MAX + 1];
    csp_usart_callback_t rx_callback;
    void * user_data;
    csp_usart_fd_t fd;
} usart_context_t;

// TODO: The context should probably be instantiated upon calling csp_usart_open with ccd_uart_handle and semaphore lock being contained in the context

usart_context_t csp_usart_context = {0};

static SemaphoreHandle_t lock = NULL;
static StaticSemaphore_t xSemaphoreBuffer;

void csp_usart_lock(void * driver_data)
{
    configASSERT(lock != NULL);
    xSemaphoreTake(lock, portMAX_DELAY);
}

void csp_usart_unlock(void * driver_data)
{
    configASSERT(lock != NULL);
    xSemaphoreGive(lock);
}

int csp_usart_write(csp_usart_fd_t fd, const void * data, size_t data_length)
{
    ccd_b_uart_Send_message((void *)fd, data, data_length);
    
    return data_length;
}

int csp_usart_open(const csp_usart_conf_t * conf, csp_usart_callback_t rx_callback, void * user_data, csp_usart_fd_t * return_fd)
{
    if (rx_callback == NULL) {
        return CSP_ERR_INVAL;
    }
    
    usart_context_t * ctx = calloc(1, sizeof(usart_context_t));
    if (ctx == NULL) {
        return CSP_ERR_NOMEM;
    }
    
    lock = xSemaphoreCreateBinaryStatic(&xSemaphoreBuffer);
    configASSERT(lock != NULL);
    xSemaphoreGive(lock);
    
    strcpy(ctx->name, conf->device);
    ctx->rx_callback = rx_callback;
    ctx->user_data = user_data;
    ctx->fd = conf->ccd_usart_handle;

    if (return_fd) {
        *return_fd = ctx->fd;
    }
    
    return CSP_ERR_NONE;
}
