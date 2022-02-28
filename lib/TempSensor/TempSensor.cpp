#include "TempSensor.h"

TempSensor::TempSensor(uint8_t analogPin)
{
  pin = analogPin;
}

void TempSensor::getReading()
{
  float total = 0.0;
  for (int i = 0; i < numSamples; i++)
  {
    total += analogRead(pin);
    delay(10);
  }
  analogReading = total / numSamples;
  calcRes();
  calcTemp();
}

void TempSensor::sendSensorLog()
{
  createQuery("post", temp, type);
  sendQuery();
}

void TempSensor::getInterval()
{
  createQuery("get", systemId, "sensor_interval");
  sendQuery();
  String response = getResponse();
  if (response != "")
  {
    int intervalInt = response.toInt();
    interval = long(intervalInt);
  }
}

void TempSensor::calcRes()
{
  resistance = seriesResistor / ((1023 / analogReading) - 1);
}

void TempSensor::calcTemp()
{
  // T = 1 / (1/T0 + 1/bco * ln(R/R0))
  temp = resistance / thermRes;
  temp = log(temp);
  temp /= bco;
  temp += 1.0 / (tempNom + 273.15);
  temp = 1.0 / temp;
  temp -= 273.15;
}