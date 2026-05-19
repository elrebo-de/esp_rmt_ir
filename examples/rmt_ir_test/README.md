## RMT IR Example

This example demonstrates how to utilize the `rmt_ir` component to receive/transmit remote control IR signals.

A button click switches on/off a YAMAHA audio receiver and a PANASONIC television for watching TV (Receiver: AV1, TV: TV).

A button double click switches on/off a YAMAHA audioreceiver and a PANASONIC television for watching Apple TV (Receiver: HDMI1, TV: AV:HDMI1).

A button long click switches on/off the tuner of a YAMAHA audio receiver for listening to the radio.

## Hardware

* An M5 ATOM LITE Esp32 SoC with IR sender on GPIO 12 and onBoard button on GPIO 39.
* An IR sensor connected to the GPIO pin 26 is needed to receive IR signals.

## Build and Flash

Build the project and flash it to the board, then run the monitor tool to view the serial output:

* Run `source <directory with ESP-IDF version>/export.sh` to set IDF environment
* Run `idf.py set-target esp32` to set target chip ESP32
* Run `idf.py -p PORT flash monitor` to build, flash and monitor the project

(To exit the serial monitor, type `Ctrl-]` (or on a Mac `Ctrl-Option-6`).

See the Getting Started Guide for all the steps to configure and use the ESP-IDF to build projects.

## Function
The IR receiver and transmitter is initialized and a button is defined.
Two callback functions are defined for the events
* BUTTON_SINGLE_CLICK,
* BUTTON_DOUBLE_CLICK and
* BUTTON_LONG_PRESS_START with press_time 1000 msec.

### Single Click
In case of a single_click two NEC command frames are sent to the YAMAHA audio receiver and two PANASONIC command frames are sent to the PANASONIC TV.

#### Example Output

```
I (19201) onBoardButton Callback: for Event BUTTON_SINGLE_CLICK called!
I (19201) button: BUTTON_SINGLE_CLICK
I (19201) rmt ir test: state = 1
I (19201) NecProtocol: Prepare a NEC command frame address=7A, code=03
I (19211) NecProtocol: Transmit a NEC command frame address=7A85, code=03FC
I (24211) PanasonicProtocol: Transmit a PANASONIC command frame non_saving_bits_1=4004, system_code=01, address=00, command=BC, checksum=BD
I (32211) PanasonicProtocol: Transmit a PANASONIC command frame non_saving_bits_1=4004, system_code=01, address=40, command=0C, checksum=4D
```

### Double Click
In case of a double click two NEC command frames are sent to the YAMAHA audio receiver and three PANASONIC command frames are sent to the PANASONIC TV.

#### Example Output

```
I (318541) onBoardButton Callback: for Event BUTTON_DOUBLE_CLICK called!
I (318541) button: BUTTON_DOUBLE_CLICK
I (318541) rmt ir test: state = 1
I (318551) NecProtocol: Prepare a NEC command frame address=7A, code=09
I (318551) NecProtocol: Transmit a NEC command frame address=7A85, code=09F6
I (323561) PanasonicProtocol: Transmit a PANASONIC command frame non_saving_bits_1=4004, system_code=01, address=00, command=BC, checksum=BD
I (328561) PanasonicProtocol: Transmit a PANASONIC command frame non_saving_bits_1=4004, system_code=01, address=00, command=A0, checksum=A1
I (333561) PanasonicProtocol: Transmit a PANASONIC command frame non_saving_bits_1=4004, system_code=01, address=00, command=92, checksum=93
```

### Long Click
In case of a long click two NEC command frames are sent to the YAMAHA audio receiver and three PANASONIC command frames are sent to the PANASONIC TV.

#### Example Output

```
I (660771) onBoardButton Callback: for Event BUTTON_LONG_PRESS_START_1000 called!
I (660771) button: BUTTON_LONG_PRESS_START
I (660771) rmt ir test: state = 1
I (660781) NecProtocol: Prepare a NEC command frame address=7E, code=2A
I (660781) NecProtocol: Transmit a NEC command frame address=7E81, code=2AD5
I (662791) NecProtocol: Prepare a NEC command frame address=7A, code=16
I (662791) NecProtocol: Transmit a NEC command frame address=7A85, code=16E9
```

### Receiving IR Signals
If an IR signal is received, it is printed on the log:

Here first a NEC frame and then a PANASONIC frame are received:
#### Example Output
```
I (317941) RmtIr: wait for RX done signal
NEC or PANASONIC frame start---
{0:9043},{1:4457}
{0:587},{1:546}
{0:586},{1:1650}
{0:570},{1:559}
{0:588},{1:1654}
{0:588},{1:1669}
{0:614},{1:1629}
{0:589},{1:1668}
{0:594},{1:519}
{0:625},{1:1629}
{0:623},{1:494}
{0:602},{1:1650}
{0:548},{1:569}
{0:629},{1:491}
{0:585},{1:550}
{0:608},{1:497}
{0:560},{1:1700}
{0:587},{1:1653}
{0:563},{1:564}
{0:594},{1:519}
{0:601},{1:1652}
{0:624},{1:493}
{0:602},{1:520}
{0:558},{1:561}
{0:625},{1:494}
{0:584},{1:547}
{0:585},{1:1653}
{0:562},{1:1700}
{0:585},{1:523}
{0:594},{1:1663}
{0:593},{1:1651}
{0:594},{1:1662}
{0:617},{1:1627}
{0:622},{1:0}
---NEC or PANASONIC frame end: symbol_num=34
NEC Address=7A85, Command=09F6

I (319701) rmt ir test: receiveRmtFrame
I (320701) RmtIr: wait for RX done signal
NEC or PANASONIC frame start---
{0:3406},{1:1673}
{0:450},{1:393}
{0:442},{1:1239}
{0:456},{1:386}
{0:480},{1:366}
{0:449},{1:392}
{0:439},{1:388}
{0:421},{1:419}
{0:422},{1:418}
{0:447},{1:392}
{0:444},{1:396}
{0:414},{1:427}
{0:486},{1:353}
{0:463},{1:379}
{0:455},{1:1234}
{0:413},{1:427}
{0:484},{1:357}
{0:460},{1:379}
{0:457},{1:385}
{0:457},{1:384}
{0:459},{1:384}
{0:456},{1:387}
{0:453},{1:390}
{0:440},{1:388}
{0:423},{1:1272}
{0:454},{1:389}
{0:454},{1:391}
{0:442},{1:385}
{0:421},{1:419}
{0:419},{1:422}
{0:421},{1:417}
{0:445},{1:396}
{0:409},{1:431}
{0:483},{1:1206}
{0:449},{1:392}
{0:421},{1:1269}
{0:423},{1:1272}
{0:456},{1:1229}
{0:455},{1:1234}
{0:415},{1:425}
{0:484},{1:356}
{0:461},{1:1230}
{0:444},{1:395}
{0:416},{1:1275}
{0:421},{1:1279}
{0:450},{1:1233}
{0:455},{1:1231}
{0:514},{1:326}
{0:452},{1:1238}
{0:420},{1:0}
---NEC or PANASONIC frame end: symbol_num=50
PANASONIC non_saving_bits_1=4004,
                system_code=01,
                    address=00,
                    command=BC,
                   checksum=BD,
        calculated checksum=BD

I (324731) rmt ir test: receiveRmtFrame
I (325731) RmtIr: wait for RX done signal
```
