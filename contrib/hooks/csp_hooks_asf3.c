/* Atmel Software Framework 3 */

#include "csp/csp_hooks.h"
#include "core_cm7.h"

void csp_reboot_hook(void) {
    NVIC_SystemReset();
}

void csp_shutdown_hook(void) {
    NVIC_SystemReset();
}