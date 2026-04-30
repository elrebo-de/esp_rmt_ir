# IR transmitter/receiver component using the "NEC protocol"

This repository contains an ESP-IDF component for an IR transmitter and receiver using the NEC protocol.It runs on 
any ESP32 processor connected to an IR receiver and/or IR transmitter and is built using the ESP-IDF build system in version 5.5+.

The component is implemented as C++ class `NecIr`.

## Connecting the component and Usage

You need to include ```nec_ir.hpp```.

Then you have to initialize the `NecIr` class:
```C++
    /* Initialize NecIr class */
    NecIr* necIr = &necIr->getInstance(); // get the Singleton instance
    necIr->setGpioPins(12,26); // set the GPIO pins
    necIr->initialize(); // initialize NEC IR RMT
```

Now you can transmit NEC IR codes with `transmitNecCommandFrame` and `transmitNecRepeatFrame`.

This example sends the IR code for pushing the "TV Scene" button on the remote control of a YAMAHA Audio receiver: 
```C++
        necIr->transmitNecCommandFrame(0x857a, 0x7c03); // "TV Scene"
```

It is also possible to receive IR signals with an IR receiver.

This example receives IR signals in an endless loop and displays prints the received data in the log:
```C++
    while(1) {
        necIr->receiveNecFrame();
    }
```

## API
The API of the component is located in the include directory ```include/nec_ir.hpp``` and defines the
C++ class ```NecIr```.

```NecIr``` is a Singleton class.

The beginning of the class definition is shown here:
```C++
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
                          uint16_t rxPin   // GPIO pin for IR receiver
                        );

        void initialize();
        void transmitNecCommandFrame(uint16_t address, uint16_t code);
        void transmitNecRepeatFrame();
        void receiveNecFrame();
        
        ...
```

# ToDo
Currently the only tested IR sender is the built-in IR LED in M5Atom Lite. All configurations are fixed for this environment. They have to be exposed to the class user in later versions.

Currently there is no access to the received IR signals, they are only printed to the log. This has to be changed in a later version.

# License
This component is provided under the Apache 2.0 license.

The work is based on the github repository `espressif/esp-idf`, where this example code is included: `esp-idf/examples/peripherals/rmt/ir_nec_transceiver`
This component is based on this example code. 
