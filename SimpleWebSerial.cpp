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
    //Serial.println("Library instance created!");
/*
    // Initialize events
    for (int i = 0; i < MaximumNumberOfEvents; i++) {
        eventNames[i] = "Hello";
    }
    */
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
            if (bufferIndex >= SerialBufferSize) {
                bufferIndex = SerialBufferSize - 1;
            }
        }
        // When we receive the endMarker, terminate the string and set parseData to true
        else {
            receivedChars[bufferIndex] = '\0'; // terminate the string
            parseData = true;
            bufferIndex = 0;
        }
    }

    if (parseData) {

        // It is to be assumed that receivedChars will be stringified JSON once it's complete.
        Serial.print("This just in ... ");
        Serial.println(receivedChars);
        JSONVar parsed = JSON.parse(receivedChars);

        // JSON.typeof(jsonVar) can be used to get the type of the var
        /*
        if (JSON.typeof(parsed) == "undefined") {
            Serial.println("Parsing input failed! Malformed data sent or buffer overflow.");
            return;
        }
*/

        Serial.println(JSON.stringify(parsed));
        Serial.println(JSON.typeof(parsed));
        parseData = false;

        // Find out if it's an named event
        bool namedEvent = false;
        if (parsed.length() > 1 && JSON.typeof(parsed[0]) == "string") {
            Serial.println(
                    "Received array has more than 1 element and its first element is string. Assuming named event!");
            for (int i = 0; i < ARRAYSIZE(eventNames); i++) {
                if (eventNames[i][0] == '\0') continue;

                //Serial.print("Comparing ");
                //Serial.print((const char *) parsed[0]);
                //Serial.print(" to ");
                //Serial.println(eventNames[i]);

                int compare_result = strcmp(eventNames[i], (const char *)parsed[0]);
                //Serial.println(compare_result);
                //Serial.print("first character in eventNames:");
                //Serial.println(eventNames[_index][0]);

                //Serial.print("first character in received event:");
                //Serial.println(eventName[0]);

                // Serial.println(compare_result);
                if (compare_result == 0) {
                    Serial.print("Found event named ");
                    Serial.print(parsed[0]);
                    Serial.println(" in registered eventNames");
                    Serial.println(parsed[1]);

                    // (*callbacks[i])((JSONVar) parsed[1]);

                    namedEvent = true;
                    // break;
                }
            }
            if (!namedEvent) {
                Serial.print("Could not find an event named '");
                Serial.print(parsed[0]);
                Serial.println("' in the registered eventNames");
            }

        } else if (parsed.length() > 1 && JSON.typeof(parsed[0]) != "string"){
            Serial.println(
                    "Received array has more than 1 element but its first element is not a string. Malformed data?");
        } else {
            Serial.println("Received array has only 1 element. Handle pure data");
            // TODO Handle pure data
        }

        // If it's a named event, find out its name

        // Loop through registered eventNames

    }

}

void SimpleWebSerial::on(const char *name, void (*callback)(JSONVar)) {
    // TODO show warning if you're using more events than allowed. Check if you can do that w/ Arduino compiler

    strcpy(eventNames[_index], name);
    callbacks[_index] = callback;

    Serial.print(eventNames[_index]);
    Serial.print(" has been defined as an event with index ");
    Serial.println(_index);

    // callbacks[_index]();
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

void SimpleWebSerial::send(JSONVar data) {
    Serial.println(JSON.stringify(data));
}

void SimpleWebSerial::send(const char* name, JSONVar data) {
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

    /*
    int red = Serial.parseInt();
    int green = Serial.parseInt();
    int blue = Serial.parseInt();

    if (Serial.read() == '\n') {

        Serial.print("#");
        Serial.print(red, DEC);
        Serial.print(green, DEC);
        Serial.println(blue, DEC);
        // Serial.println(Serial.read());
    }
     */
    Serial.println(Serial.readStringUntil('\n'));

    (*_callback)();
}