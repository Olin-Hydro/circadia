#include "ECSensor.h"
#include "Arduino.h"

ECSensor::ECSensor() {
    ;
}

bool ECSensor::aboveRange() {
    this->getReading();
    return this->reading > this->minEC;
}

void ECSensor::getReading() {
    //this -> reading.reserve(30);
    //this -> reading = Serial3.readStringUntil(13);
    this -> reading = 1.56;
}

void ECSensor::sendSensorLog() {
    getReading();
    String res = sendData(this -> reading, this -> type);
}

void ECSensor::getMinEC() {
    String response = getData(this->wrapper->systemId, "ec_low");
    if (response != "") {
        this->minEC = response.toDouble();
    }
}

void ECSensor::getInterval() {
    String response = getData(this->wrapper->systemId, "sensor_interval");
    if (response != "") {
        int intervalInt = response.toInt();
        this->interval = long(intervalInt);
    }
}