// https://agisostack-plus-plus.readthedocs.io/en/latest/Tutorials/ESP32%20PlatformIO.html

// This is boilerplate code that can be found in nearly every AgIsoStack project 
// that sets up your device and starts the CAN stack, with slight modifications for ESP32.

// At an absolute minimum, you’ll need these files to send or receive anything on ISOBUS:
#include "isobus/hardware_integration/twai_plugin.hpp"
#include "isobus/hardware_integration/can_hardware_interface.hpp"
#include "isobus/isobus/can_network_manager.hpp"
#include "isobus/isobus/can_partnered_control_function.hpp"
#include "esp_log.h"
#include "freertos/task.h"
#include "driver/gpio.h"
#include "driver/twai.h"

extern "C" void app_main() {
// Adding the following code will configure the TWAI.
 // This code sets GPIO 20 and GPIO 21 to the be the TWAI transmit and receive pins respectively, 
 // and configures the default ISO11783/J1939 baud rate of 250k bits
    twai_general_config_t twaiConfig = TWAI_GENERAL_CONFIG_DEFAULT(GPIO_NUM_20, GPIO_NUM_21, TWAI_MODE_NORMAL);
    twai_timing_config_t twaiTiming = TWAI_TIMING_CONFIG_250KBITS();
    twai_filter_config_t twaiFilter = TWAI_FILTER_CONFIG_ACCEPT_ALL();
    std::shared_ptr<isobus::CANHardwarePlugin> canDriver = std::make_shared<isobus::TWAIPlugin>(&twaiConfig, &twaiTiming, &twaiFilter);

    isobus::CANHardwareInterface::set_number_of_can_channels(1);
    isobus::CANHardwareInterface::assign_can_channel_frame_handler(0, canDriver);
    isobus::CANHardwareInterface::set_periodic_update_interval(10); // Default is 4ms, but we need to adjust this for default ESP32 tick rate of 100Hz

    if (!isobus::CANHardwareInterface::start() || !canDriver->get_is_valid())
    {
        ESP_LOGE("AgIsoStack", "Failed to start hardware interface, the CAN driver might be invalid");
    }

    isobus::NAME TestDeviceNAME(0);

    //! Consider customizing some of these fields, like the function code, to be representative of your device
    TestDeviceNAME.set_arbitrary_address_capable(true);
    TestDeviceNAME.set_industry_group(1);
    TestDeviceNAME.set_device_class(0);
    TestDeviceNAME.set_function_code(static_cast<std::uint8_t>(isobus::NAME::Function::RateControl));
    TestDeviceNAME.set_identity_number(2);
    TestDeviceNAME.set_ecu_instance(0);
    TestDeviceNAME.set_function_instance(0);
    TestDeviceNAME.set_device_class_instance(0);
    TestDeviceNAME.set_manufacturer_code(1407);
    auto TestInternalECU = isobus::CANNetworkManager::CANNetwork.create_internal_control_function(TestDeviceNAME, 0);

    while (true)
    {
        // CAN stack runs in other threads. Do nothing forever.
        vTaskDelay(10);
    }

    isobus::CANHardwareInterface::stop();
}