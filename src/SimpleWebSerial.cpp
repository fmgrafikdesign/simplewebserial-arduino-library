/*
  SimpleWebSerial.cpp - Library to easily connect your Arduino to a web application.
  Created by Fabian Mohr, 2020.
  Released into the public domain.
*/

#include "Arduino.h"
#include "Arduino_JSON.h"
#include "SimpleWebSerial.h"

#define ARRAYSIZE(x)  (sizeof(x) / sizeof(x[0]))

SimpleWebSerial::SimpleWebSerial() {
}

void SimpleWebSerial::check() {
    static byte bufferIndex = 0;
    char endMarker = '\n';
    char receivedChar;

    // Read data until endMarker is received or buffer is full
    while (Serial.available() > 0 && parseData == false) {
        receivedChar = Serial.read();

        if (receivedChar != endMarker) {
            receivedChars[bufferIndex] = receivedChar;
            bufferIndex++;
            if (bufferIndex >= BufferSize) {
                this->warn("Exceeded allocated buffer size. Try sending smaller data or increasing BufferSize.");
                bufferIndex = BufferSize - 1;
            }
        }
        // When we receive the endMarker, terminate the string and set parseData to true
        else {
            receivedChars[bufferIndex] = '\0';
            parseData = true;
            bufferIndex = 0;
        }
    }

    if (parseData) {
        parseData = false;
        // It is to be assumed that receivedChars will be stringified JSON once it's complete.
        JSONVar parsed = JSON.parse(receivedChars);

        if (JSON.typeof(parsed) == "undefined") {
            this->warn("Parsing input failed! Malformed data sent or buffer overflow.");
            return;
        }

        // Find out if it's an named event
        bool namedEvent = false;
        int isPureData = strcmp("_d", (const char *) parsed[0]);
        int isPureEvent = strcmp("_e", (const char *) parsed[0]);
        if(isPureData == 0) {
            parsed[0] = "data";
        }

        if(isPureEvent == 0) {
            parsed[0] = (const char*)parsed[1];
            parsed[1] = nullptr;
        }

        if (parsed.length() > 1 && JSON.typeof(parsed[0]) == "string") {
            for (int i = 0; i < ARRAYSIZE(eventNames); i++) {
                if (eventNames[i][0] == '\0') continue;

                int compare_result = strcmp(eventNames[i], (const char *) parsed[0]);
                if (compare_result == 0) {
                    (*callbacks[i])(parsed[1]);

                    namedEvent = true;
                    break;
                }
            }
            if (!namedEvent) {
                Serial.print("Could not find an event named '");
                Serial.print(parsed[0]);
                Serial.println("' in the registered eventNames");
            }
        } else if (parsed.length() > 1 && JSON.typeof(parsed[0]) != "string"){
            this->warn("Received array has more than 1 element but its first element is not a string. Malformed data?");
        } else {
            Serial.println("Received array has only 1 element.");
        }
    }

}

void SimpleWebSerial::on(const char *name, void (*callback)(JSONVar data)) {
    strcpy(eventNames[_index], name);
    callbacks[_index] = callback;

    Serial.print(eventNames[_index]);
    Serial.print(" has been defined as an event with index ");
    Serial.println(_index);

    _index++;
}

void SimpleWebSerial::listEvents() {
    Serial.println("Listing registered events:");
    for (int i = 0; i < ARRAYSIZE(eventNames); i++) {
        if (eventNames[i][0] == '\0') continue;

        Serial.print("- ");
        Serial.println(eventNames[i]);
    }
}

void SimpleWebSerial::sendData(JSONVar data) {
    this->send("_d", data);
}

void SimpleWebSerial::sendEvent(const char *name) {
    Serial.println(JSON.stringify(name));
}

void SimpleWebSerial::send(const char *name, JSONVar data) {
    JSONVar event;
    event[0] = name;
    event[1] = data;
    Serial.println(JSON.stringify(event));
}


void SimpleWebSerial::setCallback(void (*callback)()) {
    _callback = callback;
}

void SimpleWebSerial::warn(const char *message) {
    this->send("_w", message);
}

void SimpleWebSerial::log(const char *message) {
    this->send("_l", message);
}

void SimpleWebSerial::error(const char *message) {
    this->send("_e", message);
}