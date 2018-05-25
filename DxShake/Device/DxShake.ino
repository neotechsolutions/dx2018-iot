#include "AZ3166WiFi.h"
#include "DevKitMQTTClient.h"
#include "LSM6DSLSensor.h"

static bool hasWifi = false;
static bool hasIoTHub = false;

static DevI2C *ext_i2c;
static LSM6DSLSensor *acc_gyro;

static bool InitWifi()
{
  Screen.print(1, "Wifi...");

  if (WiFi.begin() == WL_CONNECTED)
  {
    hasWifi = true;
    Screen.print(1, "Wifi Ok !");
  }
  else
  {
    hasWifi = false;
    Screen.print(1, "Wifi Ko :(");
  }

  return hasWifi;
}

static bool InitIotHub()
{
  Screen.print(2, "IoT Hub...");
  DevKitMQTTClient_SetOption(OPTION_MINI_SOLUTION_NAME, "GetStarted");
  if (DevKitMQTTClient_Init(true))
  {
    hasIoTHub = true;
    Screen.print(2, "IoT Hub Ok !");
  }
  else
  {
    hasIoTHub = false;
    Screen.print(2, "IoT Hub Ko :(");
  }

  return hasIoTHub;
}

void InitSensor()
{
  Screen.print(3, "Sensor...");
  ext_i2c = new DevI2C(D14, D15);
  acc_gyro = new LSM6DSLSensor(*ext_i2c, D4, D5);
  acc_gyro->init(NULL);
  acc_gyro->enableAccelerator();
  acc_gyro->enableGyroscope();
  acc_gyro->enablePedometer();
  acc_gyro->setPedometerThreshold(LSM6DSL_PEDOMETER_THRESHOLD_MID_LOW);
  Screen.print(3, "Sensor  Ok !");
}

void setup()
{
  Screen.init();
  Screen.print(0, "Dx Shake !");

  if (!InitWifi())
  {
    return;
  }

  if (!InitIotHub())
  {
    return;
  }

  InitSensor();
}

bool detectShake()
{
  int steps = 0;
  acc_gyro->getStepCounter(&steps);
  if (steps > 2)
  {
    acc_gyro->resetStepCounter();
    return true;
  }

  return false;
}

static void sendMessage()
{
  const char *messagePayload = "{\"Command\":\"Start\"}";
  EVENT_INSTANCE *message = DevKitMQTTClient_Event_Generate(messagePayload, MESSAGE);
  DevKitMQTTClient_Event_AddProp(message, "messageType", "command");

  Screen.print(3, "Sending...");
  if (DevKitMQTTClient_SendEventInstance(message))
  {
    Screen.print(3, "Msg sent !");
  }
  else
  {
    Screen.print(3, "Msg not sent :(");
  }
}

void loop()
{
  if (!hasWifi || !hasIoTHub)
  {
    delay(500);
    return;
  }

  if (detectShake())
  {
    Screen.print(3, "You shaked :)");
    delay(500);
    sendMessage();
    delay(500);
  }
  else
  {
    Screen.print(3, "Shake me ;o)");
    delay(100);
  }
}
