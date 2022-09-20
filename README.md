# CAN Bus Message Echoer Project

### General Information and Acknowledgements

The following project has been created to 'echo' CAN messages from a saved file. Each CAN message is read from a file loaded from the SD card.

The CAN messages must be saved in the format of [HCRL](https://ocslab.hksecurity.net/welcome)'s work on their CAN Dataset for intrusion detection ([OTIDS](https://ocslab.hksecurity.net/Dataset/CAN-intrusion-dataset)). It is formatted in the following way:
```
Timestamp:          [Time since initialised in milliseconds]        ID: [Arbitration or CAN ID]    [If it is a remote request frame (Set to the value of '100']    DLC: [Length of the data package sent]    [The data package sent]     
```

### How to run

The code in this project was developed to run on the [Arduino Uno Rev3](https://store.arduino.cc/products/arduino-uno-rev3) and the [CAN-BUS Shield V2.0](https://wiki.dfrobot.com/CAN-BUS_Shield_V2__SKU__DFR0370_) (which runs on the [MCP2515 Chip](https://ww1.microchip.com/downloads/en/DeviceDoc/MCP2515-Stand-Alone-CAN-Controller-with-SPI-20001801J.pdf)), but any Arduino and CAN-BUS shield that supports the required libraries should work too.

The libraries used in this project include:

- [SD](https://www.arduino.cc/reference/en/libraries/sd/) Version 1.2.4
- [Seeed Arduino CAN](https://github.com/Seeed-Studio/Seeed_Arduino_CAN) Version 2.3.3

To run on your Arduino, make sure you have set the:

- baud
- SPI_CS pin
- CAN_INT pin
- SD_SPI_CS pin

Note that the filename of the read file needs to replace the value of the variable **filename**.  Also, please note that certain Arduino boards might not work with file extensions with more than three characters.

### Development Environment
The IDE [CLion](https://www.jetbrains.com/clion/) was used with the plugins [PlatformIO for CLion](https://plugins.jetbrains.com/plugin/13922-platformio-for-clion) and [Serial Port Monitor](https://plugins.jetbrains.com/plugin/8031-serial-port-monitor).