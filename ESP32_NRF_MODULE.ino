void beep(int duration);
bool button(int ch);
void blinkIndicator(int pin, int offDuration);

#include <freertos/FreeRTOS.h>
#include <freertos/task.h>

TaskHandle_t txTask, rxTask;
SemaphoreHandle_t xSerialMutex;

const float conversionFactor = (60.0 / (2 * PI * 0.08));

#include <RF24.h>
#include <SPI.h>

#define CE_PIN 4
#define CSN_PIN 5

bool radioNumber = 1;
const uint8_t pipes[][6] = { "1Node", "2Node" };

RF24 radio(CE_PIN, CSN_PIN);


struct __attribute__((packed)) Struct1 { // test data structure
    uint8_t seq; // sequence 
    uint8_t counterPoint;
    uint8_t id; // node ID 
    int16_t setRPM[4];
    int16_t currentRPM[4];
    float heading;
    float xPos;
    float yPos;
    uint8_t crc; // CRC check
} data, lastData;

// test a simple checksum 
unsigned char checksum(unsigned char* data, uint8_t* crc)
{
    unsigned char sum = 0;
    while (data != crc) {
        sum += *data;
        data++;
    }
    return sum;
}

// char receivedString[256];
// uint8_t receivedData[512];

bool newData = false;
String receivedData = "";

#include "inout.h"
#include "taskRtos.h"

void setup()
{
    Serial.begin(115200);

    radio.begin();
    if (radio.isChipConnected()) {

    }
        // Serial.println("\n\nReceiver NF24 connected to SPI");
    else {
        // Serial.println("\n\nNF24 is NOT connected to SPI");
        while (1)
            ;
    }
    radio.setChannel(125);
    // radio.setDataRate(RF24_1MBPS);
    radio.setDataRate(RF24_250KBPS);
    radio.printDetails();
    if (!radioNumber) {
        radio.openWritingPipe(pipes[0]);
        radio.openReadingPipe(1, pipes[1]);
    } else {
        radio.openWritingPipe(pipes[1]);
        radio.openReadingPipe(1, pipes[0]);
    }
    radio.startListening();
    // Serial.print("data sizeof (bytes):  "); // print size data 
    // Serial.println(sizeof(Struct1));
    radio.setPayloadSize(sizeof(Struct1));

    setupInputOutput();

    lastData.counterPoint = -1;

    beep(100);

    xSerialMutex = xSemaphoreCreateMutex();

    // xTaskCreatePinnedToCore(parseTaskFunction, "transmitter task", 1024, NULL, 1, &txTask, 0);

    xTaskCreatePinnedToCore(rxTaskFunction, "receiver task", 2048, NULL, 1, &rxTask, 1);
}

void loop() { }
