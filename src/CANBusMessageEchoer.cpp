//
// Created by Tompnyx on 09/08/2022.
//
#include <Arduino.h>
#include <mcp2515_can.h>
#include <SD.h>

// CONSTANTS ==================================================================
// The baud rate of the board
#define BAUD 115200
// The SPI CS Pin of the Arduino
#define SPI_CS_PIN 10
// The CS Int Pin of the Arduino
#define CAN_INT_PIN 2
// The SD SPI CS PIN of the Arduino (If your Arduino or CAN-BUS shield has SD
// card capabilities
#define SD_SPI_CS_PIN 4

// PUBLIC VARIABLES ===========================================================
// Set CS pin
mcp2515_can CAN(SPI_CS_PIN);

// The filename of the CAN bus data
const char *filename = "AFD.txt";
// The file object the file will be opened to
File myFile;
// The time the device started
unsigned long timeStart;

// ARDUINO FUNCTIONS ==========================================================

/**
 * Checks to see if the CAN message was sent correctly
 *
 * @param report The return integer from the sendMsgBuffer function
 * @return If the message was sent correctly or not
 */
bool check_message_sent(int report) {
    if (report == CAN_SENDMSGTIMEOUT) {
        // CAN_SENDMSGTIMEOUT: A timeout has occurred
        SERIAL_PORT_MONITOR.println("A CAN_SENDMSGTIMEOUT has occurred");
    } else if (report == CAN_GETTXBFTIMEOUT) {
        // CAN_GETTXBFTIMEOUT: The program has failed to get the next free
        // buffer. This has most likely occurred due to the buffer being full.
        SERIAL_PORT_MONITOR.println("A CAN_GETTXBFTIMEOUT has occurred");
    } else {
        // CAN_OK: everything is working
        return true;
    }
    return false;
}

/**
 * Converts a hexadecimal value to its binary equivalent
 *
 * @param aChar The character to convert
 * @return The binary value returned in the form of an int
 */
uint8_t hex2bin(char aChar) {
    if (aChar >= '0' && aChar <= '9') return aChar - '0';
    else if (aChar >= 'a' && aChar <= 'f') return aChar - 'a' + 10;
    else if (aChar >= 'A' && aChar <= 'F') return aChar - 'A' + 10;
    else return 0;
}

void setup() {
    // Initialise the board at the specified baud
    SERIAL_PORT_MONITOR.begin(BAUD);
    // Wait for a serial connection
    while (!SERIAL_PORT_MONITOR);
    // Initialise the CAN board at the specified speed
    if (CAN_OK != CAN.begin(CAN_500KBPS)) {
        SERIAL_PORT_MONITOR.println("CAN init fail");
        SERIAL_PORT_MONITOR.flush();
        return;
    }
    SERIAL_PORT_MONITOR.println("CAN init ok!");
    // init the sd card
    if (!SD.begin(SD_SPI_CS_PIN)) {
        SERIAL_PORT_MONITOR.println("SD init fail");
        SERIAL_PORT_MONITOR.flush();
        return;
    }
    SERIAL_PORT_MONITOR.println("SD init ok!");

    // Open the file
    myFile = SD.open(filename, FILE_READ);

    // Test reading the file
    SERIAL_PORT_MONITOR.println("Starting file reading:\n");
    SERIAL_PORT_MONITOR.flush();

    if (myFile) {

        // Records the time the device started looping
        // NOTE: This will reset to zero after approximately 70 minutes
        timeStart = micros();

        while (myFile.available()) {
            // Reads the next line of the file
            String sdRead = myFile.readStringUntil('\n');
            SERIAL_PORT_MONITOR.println(sdRead);
            SERIAL_PORT_MONITOR.flush();

            // Extract the needed information from the read line

            // Operation to remove the '.' character from the time
            String timeString = sdRead.substring(20, 28);
            timeString.remove(1, 1);
            // The time the message was sent
            unsigned long time = strtoul(timeString.c_str(), nullptr, 16);

            // The CAN ID of the message
            unsigned long id = strtoul(sdRead.substring(40, 44).c_str(), nullptr, 16);
            // The RTR bit (RTR is the Remote Request Frame)
            byte rtr = strtoul(sdRead.substring(48, 51).c_str(), nullptr, 16);
            // The length of the payload (or DLC)
            byte len = strtoul(sdRead.substring(60, 61).c_str(), nullptr, 16);
            // The payload of the message (in hexadecimal)
            uint8_t payload[len];
            // Counter for the byte index in the payload
            int counter = 0;

            // For loop that iterates through every hex byte (2 hex characters)
            // and converts that hex value to binary to be put into the payload
            for (uint8_t i = 0; i < sdRead.substring(65).length(); i = i + 3) {
                payload[counter] = hex2bin(sdRead.charAt(65 + i)) << 4;
                payload[counter] |= hex2bin(sdRead.charAt(66 + i));
                counter++;
            }

            // Wait until the correct time is reached to send the message and
            // then send the related CAN message
            while ((millis() - timeStart) < time);
            int report = CAN.sendMsgBuf(id, 0, rtr, len, payload);

            // Check to see if the message was sent correctly and exit if not
            if (!check_message_sent(report)) {
                SERIAL_PORT_MONITOR.flush();
                return;
            }
        }
    } else {
        // There was an error with the file opening
        SERIAL_PORT_MONITOR.print("Error opening ");
        SERIAL_PORT_MONITOR.println(filename);
    }

    // Close the file
    myFile.close();
}

void loop() {
    // Nothing is needed after setup
}