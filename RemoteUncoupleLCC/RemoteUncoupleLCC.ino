// -!- C++ -!- //////////////////////////////////////////////////////////////
//
//  System        : 
//  Module        : 
//  Object Name   : $RCSfile$
//  Revision      : $Revision$
//  Date          : $Date$
//  Author        : $Author$
//  Created By    : Robert Heller
//  Created       : 2025-10-30 09:53:49
//  Last Modified : <251110.1113>
//
//  Description	
//
//  Notes
//
//  History
//	
/////////////////////////////////////////////////////////////////////////////
//
//    Copyright (C) 2025  Robert Heller D/B/A Deepwoods Software
//			51 Locke Hill Road
//			Wendell, MA 01379-9728
//
//    This program is free software; you can redistribute it and/or modify
//    it under the terms of the GNU General Public License as published by
//    the Free Software Foundation; either version 2 of the License, or
//    (at your option) any later version.
//
//    This program is distributed in the hope that it will be useful,
//    but WITHOUT ANY WARRANTY; without even the implied warranty of
//    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//    GNU General Public License for more details.
//
//    You should have received a copy of the GNU General Public License
//    along with this program; if not, write to the Free Software
//    Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
//
// 
//
//////////////////////////////////////////////////////////////////////////////

static const char rcsid[] = "@(#) : $Id$";

#include <Arduino.h>
#include <SPIFFS.h>
#include <USB.h>
#include <USBCDC.h>

#include <OpenMRNLite.h>
#include "openlcb/MultiConfiguredConsumer.hxx"
#include "openlcb/TcpDefs.hxx"
#include "utils/GpioInitializer.hxx"

// Pick an operating mode below, if you select USE_WIFI it will expose this
// node on WIFI. If USE_TWAI / USE_TWAI_ASYNC are enabled the node will be
// available on CAN.
//
// Enabling both options will allow the ESP32 to be accessible from
// both WiFi and CAN interfaces.
//
// NOTE: USE_CAN utilizes the new CAN driver which offers
// both select() (default) or fnctl() (async) access.

#define USE_WIFI
//#define USE_CAN

// Uncomment the line below to have all packets printed to the Serial
// output. This is not recommended for production deployment.
#define PRINT_PACKETS

// uncomment the line below to specify a GPIO pin that should be used to force
// a factory reset when the node starts and the GPIO pin reads LOW.
// NOTE: GPIO 15 is also used for IO16, care must be taken to ensure that this
// GPIO pin is not used both for FACTORY_RESET and an OUTPUT pin.
//#define FACTORY_RESET_GPIO_PIN 15

// Uncomment the line below to configure the native USB CDC for all output from
// OpenMRNLite. When not defined the default UART0 will be used instead.
//
// NOTE: USB CDC is connected to GPIO 19 (D-) and 20 (D+) and can not be
// changed to any other pins.
//#define USE_USB_CDC_OUTPUT

// Uncomment FIRMWARE_UPDATE_BOOTLOADER to enable the bootloader feature when
// using the TWAI device.
//
// NOTE: in order for this to work you *MUST* use a partition schema that has
// two app partitions, typically labeled with "OTA" in the partition name in
// the Arduino IDE.
//#define FIRMWARE_UPDATE_BOOTLOADER

// Configuration option validation

#if defined(FIRMWARE_UPDATE_BOOTLOADER) && !defined(USE_CAN)
#error Firmware update is only supported via CAN, enable USE_CAN to use this.
#endif

#if ARDUINO_USB_CDC_ON_BOOT && defined(USE_USB_CDC_OUTPUT)
// When ARDUINO_USB_CDC_ON_BOOT = 1 "Serial" will map to USB-CDC automatically
// and will be enabled on startup. We do not need to wrap or otherwise treat it
// any differently.
#undef USE_USB_CDC_OUTPUT
#warning Disabling USE_USB_CDC_OUTPUT since USB-CDC is enabled in Arduino IDE
#endif

#include "config.h"

/// This is the node id to assign to this device, this must be unique
/// on the CAN bus.
#include "NODEID.h"
//static constexpr uint64_t NODE_ID = UINT64_C(0x050101012289);

