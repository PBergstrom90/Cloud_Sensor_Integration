#ifndef SENSOR_H
#define SENSOR_H

#include <DHT.h>
#include <Adafruit_Sensor.h>
#include "connect.h"

class Connect;

class Sensor {
private:
    DHT dht;
public: 
    Sensor(uint8_t pin, uint8_t type);
    void readData(float &temperature, float &humidity);
    void processAndSendData(Connect &connect, float &temperature, float &humidity);
    void printData(float &temperature, float &humidity);
};

#endif