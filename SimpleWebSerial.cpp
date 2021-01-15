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
            receivedChars[bufferIndex] = '\0'; // terminate the string
            parseData = true;
            // Serial.println(bufferIndex);
            bufferIndex = 0;
        }
    }

    if (parseData) {
        parseData = false;
        // It is to be assumed that receivedChars will be stringified JSON once it's complete.
        JSONVar parsed = JSON.parse(receivedChars);

        // If I comment this out, JSON.typeof(parsed) is never undefined. Why?
        // Serial.println(receivedChars);

        // Why is this only undefined when I print the received Chars? Nobody knows!
        if (JSON.typeof(parsed) == "undefined") {
            this->warn("Parsing input failed! Malformed data sent or buffer overflow.");
            return;
        }

        //Serial.println(JSON.stringify(parsed));
        //Serial.println(JSON.typeof(parsed));

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
            // Serial.println("Received array has more than 1 element and its first element is string. Assuming named event!");

            // Parse object 1 level deep
            /*
            if(JSON.typeof(parsed[1]) == "object") {
                parsed[1] = JSON.parse(parsed[1]);
                Serial.println("Parsed received data because it was an object.");
            }
             */

            for (int i = 0; i < ARRAYSIZE(eventNames); i++) {
                if (eventNames[i][0] == '\0') continue;

                //Serial.print("Comparing ");
                //Serial.print((const char *) parsed[0]);
                //Serial.print(" to ");
                //Serial.println(eventNames[i]);

                int compare_result = strcmp(eventNames[i], (const char *) parsed[0]);
                //Serial.println(compare_result);
                //Serial.print("first character in eventNames:");
                //Serial.println(eventNames[_index][0]);

                //Serial.print("first character in received event:");
                //Serial.println(eventName[0]);

                // Serial.println(compare_result);
                if (compare_result == 0) {
                    /*
                    Serial.print("Found event named ");
                    Serial.print(parsed[0]);
                    Serial.println(" in registered eventNames");
                    Serial.print("Trying to call callback with index ");
                    Serial.println(i);
                     */
                    //Serial.println((*callbacks[i]));

                    // (*callbacks[i])((JSONVar) parsed[1]);
                    // (*_callback)();

                    //Serial.print("Calling callback with parameters: ");
                    //Serial.println(parsed[1]);
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
            //Serial.println("Received array has more than 1 element but its first element is not a string. Malformed data?");
            this->warn("Received array has more than 1 element but its first element is not a string. Malformed data?");
        } else {
            Serial.println("Received array has only 1 element.");
        }

        // If it's a named event, find out its name

        // Loop through registered eventNames

    }

}

void SimpleWebSerial::on(const char *name, void (*callback)(JSONVar data)) {
    // TODO show warning if you're using more events than allowed. Check if you can do that w/ Arduino compiler

    strcpy(eventNames[_index], name);
    callbacks[_index] = callback;

    Serial.print(eventNames[_index]);
    Serial.print(" has been defined as an event with index ");
    Serial.println(_index);

    // callbacks[_index](data);
    _index++;
}

void SimpleWebSerial::listEvents() {
    Serial.println("Listing registered events:");
    for (int i = 0; i < ARRAYSIZE(eventNames); i++) {
        if (eventNames[i][0] == '\0') continue;

        Serial.print("- ");
        Serial.println(eventNames[i]);
    }
    // Serial.println(eventName);
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

void SimpleWebSerial::onData() {
    // look for the next valid integer in the incoming serial stream:

    //(*_callback)();
}

void SimpleWebSerial::warn(const char *message) {
    this->send("_w", message);
}