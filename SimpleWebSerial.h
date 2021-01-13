/*
  SimpleWebSerial.h - Library to easily connect your Arduino to a web application.
  Created by Fabian Mohr, 2020.
  Released into the public domain.
*/

#ifndef SimpleWebSerial_h
#define SimpleWebSerial_h

// 2 KB ram on uno make 16 events take about a quarter of available ram... hmm.
// If you notice you're using either too much ram or need more/longer event names, adjust these numbers as you see fit.
#ifndef MaximumNumberOfEvents
#define MaximumNumberOfEvents 8
#endif

#ifndef MaximumEventNameLength
#define MaximumEventNameLength 16
#endif

#ifndef SerialBufferSize
#define SerialBufferSize 64
#endif

#include "Arduino.h"
#include "Arduino_JSON.h"

class SimpleWebSerial {
public:
    SimpleWebSerial();
    void check(); // Makes the library check for serial data
    //void on(const char* name, void (*callback)()); // Event name, callback
    void on(const char* name, void (*callback)(JSONVar)); // Event name, callback
    void onData(); // callback
    void send(JSONVar data); // Event name + data or just data
    void send(const char* name, JSONVar data); // Event name + data or just data
    void setCallback(void (*callback)()); // Set callback for debug purposes
    void listEvents(); // DEBUG: List all registered events
private:
    void (*_callback)();
    int _index = 0; // Keeps track on used events

    char eventNames[MaximumNumberOfEvents][MaximumEventNameLength]; // Array of event names
    void (*callbacks[])(JSONVar); // Array of function pointers
    char receivedChars[SerialBufferSize];
    bool parseData = false;
    // char eventName[MaximumEventNameLength];


};

#endif