#if defined(USE_WIFI)
// Configuring WiFi accesspoint name and password
// ^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^
// There are two options:
// 1) edit the sketch to set this information just below. Use quotes:
//     const char* ssid     = "linksys";
//     const char* password = "superSecret";
// 2) add a new file to the sketch folder called something.cpp with the
// following contents:
//     #include <OpenMRNLite.h>
//
//     char WIFI_SSID[] = "linksys";
//     char WIFI_PASS[] = "theTRUEsupers3cr3t";

/// This is the name of the WiFi network (access point) to connect to.
const char *ssid = WIFI_SSID;

/// Password of the wifi network.
const char *password = WIFI_PASS;


/// This is the hostname which the ESP32 will advertise via mDNS, it should be
/// unique.
const char *hostname = "boxcar1234mrn";

OVERRIDE_CONST(gridconnect_buffer_size, 3512);
//OVERRIDE_CONST(gridconnect_buffer_delay_usec, 200000);
OVERRIDE_CONST(gridconnect_buffer_delay_usec, 2000);
OVERRIDE_CONST(gc_generate_newlines, CONSTANT_TRUE);
OVERRIDE_CONST(executor_select_prescaler, 60);
OVERRIDE_CONST(gridconnect_bridge_max_outgoing_packets, 2);

#endif // USE_WIFI

#if defined(USE_CAN)
/// This is the ESP32-S2 pin connected to the SN65HVD23x/MCP2551 R (RX) pin.
/// Recommended pins: 40, 41, 42.
/// NOTE: If you are using a pin other than 40 you will likely need to adjust
/// the GPIO pin definitions for the outputs.
constexpr gpio_num_t CAN_RX_PIN = GPIO_NUM_40;

/// This is the ESP32 pin connected to the SN65HVD23x/MCP2551 D (TX) pin.
/// Recommended pins: 40, 41, 42.
/// NOTE: If you are using a pin other than 41 you will likely need to adjust
/// the GPIO pin definitions for the outputs.
constexpr gpio_num_t CAN_TX_PIN = GPIO_NUM_41;

#endif // USE_CAN

#if defined(FACTORY_RESET_GPIO_PIN)
static constexpr uint8_t FACTORY_RESET_COUNTDOWN_SECS = 10;
#endif // FACTORY_RESET_GPIO_PIN

/// This is the primary entrypoint for the OpenMRN/LCC stack.
OpenMRN openmrn(NODE_ID);

/// ConfigDef comes from config.h and is specific to this particular device and
/// target. It defines the layout of the configuration memory space and is also
/// used to generate the cdi.xml file. Here we instantiate the configuration
/// layout. The argument of offset zero is ignored and will be removed later.
static constexpr openlcb::ConfigDef cfg(0);

#if defined(FIRMWARE_UPDATE_BOOTLOADER)
// Include the Bootloader HAL implementation for the ESP32. This should only
// be included in one ino/cpp file.
#include "freertos_drivers/esp32/Esp32BootloaderHal.hxx"
#endif // FIRMWARE_UPDATE_BOOTLOADER

#if defined(USE_WIFI)
Esp32WiFiManager wifi_mgr(ssid, password, openmrn.stack(), cfg.seg().wifi());
#endif // USE_WIFI

#if defined(USE_CAN)
Esp32Can can_driver(CAN_TX_PIN, CAN_RX_PIN);
#endif // USE_CAN

// Declare output pins.  
GPIO_PIN(DIR, GpioOutputSafeLow, 5);
GPIO_PIN(STEP, GpioOutputSafeLow, 6);
const int microMode = 16; // microstep mode, default is 1/16 so 16; ex: 1/4 would be 4
// full rotation * microstep divider
GPIO_PIN(FRONTEN, GpioOutputSafeHigh, 9);
GPIO_PIN(REAREN, GpioOutputSafeHigh, 10);
GPIO_PIN(SLEEP, GpioOutputSafeHigh, 11);

#include "UncouplerConsumer.h"

