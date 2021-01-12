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
    while (Serial.available() > 0) {

        // Read data until newline or break character or whatever
        // char buffer[512] = Serial.readStringUntil('\n');

        //DEBUG: Create a fake json we want to receive
        JSONVar testObject;
        JSONVar testArray;

        testObject = true;

        testArray[0] = "values";
        testArray[1] = testObject;

        Serial.println(JSON.stringify(testArray));
        //Serial.println(JSON.typeof(testArray[1]));

        Serial.read();

        // Find out if it's an named event
        bool namedEvent = false;
        if (testArray.length() > 1 && JSON.typeof(testArray[0]) == "string") {
            Serial.println(
                    "Received array has more than 1 element and its first element is string. Assuming named event!");
            for (int i = 0; i < ARRAYSIZE(eventNames); i++) {
                if (eventNames[i][0] == '\0') continue;

                Serial.print("Comparing ");
                Serial.print((const char *) testArray[0]);
                Serial.print(" to ");
                Serial.println(eventNames[i]);

                strncpy(eventName, testArray[0], MaximumEventNameLength);

                int compare_result = strcmp(eventNames[i], eventName);
                Serial.print("first character in eventNames:");
                Serial.println(eventNames[_index][0]);

                Serial.print("first character in received event:");
                Serial.println(eventName[0]);

                Serial.println(compare_result);
                if (compare_result == 0) {
                    Serial.print("Found event named ");
                    Serial.print(eventName);
                    Serial.println(" in registered eventNames");
                    namedEvent = true;
                    break;
                }
            }
            if (!namedEvent) {
                Serial.print("Could not find an event named '");
                Serial.print(eventName);
                Serial.println("' in the registered eventNames");
            }

        } else if (testArray.length() > 1 && JSON.typeof(testArray[0]) != "string"){
            Serial.println(
                    "Received array has more than 1 element but its first element is not a string. Malformed data?");
        } else {
            Serial.println("Received array has only 1 element. Handle pure data");
            // Handle pure data
        }

        // If it's a named event, find out its name

        // Loop through registered eventNames

        //this->onData();
    }
}

void SimpleWebSerial::on(const char *name, void (*callback)()) {
    // TODO show warning if you're using more events than allowed. Check if you can do that w/ Arduino compiler

    Serial.print(name);
    Serial.print(" has been defined as an event with index ");
    Serial.println(_index);

    strcpy(eventNames[_index], name);
    callbacks[_index] = callback;

    int compare_result = strcmp(eventNames[_index], "values");
    Serial.println(eventNames[_index]);
    if (compare_result == 0) {
        Serial.println("Event name equals values!");
    }
    callbacks[_index]();
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

void SimpleWebSerial::send() {

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