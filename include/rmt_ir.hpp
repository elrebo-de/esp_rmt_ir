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

/* class RmtIr
   Class to implement an IR transmitter / receiver which uses the "RMT protocol".

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
        void transmitNecCommandFrame(uint16_t address, uint16_t code);
        void transmitNecRepeatFrame();
        void receiveNecFrame();

    private:
        RmtIr() {}                 // Constructor

        bool nec_check_in_range(uint32_t signal_duration, uint32_t spec_duration);
        bool nec_parse_logic0(rmt_symbol_word_t *rmt_nec_symbols);
        bool nec_parse_logic1(rmt_symbol_word_t *rmt_nec_symbols);
        bool nec_parse_frame(rmt_symbol_word_t *rmt_nec_symbols);
        bool nec_parse_frame_repeat(rmt_symbol_word_t *rmt_nec_symbols);
        void example_parse_nec_frame(rmt_symbol_word_t *rmt_nec_symbols, size_t symbol_num);

        std::string tag = "RmtIr";
        uint16_t txPin = 12; // M5 Atom Lite Builtin IR LED
        uint16_t rxPin = 26;

        bool initialized = false;

        rmt_channel_handle_t rx_channel = NULL;
        rmt_channel_handle_t tx_channel = NULL;

        rmt_encoder_handle_t nec_encoder = NULL;

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