UncouplerConsumer uncouplerConsumer(openmrn.stack()->node(),
                                    cfg.seg().coupler(),DIR_Pin::instance(), 
                                    STEP_Pin::instance(), FRONTEN_Pin::instance(),
                                    REAREN_Pin::instance(), SLEEP_Pin::instance());


// Create an initializer that can initialize all the GPIO pins in one shot
typedef GpioInitializer<
#if defined(FACTORY_RESET_GPIO_PIN)
    FACTORY_RESET_Pin,                           // factory reset
#endif // FACTORY_RESET_GPIO_PIN
    DIR_Pin, STEP_Pin, FRONTEN_Pin, REAREN_Pin, SLEEP_Pin
    > GpioInit;

class FactoryResetHelper : public DefaultConfigUpdateListener {
public:
    UpdateAction apply_configuration(int fd, bool initial_load,
                                     BarrierNotifiable *done) OVERRIDE {
        AutoNotify n(done);
        return UPDATED;
    }

    void factory_reset(int fd) override
    {
        cfg.userinfo().name().write(fd, openlcb::SNIP_STATIC_DATA.model_name);
        cfg.userinfo().description().write(
            fd, "OpenLCB BoxCar uncoupler " ARDUINO_VARIANT);
    }
} factory_reset_helper;

namespace openlcb
{
    // Name of CDI.xml to generate dynamically.
    const char CDI_FILENAME[] = "/spiffs/cdi.xml";

    // This will stop openlcb from exporting the CDI memory space upon start.
    extern const char CDI_DATA[] = "";

    // Path to where OpenMRN should persist general configuration data.
    extern const char *const CONFIG_FILENAME = "/spiffs/openlcb_config";

    // The size of the memory space to export over the above device.
    extern const size_t CONFIG_FILE_SIZE = cfg.seg().size() + cfg.seg().offset();

    // Default to store the dynamic SNIP data is stored in the same persistant
    // data file as general configuration data.
    extern const char *const SNIP_DYNAMIC_FILENAME = CONFIG_FILENAME;
}

#if defined(USE_USB_CDC_OUTPUT)
// USB CDC wrapper that uses TinyUSB internally to route data to/from the USB
// CDC device driver.
USBCDC USBSerial;

// Override the log_output method from OpenMRNLite to redirect all log output
// via USB CDC.
void log_output(char* buf, int size)
{
    if (size <= 0) return;
    buf[size] = '\0';
    USBSerial.println(buf);
}
#endif // USE_USB_CDC_OUTPUT

void check_for_factory_reset()
{
#if defined(FACTORY_RESET_GPIO_PIN)
    // Check the factory reset pin which should normally read HIGH (set), if it
    // reads LOW (clr) delete the cdi.xml and openlcb_config
    if (!FACTORY_RESET_Pin::instance()->read())
    {
        LOG(WARNING, "!!!! WARNING WARNING WARNING WARNING WARNING !!!!");
        LOG(WARNING, "The factory reset GPIO pin %d has been triggered.",
            FACTORY_RESET_GPIO_PIN);
        for (uint8_t sec = FACTORY_RESET_COUNTDOWN_SECS;
             sec > 0 && !FACTORY_RESET_Pin::instance()->read(); sec--)
        {
            LOG(WARNING, "Factory reset will be initiated in %d seconds.",
                sec);
            usleep(SEC_TO_USEC(1));
        }
        if (!FACTORY_RESET_Pin::instance()->read())
        {
            unlink(openlcb::CDI_FILENAME);
            unlink(openlcb::CONFIG_FILENAME);
            LOG(WARNING, "Factory reset complete");
        }
        else
        {
            LOG(WARNING, "Factory reset aborted as pin %d was not held LOW",
                FACTORY_RESET_GPIO_PIN);
        }
    }
#endif // FACTORY_RESET_GPIO_PIN
}

