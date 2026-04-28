/*
 * nec_ir.hpp
 *
 *      Author: christophoberle
 *
 * this work is licenced under the Apache 2.0 licence
 */

#ifndef NEC_IR_HPP_
#define NEC_IR_HPP_

#include <string>

/* class NecIr
   Class to implement an IR transmitter / receiver which uses the "NEC protocl".

   The original code is taken from the GitHub repository https://github.com/espressif/esp-idf.git
   from file esp-idf/examples/peripherals/rmt/ir_nec_transceiver.

   Source of Singleton class structure - https://stackoverflow.com/a/1008289
   Posted by Loki Astari, modified by community. See post 'Timeline' for change history
   Retrieved 2026-02-01, License - CC BY-SA 4.0
*/

class NecIr
{
    public:
        static NecIr& getInstance();
        void setGpioPins( uint16_t txPin,  // GPIO pin for IR transmitter
                          uint16_t rcPin   // GPIO pin for IR receiver
                                    );

        void initialize();
        void transmit(uint16_t address, uint16_t code);
        void receive();

    private:
        NecIr() {}                 // Constructor

        std::string tag = "NecIr";
        uint16_t txPin = 12;
        uint16_t rcPin = 26;

        bool initialized = false;

    public:
        NecIr(NecIr const&) = delete;
        void operator=(NecIr const&) = delete;
};

#endif /* NEC_IR_HPP_ */
