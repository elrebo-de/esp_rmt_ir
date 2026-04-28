/*
 * time_sync.cpp
 *
 *      Author: christophoberle
 *
 * this work is licenced under the Apache 2.0 licence
 */

#include <string>

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#include "sdkconfig.h"
#include "esp_log.h"

#include "time_sync.hpp"

#include <time.h>
#include <sys/time.h>
#include <esp_sntp.h>

TimeSync& TimeSync::getInstance()
{
    static TimeSync instance; // Guaranteed to be destroyed. Instantiated on first use.
    return instance;
}

// Function to set sntp servers
void TimeSync::setSntpServers( std::string sntpServer1,
                                 std::string sntpServer2,
                                 std::string sntpServer3
                               )
{
    ESP_LOGI(tag.c_str(), "Set SNTP srvers");
    if(sntpServer1 != "") this->sntpServer1 = sntpServer1;
    if(sntpServer2 != "") this->sntpServer2 = sntpServer2;
    if(sntpServer3 != "") this->sntpServer3 = sntpServer3;
}

// Function to set syncIntervalMs
void TimeSync::setSyncIntervalMs(uint32_t syncIntervalMs)
 {
     ESP_LOGI(tag.c_str(), "Setting syncIntervalMs to: %lu", syncIntervalMs);
     this->syncIntervalMs = syncIntervalMs;
 }

// Function to get syncIntervalMs
uint32_t TimeSync::getSyncIntervalMs()
{
    return this->syncIntervalMs;
}

// Function to test whether time is synchronized
bool TimeSync::isSynchronized()
{
    return this->timeSynchronized;
}

// Function to initialize SNTP  with multiple servers
void TimeSync::initializeSntp()
{
    ESP_LOGI(tag.c_str(), "Initializing SNTP");
    esp_sntp_setoperatingmode(SNTP_OPMODE_POLL);

    // Set multiple NTP servers
    esp_sntp_setservername(0, this->sntpServer1.c_str()); // Primary server
    esp_sntp_setservername(1, this->sntpServer2.c_str()); // Secondary server
    esp_sntp_setservername(2, this->sntpServer3.c_str()); // Tertiary server

    esp_sntp_init();
}

// Function to obtain and print time
void TimeSync::obtainTime(void)
{
    // Wait for time to be set
    time_t now = 0;
    struct tm timeinfo = {
                           0, // tm_sec	int	seconds after the minute	0-60*
                           0, // tm_min	int	minutes after the hour	0-59
                           0, // tm_hour	int	hours since midnight	0-23
                           0, // tm_mday	int	day of the month	1-31
                           0, // tm_mon	int	months since January	0-11
                           0, // tm_year	int	years since 1900
                           0, // tm_wday	int	days since Sunday	0-6
                           0, // tm_yday	int	days since January 1	0-365
                           0  // tm_isdst	int	Daylight Saving Time flag
                         };
    int retry = 0;
    const int retry_count = 10;

    while (timeinfo.tm_year < (2025 - 1900) && ++retry < retry_count)
    {
        ESP_LOGD(tag.c_str(), "Waiting for system time to be set... (%d/%d)", retry, retry_count);
        vTaskDelay(2000 / portTICK_PERIOD_MS);
        time(&now);
        localtime_r(&now, &timeinfo);
    }

    if (retry == retry_count)
    {
        ESP_LOGE(tag.c_str(), "Failed to obtain time.");
        return;
    }

    ESP_LOGI(tag.c_str(), "Time synchronized.");
    this->timeSynchronized = true;
}

// Function to set timezone
void TimeSync::setTimezone(std::string timezone)
{
    ESP_LOGI(tag.c_str(), "Setting timezone to: %s", timezone.c_str());
    setenv("TZ", timezone.c_str(), 1); // Set the TZ environment variable
    tzset(); // Apply the timezone change
}

// Print current calendar information
void TimeSync::printCalendar()
{
    time_t now;
    struct tm timeinfo;
    char buffer[64];

    time(&now);
    localtime_r(&now, &timeinfo);

    strftime(buffer, sizeof(buffer), "Date: %Y-%m-%d, Time: %H:%M:%S", &timeinfo);
    ESP_LOGI(tag.c_str(), "%s", buffer);
    ESP_LOGI(tag.c_str(), "Weekday: %d (0=Sunday, 6=Saturday)", timeinfo.tm_wday);
    ESP_LOGI(tag.c_str(), "Day of the year: %d", timeinfo.tm_yday + 1);
}
