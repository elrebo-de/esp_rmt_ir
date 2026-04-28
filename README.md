# IR transmitter/receiver component using the "NEC protocol"

This repository contains an ESP-IDF component for an IR transmitter and receiver using the NEC protocol.It runs on 
any ESP32 processor connected to an IR receiver and/or IR transmitter and is built using the ESP-IDF build system in version 5.5+.

The component is implemented as C++ class `NecIr`.

## Connecting the component and Usage

You need to include ```nec_ir.hpp```.

# # # # # # # # # 
After establishing a Wifi connection the ```TimeSync``` class must be initialized and

the timeTask must be started.

From then on the time will be synchronized.
```
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#include "time_sync.hpp"

static void timeTask(void *pc){
    TimeSync* timeSync = &TimeSync::getInstance();

    // Synchronize time
    timeSync->obtainTime();

    while (1)
    {
        timeSync->printCalendar();
        vTaskDelay(pdMS_TO_TICKS(timeSync->get_sync_interval_ms())); // Print calendar every 5 minutes
    }
}

extern "C" void app_main(void)
{
    ... Wifi must be established here
    
    /* Initialize TimeSync class */
    ESP_LOGI(tag, "TimeSync");
    TimeSync* timeSync = &timeSync->getInstance();
    timeSync->initializeSntp();
    timeSync->setTimezone(std::string("CET"));
    timeSync->setSyncIntervalMs(30000);

    xTaskCreate(timeTask, "time_task", 4096, NULL, 5, NULL);

    while(!timeSync->isSynchronized()) {
        ESP_LOGI(tag, "time is not yet synchronized");
        vTaskDelay(pdMS_TO_TICKS(1000)); // delay 1 second
    }

    ... now the time is synchronized and will be synchronized again every sync_interval_ms milliseconds
    
    ... do your work here
}
```

## API
The API of the component is located in the include directory ```include/time_sync.hpp``` and defines the
C++ class ```TimeSync```.

```TimeSync``` is a Singleton class.

```C++
/* class TimeSync
   Class to synchronize time with SNTP servers.

   The original code is taken from the GitHub repository https://github.com/sachin42/idfcomponents.git
   from file HTTPClient/main.cpp.

   Source of Singleton class structure - https://stackoverflow.com/a/1008289
   Posted by Loki Astari, modified by community. See post 'Timeline' for change history
   Retrieved 2026-02-01, License - CC BY-SA 4.0
*/

class TimeSync
{
    public:
        static TimeSync& getInstance();
        void setSntpServers( std::string sntpServer1,  // 1. sntp server
                             std::string sntpServer2,  // 2. sntp server
                             std::string sntpServer3   // 3. sntp server
                           );
        void setTimezone(std::string timezone);
        void setSyncIntervalMs(uint32_t syncIntervalMs);

        uint32_t getSyncIntervalMs();
        bool isSynchronized();

        void initializeSntp();
        void obtainTime(void);
        void printCalendar();

    private:
        TimeSync() {}                 // Constructor

        std::string tag = "TimeSync";
        std::string sntpServer1 = "pool.ntp.org";
        std::string sntpServer2 = "time.nist.gov";
        std::string sntpServer3 = "time.google.com";

        uint32_t syncIntervalMs = 300000;

        bool timeSynchronized = false;

    public:
        TimeSync(TimeSync const&) = delete;
        void operator=(TimeSync const&) = delete;
};
```

# License
This component is provided under the Apache 2.0 license.

The work is based on the github repository `espressif/esp-idf`, where this example code is included: `esp-idf/examples/peripherals/rmt/ir_nec_transceiver`
