# IR transmitter/receiver component using different protocols

This repository contains an ESP-IDF component for an IR transmitter and receiver using different remote control protocols. It runs on 
any ESP32 processor connected to an IR receiver and/or IR transmitter and is built using the ESP-IDF build system in version 5.5+.

Currently two protocols are implemented:
* NEC protocol (used eg. by YAMAHA audio receiver)
* PANASONIC protocol (used eg. by PANASONIC VIERA TV)

The component is implemented as C++ class `RmtIr`.

## Connecting the component and Usage

You need to include ```rmt_ir.hpp```.

Then you have to initialize the `RmtIr` class:
```C++
    /* Initialize RmtIr class */
    RmtIr* rmtIr = &rmtIr->getInstance(); // get the Singleton instance
    rmtIr->setGpioPins(12,26); // set the GPIO pins
    rmtIr->initialize(); // initialize RMT IR
```

Now you can transmit NEC IR codes with `transmitNecCommandFrame` and `transmitNecRepeatFrame` and PANASONIC IR codes with `transmitPanasonicCommandFrame`.

This example sends the IR code for pushing the "TV Scene" button on the remote control of a YAMAHA audio receiver:
```C++
        rmtIr->transmitNecCommandFrame(0x857a, 0x7c03); // "TV Scene"
```

This example sends the IR code for pushing the "Power 0/1" button on the remote control of a PANASONIC VIERA TV:
```C++
        rmtIr->transmitPanasonicCommandFrame(0x2002, 0x80, 0x00, 0x3d);  // "Power 0/1"
```

It is also possible to receive IR signals with an IR receiver.

This example receives IR signals in an endless loop and prints the received data in the log:
```C++
    while(1) {
        rmtIr->receiveNecOrPanasonicFrame();
    }
```

## API
The API of the component is located in the include directory ```include/rmt_ir.hpp``` and defines the
C++ class ```RmtIr```.

```RmtIr``` is a Singleton class.

The beginning of the class definition is shown here:
```C++
/* class RmtIr
   Class to implement an IR transmitter / receiver which uses different protocols.

   The original code is taken from the GitHub repository https://github.com/espressif/esp-idf.git
   from file esp-idf/examples/peripherals/rmt/ir_nec_transceiver.

   Source of Singleton class structure - https://stackoverflow.com/a/1008289
   Posted by Loki Astari, modified by community. See post 'Timeline' for change history
   Retrieved 2026-02-01, License - CC BY-SA 4.0
*/

    public:
        static RmtIr& getInstance();
        void setGpioPins( uint16_t txPin,  // GPIO pin for IR transmitter
                          uint16_t rxPin   // GPIO pin for IR receiver
                        );

        void initialize();

        void transmitNecCommandFrame(uint16_t address, uint16_t code);
        void transmitNecRepeatFrame();

        void transmitPanasonicCommandFrame(uint16_t non_saving_bits_1, uint8_t system_code, uint8_t address, uint8_t command);

        void receiveNecOrPanasonicFrame();

        ...
```

# ToDo
Currently only the NEC and the PANASONIC protocols are implemented.

Currently the only tested IR sender is the built-in IR LED in M5 Atom Lite. All configurations are fixed for this environment. They have to be exposed to the class user in later versions.

Currently there is no access to the received IR signals, they are only printed to the log. This has to be changed in a later version.

# License
This component is provided under the Apache 2.0 license.

The work is based on the github repository `espressif/esp-idf`, where this example code is included: `esp-idf/examples/peripherals/rmt/ir_nec_transceiver`
This component is based on this example code. 
