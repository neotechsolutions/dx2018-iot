#include "AZ3166WiFi.h"
#include "DevKitMQTTClient.h"

static bool hasWifi = false;
static bool hasIoTHub = false;

const int userLedPin = 45;

Thread *blenderThread;

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

  DevKitMQTTClient_SetDeviceMethodCallback(DeviceMethodCallback);

  return hasIoTHub;
}

void BlenderThreadRun()
{
  Screen.print(3, "Pulse on");
  digitalWrite(userLedPin, HIGH);
  delay(5000);
  digitalWrite(userLedPin, LOW);
  Screen.print(3, "Pulse off");
}

static int DeviceMethodCallback(const char *methodName, const unsigned char *payload, int size, unsigned char **response, int *response_size)
{
  const char *responseMessage = "\"Successfully invoke device method\"";
  int result = 200;

  if (strcmp(methodName, "start") == 0)
  {
    Screen.print(3, "Started !");
    digitalWrite(userLedPin, HIGH);
  }
  else if (strcmp(methodName, "stop") == 0)
  {
    Screen.print(3, "Stopped !");
    digitalWrite(userLedPin, LOW);
  }
  else if (strcmp(methodName, "pulse") == 0)
  {
    blenderThread = new Thread;
    blenderThread->start(BlenderThreadRun);
  }
  else
  {
    responseMessage = "\"No method found\"";
    result = 404;
  }

  *response_size = strlen(responseMessage) + 1;
  *response = (unsigned char *)strdup(responseMessage);

  return result;
}

void setup()
{
  pinMode(userLedPin, OUTPUT);
  digitalWrite(userLedPin, LOW);

  Screen.init();
  Screen.print(0, "Dx Blender !");

  if (!InitWifi())
  {
    return;
  }

  if (!InitIotHub())
  {
    return;
  }
}

void loop()
{
  if (hasWifi && hasIoTHub)
  {
    DevKitMQTTClient_Check();
  }
  delay(10);
}