void setup() {
#if !defined(USE_USB_CDC_OUTPUT)
    Serial.begin(115200L);
#else
    USB.productName(openlcb::SNIP_STATIC_DATA.model_name);
    USB.manufacturerName(openlcb::SNIP_STATIC_DATA.manufacturer_name);
    USB.firmwareVersion(openlcb::CANONICAL_VERSION);
    USB.serialNumber(uint64_to_string_hex(NODE_ID).c_str());
    USB.begin();
    USBSerial.begin();
    USBSerial.setDebugOutput(true);
    // Give time for the USB peripheral to startup and be ready to use.
    delay(5000);
#endif // USE_USB_CDC_OUTPUT

    uint8_t reset_reason = Esp32SocInfo::print_soc_info();
    LOG(INFO, "[Node] ID: %s", uint64_to_string_hex(NODE_ID).c_str());
    LOG(INFO, "[SNIP] version:%d, manufacturer:%s, model:%s, hw-v:%s, sw-v:%s",
        openlcb::SNIP_STATIC_DATA.version,
        openlcb::SNIP_STATIC_DATA.manufacturer_name,
        openlcb::SNIP_STATIC_DATA.model_name,
        openlcb::SNIP_STATIC_DATA.hardware_version,
        openlcb::SNIP_STATIC_DATA.software_version);

    // Initialize the SPIFFS filesystem as our persistence layer
    if (!SPIFFS.begin())
    {
        LOG(WARNING,
            "SPIFFS failed to mount, attempting to format and remount");
        if (!SPIFFS.begin(true))
        {
            LOG_ERROR("SPIFFS mount failed even with format, giving up!");
            while (1)
            {
                // Unable to start SPIFFS successfully, give up and wait
                // for WDT to kick in
            }
        }
    }

#if defined(FIRMWARE_UPDATE_BOOTLOADER)
    // initialize the bootloader.
    esp32_bootloader_init(reset_reason);

    // if we have a request to enter the bootloader we need to process it
    // before we startup the OpenMRN stack.
    if (request_bootloader())
    {
        esp32_bootloader_run(NODE_ID, CAN_RX_PIN, CAN_TX_PIN);
        // This line should not be reached as the esp32_bootloader_run method
        // will not return by default.
        HASSERT(false);
    }
#endif // FIRMWARE_UPDATE_BOOTLOADER
    check_for_factory_reset();

    // Create the CDI.xml dynamically
    openmrn.create_config_descriptor_xml(cfg, openlcb::CDI_FILENAME);

    // Create the default internal configuration file
    openmrn.stack()->create_config_file_if_needed(cfg.seg().internal_config(),
        openlcb::CANONICAL_VERSION, openlcb::CONFIG_FILE_SIZE);

    // initialize all declared GPIO pins
    GpioInit::hw_init();

#if defined(USE_CAN)
    can_driver.begin();
    openmrn.add_can_port(&can_driver);
#endif // USE_CAN

    // Start the OpenMRN stack
    openmrn.begin();

    if (reset_reason == RTCWDT_BROWN_OUT_RESET)
    {
        openmrn.stack()->executor()->add(new CallbackExecutable([]()
        {
            openmrn.stack()->send_event(openlcb::Defs::NODE_POWER_BROWNOUT_EVENT);
        }));
    }

#if defined(PRINT_PACKETS)
    // Dump all packets as they are sent/received.
    // Note: This should not be enabled in deployed nodes as it will have
    // performance impact.
    openmrn.stack()->print_all_packets();
#endif // PRINT_PACKETS

    // start executor thread since this is required for select() to work in the
    // OpenMRN executor.
    openmrn.start_executor_thread();
}
                
void loop() {
    // Call the OpenMRN executor, this needs to be done as often
    // as possible from the loop() method.
    openmrn.loop();
}    
#if defined(FIRMWARE_UPDATE_BOOTLOADER)

extern "C"
{

/// Updates the state of a status LED.
///
/// @param led is the LED to update.
/// @param value is the new state of the LED.
void bootloader_led(enum BootloaderLed led, bool value)
{
    LOG(INFO, "[Bootloader] bootloader_led(%d, %d)", led, value);
}

} // extern "C"

#endif // FIRMWARE_UPDATE_BOOTLOADER
