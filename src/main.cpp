#include "PhSensor.h"
#include "EcSensor.h"
#include "TempSensor.h"
#include "Relay.h"
#include <RTClib.h>

PhSensor phSensor(A0);
EcSensor ecSensor;
TempSensor tempSensor(A2);
Relay phDownPump(51, "ph");
Relay ecUpPump(49, "ec");
Relay waterPump(25, "water");
Relay light(23, "light");
RTC_DS3231 rtc;
DateTime lastRefresh;
int refreshParams = 300;

void setup()
{
  Serial.begin(9600);
  Serial3.begin(9600);
  if (!rtc.begin())
  {
    Serial.println("Couldn't find RTC");
    Serial.flush();
    while (1)
      delay(10);
  }
  delay(1000);
  rtc.adjust(DateTime(2022, 1, 1, 1, 0, 0));
  DateTime oldTime = rtc.now() - TimeSpan(10000); // Set older time to make all actions take place on startup
  lastRefresh = oldTime;
  phSensor.lastReading = oldTime;
  ecSensor.lastReading = oldTime;
  tempSensor.lastReading = oldTime;
  phDownPump.lastOff = oldTime;
  phDownPump.lastOn = oldTime;
  ecUpPump.lastOn = oldTime;
  ecUpPump.lastOff = oldTime;
  waterPump.lastOn = oldTime;
  waterPump.lastOff = oldTime;
  phSensor.interval = 300;
  ecSensor.interval = 300;
  tempSensor.interval = 300;
  phDownPump.interval = 1200;
  phDownPump.onTime = 5;
  ecUpPump.interval = 1200;
  ecUpPump.onTime = 6;
  waterPump.interval = 1200;
  waterPump.onTime = 300;
  light.interval = 1200;
  light.onTime = 300;
}

bool isPassedInterval(int interval, DateTime lastReading)
{
  DateTime nextTime = lastReading + TimeSpan(interval);
  return (nextTime < rtc.now());
}

void updateParams()
{
  if (isPassedInterval(refreshParams, lastRefresh))
  {
    phSensor.getInterval();
    phSensor.getMaxPh();
    ecSensor.getInterval();
    ecSensor.getMinEc();
    tempSensor.getInterval();
    phDownPump.getInterval();
    phDownPump.getOnTime();
    ecUpPump.getInterval();
    ecUpPump.getOnTime();
    waterPump.getInterval();
    waterPump.getOnTime();
    lastRefresh = rtc.now();
  }
}

void runPhDownPump()
{
  if (isPassedInterval(phDownPump.interval, phDownPump.lastOff))
  {
    if (phSensor.isAboveRange())
    {
      phDownPump.On();
      phDownPump.sendTaskLog(1);
      phDownPump.lastOn = rtc.now();
      delay(phDownPump.onTime * 1000);
      phDownPump.Off();
      phDownPump.sendTaskLog(0);
      phDownPump.lastOff = rtc.now();
    }
  }
}

void runEcUpPump()
{
  if (isPassedInterval(ecUpPump.interval, ecUpPump.lastOff))
  {
    if (ecSensor.isBelowRange())
    {
      ecUpPump.On();
      ecUpPump.sendTaskLog(1);
      ecUpPump.lastOn = rtc.now();
      delay(ecUpPump.onTime * 1000);
      ecUpPump.Off();
      ecUpPump.sendTaskLog(0);
      ecUpPump.lastOff = rtc.now();
    }
  }
}

void runWaterPump()
{
  if (isPassedInterval(waterPump.interval, waterPump.lastOff))
  {
    if (waterPump.lastOff >= waterPump.lastOn)
    {
      waterPump.On();
      waterPump.sendTaskLog(1);
      waterPump.lastOn = rtc.now();
    }
  }
  if (isPassedInterval(waterPump.onTime, waterPump.lastOn))
  {
    if (waterPump.lastOff < waterPump.lastOn)
    {
      waterPump.Off();
      waterPump.sendTaskLog(0);
      waterPump.lastOff = rtc.now();
    }
  }
}

void runlights()
{
  if (isPassedInterval(light.interval, light.lastOff))
  {
    if (light.lastOff >= light.lastOn)
    {
      light.On();
      light.sendTaskLog(1);
      light.lastOn = rtc.now();
    }
  }
  if (isPassedInterval(light.onTime, light.lastOn))
  {
    if (light.lastOff < light.lastOn)
    {
      light.Off();
      light.sendTaskLog(0);
      light.lastOff = rtc.now();
    }
  }
}

void runPhSensor()
{
  if (isPassedInterval(phSensor.interval, phSensor.lastReading))
  {
    phSensor.getReading();
    phSensor.sendSensorLog();
    phSensor.lastReading = rtc.now();
  }
}

void runEcSensor()
{
  if (isPassedInterval(ecSensor.interval, ecSensor.lastReading))
  {
    ecSensor.getReading();
    ecSensor.sendSensorLog();
    ecSensor.lastReading = rtc.now();
  }
}

void runTempSensor()
{
  if (isPassedInterval(tempSensor.interval, tempSensor.lastReading))
  {
    tempSensor.getReading();
    tempSensor.sendSensorLog();
    tempSensor.lastReading = rtc.now();
  }
}

void loop()
{
  runPhSensor();
  runEcSensor();
  runTempSensor();
  runPhDownPump();
  runEcUpPump();
  runlights();
  runWaterPump();
  updateParams();
  delay(100);
}
