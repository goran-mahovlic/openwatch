// Single bank bootloader for NRF52 / ID107HR Plus smartwatch - Olivier Mauti 2017
// Starts in DFU mode after reset, timeout 30s (to be adjusted in SDK)
//
//

#include "dfu_transport.h"
#include "bootloader.h"
#include "bootloader_util.h"
#include <stdint.h>
#include <string.h>
#include <stddef.h>
#include "nordic_common.h"
#include "nrf.h"
#include "nrf_soc.h"
#include "app_error.h"
#include "nrf_gpio.h"
#include "ble.h"
#include "nrf.h"
#include "ble_hci.h"
#include "app_scheduler.h"
#include "app_timer_appsh.h"
#include "nrf_error.h"
#include "bsp.h"
#include "softdevice_handler_appsh.h"
#include "pstorage_platform.h"
#include "nrf_mbr.h"
#include "nrf_log.h"


#define IS_SRVC_CHANGED_CHARACT_PRESENT 1                                                       /**< Include the service_changed characteristic. For DFU this should normally be the case. */

#define APP_TIMER_PRESCALER             0                                                       /**< Value of the RTC1 PRESCALER register. */
#define APP_TIMER_OP_QUEUE_SIZE         4                                                       /**< Size of timer operation queues. */

#define SCHED_MAX_EVENT_DATA_SIZE       MAX(APP_TIMER_SCHED_EVT_SIZE, 0)                        /**< Maximum size of scheduler events. */

#define SCHED_QUEUE_SIZE                20                                                      /**< Maximum number of events in the scheduler queue. */


/**@brief Callback function for asserts in the SoftDevice.
 *
 * @details This function will be called in case of an assert in the SoftDevice.
 *
 * @warning This handler is an example only and does not fit a final product. You need to analyze 
 *          how your product is supposed to react in case of Assert.
 * @warning On assert from the SoftDevice, the system can only recover on reset.
 *
 * @param[in] line_num    Line number of the failing ASSERT call.
 * @param[in] file_name   File name of the failing ASSERT call.
 */
void assert_nrf_callback(uint16_t line_num, const uint8_t * p_file_name)
{
    app_error_handler(0xDEADBEEF, line_num, p_file_name);
}


/**@brief Function for initializing the timer handler module (app_timer).
 */
static void timers_init(void)
{
    // Initialize timer module, making it use the scheduler.
    APP_TIMER_APPSH_INIT(APP_TIMER_PRESCALER, APP_TIMER_OP_QUEUE_SIZE, true);
}

/**@brief Function for dispatching a BLE stack event to all modules with a BLE stack event handler.
 *
 * @details This function is called from the scheduler in the main loop after a BLE stack
 *          event has been received.
 *
 * @param[in]   p_ble_evt   Bluetooth stack event.
 */
static void sys_evt_dispatch(uint32_t event)
{
    pstorage_sys_event_handler(event);
}


static void ble_stack_init(void)
{
    uint32_t         err_code;
    sd_mbr_command_t com = {SD_MBR_COMMAND_INIT_SD, };
    nrf_clock_lf_cfg_t clock_lf_cfg = NRF_CLOCK_LFCLKSRC;

    err_code = sd_mbr_command(&com);
    APP_ERROR_CHECK(err_code);
    
    err_code = sd_softdevice_vector_table_base_set(BOOTLOADER_REGION_START);
    APP_ERROR_CHECK(err_code);
   
    SOFTDEVICE_HANDLER_APPSH_INIT(&clock_lf_cfg, true);

    // Enable BLE stack.
    ble_enable_params_t ble_enable_params;
    // Only one connection as a central is used when performing dfu.
    err_code = softdevice_enable_get_default_config(1, 1, &ble_enable_params);
    APP_ERROR_CHECK(err_code);

    ble_enable_params.gatts_enable_params.service_changed = IS_SRVC_CHANGED_CHARACT_PRESENT;
    err_code = softdevice_enable(&ble_enable_params);
    APP_ERROR_CHECK(err_code);
    
    err_code = softdevice_sys_evt_handler_set(sys_evt_dispatch);
    APP_ERROR_CHECK(err_code);
}


/**@brief Function for event scheduler initialization.
 */
static void scheduler_init(void)
{
    APP_SCHED_INIT(SCHED_MAX_EVENT_DATA_SIZE, SCHED_QUEUE_SIZE);
}


/**@brief Function for bootloader main entry.
 */
int main(void)
{
    uint32_t err_code;
    
    // This check ensures that the defined fields in the bootloader corresponds with actual
    // setting in the chip.
    APP_ERROR_CHECK_BOOL(*((uint32_t *)NRF_UICR_BOOT_START_ADDRESS) == BOOTLOADER_REGION_START);
    APP_ERROR_CHECK_BOOL(NRF_FICR->CODEPAGESIZE == CODE_PAGE_SIZE);

    // Initialize.
    timers_init();

    (void)bootloader_init();

    if (bootloader_dfu_sd_in_progress())
    {

        err_code = bootloader_dfu_sd_update_continue();
        APP_ERROR_CHECK(err_code);

        ble_stack_init();
        scheduler_init();

        err_code = bootloader_dfu_sd_update_finalize();
        APP_ERROR_CHECK(err_code);
    }
    else
    {
        // If stack is present then continue initialization of bootloader.
        ble_stack_init();
        scheduler_init();
    }
    
   
	// Start DFU mode
	err_code = bootloader_dfu_start();
	APP_ERROR_CHECK(err_code);

	// run app if valid
    if (bootloader_app_is_valid(DFU_BANK_0_REGION_START) && !bootloader_dfu_sd_in_progress())
    {
        bootloader_app_start(DFU_BANK_0_REGION_START);
    }
    
    NVIC_SystemReset();
}
