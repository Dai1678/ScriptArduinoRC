#include <Arduino.h>

struct motorCommand {
  String orderId;
  int timeNum;
  int rightSpeed;
  int leftSpeed;
};

struct motorCommand checkData(String data);
