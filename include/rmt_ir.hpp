/*
 * rmt_ir.hpp
 *
 *      Author: christophoberle
 *
 * this work is licenced under the Apache 2.0 licence
 */

#ifndef RMT_IR_HPP_
#define RMT_IR_HPP_

#include <string>
#include "driver/rmt_tx.h"
#include "driver/rmt_rx.h"

#include "nec_protocol.hpp"
#include "panasonic_protocol.hpp"

/* class RmtIr
   Class to implement an IR transmitter / receiver which uses different protocols.

   The original code is taken from the GitHub repository https://github.com/espressif/esp-idf.git
   from file esp-idf/examples/peripherals/rmt/ir_nec_transceiver.

   Source of Singleton class structure - https://stackoverflow.com/a/1008289
   Posted by Loki Astari, modified by community. See post 'Timeline' for change history
   Retrieved 2026-02-01, License - CC BY-SA 4.0
*/

class RmtIr
{
    public:
        static RmtIr& getInstance();
        void setGpioPins( uint16_t txPin,  // GPIO pin for IR transmitter
                          uint16_t rxPin   // GPIO pin for IR receiver
                        );

        void initialize();

        void transmitNecCommandFrame(uint8_t address, uint8_t code);
        void transmitNecCommandFrame(uint16_t address, uint8_t code);
        void transmitNecCommandFrame(uint16_t address, uint16_t code);
        void transmitNecRepeatFrame();

        void transmitPanasonicCommandFrame(uint16_t non_saving_bits_1, uint8_t system_code, uint8_t address, uint8_t command);

        void receiveNecOrPanasonicFrame();

        NecProtocol *necProtocol;
        PanasonicProtocol *panasonicProtocol;

    private:
        RmtIr() {}                 // Constructor

        std::string tag = "RmtIr";
        uint16_t txPin = 12; // M5 Atom Lite Builtin IR LED
        uint16_t rxPin = 26;

        bool initialized = false;

        rmt_channel_handle_t rx_channel = NULL;
        rmt_channel_handle_t tx_channel = NULL;

        QueueHandle_t receive_queue;

        /**
         * @brief Saving RMT decode results
         */
        uint16_t s_nec_code_address;
        uint16_t s_nec_code_command;

    public:
        RmtIr(RmtIr const&) = delete;
        void operator=(RmtIr const&) = delete;
};

#endif /* RMT_IR_HPP_ */
