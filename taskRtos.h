void rxTaskFunction(void* pvParameters)
{
    while (1) {
        static uint8_t seq_check = 0;
        static int crcErrors = 0, seqErrors = 0, frames = 0;

        if (radio.available()) {
            while (radio.available()) {
                int length = radio.getPayloadSize();
                // Serial.print("received ");
                // Serial.print(length);
                // cek packet size
                if (length != sizeof(data)) {
                    // Serial.println(" bytes - invalid packet size!");
                    return;
                }
                radio.read(&data, sizeof(data));

                // Thresholds untuk perubahan jarak sebagai trigger update Serial
                float positionThreshold = 0.05;  
                float headingThreshold = 0.5; 

                // Check perubahan berdasarkan Thresholds
                bool positionChanged = (abs(data.xPos - lastData.xPos) > positionThreshold)
                    || (abs(data.yPos - lastData.yPos) > positionThreshold);
                bool headingChanged = abs(data.heading - lastData.heading) > headingThreshold;
                bool counterPointChanged = data.counterPoint != lastData.counterPoint;

                // Send data if the position, heading, or counter point has changed
                if (positionChanged) {
                    Serial.print(data.xPos, 2), Serial.print(",");
                    Serial.print(data.yPos, 2), Serial.print(",");
                    Serial.print(data.heading, 2), Serial.print(",");
                    Serial.println(data.counterPoint);

                    // Update the last data
                    lastData.xPos = data.xPos;
                    lastData.yPos = data.yPos;
                    lastData.heading = data.heading;
                    lastData.counterPoint = data.counterPoint;
                }

                // for (int i = 0; i < 4; i++) {
                //     Serial.print(data.setRPM[i] / conversionFactor);
                //     Serial.print(",");
                // }

                // for (int i = 0; i < 4; i++) {
                //     Serial.print(data.currentRPM[i] / conversionFactor);
                //     if (i < 3) {
                //         Serial.print(",");
                //     }
                // }
                // Serial.println();

                if (checksum((unsigned char*)&data, &data.crc) == data.crc) {
                    // Serial.print(" CRC OK  ");
                } else {
                    // Serial.print(" CRC ERROR!  ");
                    crcErrors++;
                }
                // Serial.print(" errors ");
                // Serial.print(crcErrors);
                // Serial.print(" seq:   ");
                // Serial.print(data.seq);
                if (seq_check == data.seq) {

                }
                // Serial.print(" seq OK  - ");
                else {
                    // Serial.print(" seq error! expected ");
                    // Serial.print(seq_check);
                    seqErrors++;
                }
                // Serial.print(" - seq errors ");
                // Serial.println(seqErrors);
                seq_check = ++data.seq;
            }
            vTaskDelay(100 / portTICK_PERIOD_MS); // menghindari overload di Matlab dan Serial monitor
        }
    }
}

// void parseData(String data)
// {
//     int startPos, endPos;
//     String values;

//     // Parse integer data
//     if (data.indexOf('<') != -1 && data.indexOf('>') != -1) {
//         startPos = data.indexOf('<') + 1;
//         endPos = data.indexOf('>');
//         values = data.substring(startPos, endPos);
//         int index = 0;
//         char* ptr = strtok((char*)values.c_str(), ",");
//         while (ptr != NULL && index < 8) {
//             if (index < 4) {
//                 // Serial.print("setRPM: ");
//             } else {
//                 // Serial.print("currentRPM: ");
//             }
//             // Serial.println(ptr);
//             ptr = strtok(NULL, ",");
//             index++;
//         }
//     }

//     // Parse float data
//     if (data.indexOf('[') != -1 && data.indexOf(']') != -1) {
//         startPos = data.indexOf('[') + 1;
//         endPos = data.indexOf(']');
//         values = data.substring(startPos, endPos);
//         int index = 0;
//         char* ptr = strtok((char*)values.c_str(), ",");
//         while (ptr != NULL && index < 7) {
//             if (index < 3) {
//                 Serial.print("xRobot: ");
//             } else if (index < 4) {
//                 Serial.print("yRobot: ");
//             } else if (index < 5) {
//                 Serial.print("zRobot: ");
//             } else if (index < 6) {
//                 Serial.print("psi: ");
//             } else {
//                 Serial.print("errorHeading: ");
//             }
//             Serial.println(ptr);
//             ptr = strtok(NULL, ",");
//             index++;
//         }
//     }
// }

// void parseTaskFunction(void* pvParameters)
// {
//     while (1) {
//         if (newData) {
//             if (xSemaphoreTake(xSerialMutex, portMAX_DELAY) == pdTRUE) {
//                 parseData(receivedData);
//                 receivedData = ""; // Clear the buffer
//                 newData = false; // Clear the flag after parsing the data
//                 xSemaphoreGive(xSerialMutex);
//             }
//         }
//         vTaskDelay(100 / portTICK_PERIOD_MS); // Adjust as necessary
//     }
// }

// void displayTask(void* pvParameters)
// {
//     while (1) {
//         if (newData) {
//             parseData(receivedData);
//             Serial.println("Data received:");

//             Serial.print("setRPM: ");
//             for (int i = 0; i < 4; i++) {
//                 Serial.print(setRPM[i]);
//                 Serial.print(" ");
//             }
//             Serial.println();

//             Serial.print("currentRPM: ");
//             for (int i = 0; i < 4; i++) {
//                 Serial.print(currentRPM[i]);
//                 Serial.print(" ");
//             }
//             Serial.println();

//             Serial.print("setVmotor: ");
//             for (int i = 0; i < 4; i++) {
//                 Serial.print(setVmotor[i]);
//                 Serial.print(" ");
//             }
//             Serial.println();

//             Serial.print("currentVmotor: ");
//             for (int i = 0; i < 4; i++) {
//                 Serial.print(currentVmotor[i]);
//                 Serial.print(" ");
//             }
//             Serial.println();

//             Serial.print("xRobot: ");
//             Serial.println(xRobot);

//             Serial.print("yRobot: ");
//             Serial.println(yRobot);

//             Serial.print("zRobot: ");
//             Serial.println(zRobot);

//             receivedData = ""; // Clear the buffer
//             newData = false; // Clear the flag after displaying the data
//         }
//         vTaskDelay(100 / portTICK_PERIOD_MS); // Display task runs every 100ms
//     }
// }