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
In case of a single_click a NEC command frame is sent to the YAMAHA audio receiver and a PANASONIC command frame is sent to the PANASONIC TV.

#### Example Output

```
I (290) main_task: Calling app_main()
I (790) rmt ir test: Example Program
I (790) rmt ir test: NecIr
I (790) RmtIr: Set GPIO pins
I (790) RmtIr: Initializing NEC IR RMT
I (790) RmtIr: create RMT RX channel
I (790) RmtIr: register RX done callback
I (790) RmtIr: create RMT TX channel
I (800) RmtIr: modulate carrier to TX channel
I (800) RmtIr: enable RMT TX and RX channels
I (800) NecProtocol: install IR NEC encoder
I (810) PanasonicProtocol: install IR PANASONIC encoder
I (810) onBoardButton: Button Type GPIO
I (820) button: IoT Button Version: 4.1.6
I (820) onBoardButton: RegisterCallbackForEvent called
I (830) onBoardButton: RegisterCallbackForEvent called
I (830) rmt ir test: receiveNecOrPanasonicFrame
I (1830) RmtIr: wait for RX done signal
I (2830) RmtIr: wait for RX done signal
I (3830) RmtIr: wait for RX done signal
I (4830) RmtIr: wait for RX done signal
I (5830) RmtIr: wait for RX done signal
I (5950) onBoardButton Callback: for Event BUTTON_SINGLE_CLICK called!
I (5950) button: BUTTON_SINGLE_CLICK
I (5950) NecProtocol: Transmit a NEC command frame address=817E, code=D52A
I (6450) PanasonicProtocol: Transmit a PANASONIC command frame non_saving_bits_1=2002, system_code=80, address=00, command=3D, checksum=BD
I (6830) RmtIr: wait for RX done signal
I (7830) RmtIr: wait for RX done signal
I (8830) RmtIr: wait for RX done signal
I (9830) RmtIr: wait for RX done signal
```

### Double Click
In case of a double click two NEC commands are sent to the YAMAHA audio receiver.

#### Example Output

```
I (28570) RmtIr: wait for RX done signal
I (29570) RmtIr: wait for RX done signal
I (30570) RmtIr: wait for RX done signal
I (31520) onBoardButton Callback: for Event BUTTON_DOUBLE_CLICK called!
I (31520) button: BUTTON_DOUBLE_CLICK
I (31520) NecProtocol: Transmit a NEC command frame address=817E, code=D52A
I (31570) RmtIr: wait for RX done signal
I (32030) NecProtocol: Transmit a NEC command frame address=857A, code=E916
I (32570) RmtIr: wait for RX done signal
I (33570) RmtIr: wait for RX done signal
I (34570) RmtIr: wait for RX done signal
I (35570) RmtIr: wait for RX done signal
```

### Receiving IR Signals
If an IR signal is received, it is printed on the log:

Here first a NEC frame and then a PANASONIC frame are received:
#### Example Output
```
I (4830) RmtIr: wait for RX done signal
NEC or PANASONIC frame start---
{0:9032},{1:4468}
{0:603},{1:516}
{0:586},{1:1679}
{0:609},{1:1626}
{0:586},{1:1664}
{0:624},{1:1626}
{0:586},{1:1677}
{0:614},{1:1624}
{0:588},{1:540}
{0:591},{1:1651}
{0:588},{1:539}
{0:594},{1:519}
{0:604},{1:520}
{0:581},{1:534}
{0:604},{1:518}
{0:558},{1:575}
{0:585},{1:1651}
{0:587},{1:544}
{0:589},{1:1650}
{0:590},{1:537}
{0:589},{1:1653}
{0:600},{1:525}
{0:623},{1:1623}
{0:604},{1:518}
{0:583},{1:534}
{0:603},{1:1649}
{0:557},{1:561}
{0:628},{1:1623}
{0:583},{1:550}
{0:586},{1:1651}
{0:588},{1:540}
{0:586},{1:1656}
{0:591},{1:1668}
{0:613},{1:0}
---NEC or PANASONIC frame end: symbol_num=34
NEC Address=817E, Command=D52A

I (5210) rmt ir test: receiveNecOrPanasonicFrame
NEC or PANASONIC frame start---
{0:3435},{1:1622}
{0:441},{1:400}
{0:408},{1:1281}
{0:449},{1:392}
{0:442},{1:399}
{0:418},{1:422}
{0:407},{1:431}
{0:487},{1:354}
{0:487},{1:353}
{0:462},{1:381}
{0:481},{1:359}
{0:459},{1:384}
{0:482},{1:359}
{0:483},{1:362}
{0:479},{1:1202}
{0:482},{1:360}
{0:460},{1:383}
{0:480},{1:365}
{0:450},{1:376}
{0:454},{1:387}
{0:448},{1:392}
{0:446},{1:394}
{0:419},{1:421}
{0:407},{1:434}
{0:482},{1:1207}
{0:446},{1:396}
{0:416},{1:423}
{0:406},{1:433}
{0:457},{1:383}
{0:486},{1:354}
{0:485},{1:355}
{0:484},{1:358}
{0:482},{1:360}
{0:459},{1:1226}
{0:483},{1:359}
{0:485},{1:1202}
{0:484},{1:1207}
{0:444},{1:1258}
{0:451},{1:1231}
{0:459},{1:383}
{0:484},{1:359}
{0:456},{1:1227}
{0:486},{1:357}
{0:483},{1:1201}
{0:486},{1:1206}
{0:418},{1:1272}
{0:453},{1:1242}
{0:458},{1:384}
{0:485},{1:1200}
{0:482},{1:0}
---NEC or PANASONIC frame end: symbol_num=50
PANASONIC non_saving_bits_1=2002,
system_code=80,
address=00,
command=3D,
checksum=BD,
calculated checksum=BD

I (5740) rmt ir test: receiveNecOrPanasonicFrame
I (6740) RmtIr: wait for RX done signal
I (7740) RmtIr: wait for RX done signal
```
