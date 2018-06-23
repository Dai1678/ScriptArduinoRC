#include <SoftwareSerial.h>
#include "Arduino.h"
#include <stdio.h>

SoftwareSerial BT(10, 11);

/*
  ピン番号 3 : モータ出力 左タイヤ
  ピン番号 5 : モータ出力 右タイヤ
  ピン番号 6 : HIGH 左タイヤ前進
  ピン番号 7 : HIGH 左タイヤ後進
  ピン番号 8 : HIGH 右タイヤ前進
  ピン番号 9 : HIGH 右タイヤ後進
*/

#define MOTOR_POWER_LEFT 3
#define MOTOR_POWER_RIGHT 5
#define MOTOR_FRONT_LEFT 6
#define MOTOR_BACK_LEFT 7
#define MOTOR_FRONT_RIGHT 8
#define MOTOR_BACK_RIGHT 9

struct motorCommand {
  String orderId;
  int timeNum;
  int rightSpeed;
  int leftSpeed;
};


void setup() {
  BT.begin(9600);
  Serial.begin(38400);

  //左タイヤ
  pinMode(MOTOR_FRONT_LEFT, OUTPUT);
  pinMode(MOTOR_BACK_LEFT, OUTPUT);

  //右タイヤ
  pinMode(MOTOR_FRONT_RIGHT, OUTPUT);
  pinMode(MOTOR_BACK_RIGHT, OUTPUT);

}

/*-------------------------------------------------
   コントローラーから送られてくるデータフォーマット
   orderId : ラジコンに対する動作命令
      1 : 前進  2 : 後退  3 : 左回転  4 : 右回転
   Time : 動作命令の実行時間
   Speed : 動作命令のモータパワー値

   例: 102100100 (1命令分) (9byte)

   1桁目 -> imageId
   2,3桁目 -> Time
   4,5,6桁目 -> Speed (右回転)
   7,8,9桁目 -> Speed (左回転)

   モータ値100で2秒間前進
  -------------------------------------------------*/

String cmd[20];
int lineNum = 20; // 6Command * 行数

void loop() {  
  if(BT.available() == 0){
    return;
  }
  
  int i = 0;
  while (BT.available() != 0){
    //Serial.println("aaaaaaaa");
    cmd[i] = BT.readStringUntil('\0');
    //Serial.println(cmd[i]);
    delay(50);
    i++;
  }

    // 送信Commandの0番目にfが無かったら
    if (cmd[0].indexOf("f") == -1) {
      struct motorCommand command[6];
      checkData(cmd[0],command);
      for (int j = 0; j < 6; j++){
        motorControl(command[j]); 
      }
      //sleepMotor();
          
    } else {
      for (int k = 1; k < lineNum; k++) {
        if (cmd[k].length() < 4){
          break;
        }
        
        struct motorCommand command[6];
        checkData(cmd[k],command);
        for (int j = 0; j < 6; j++){
          motorControl(command[j]); 
        }
      }
      //sleepMotor();
    }
    //sleepMotor();
}

void checkData(String data, struct motorCommand command[6]) {
  String copyData = data;
  int roopNum = copyData.length() / 9;
  for (int i = 0; i < roopNum; i++) {
    //Idの取り出し(1桁)
    String orderId = copyData.substring(0, 1);

    //Timeの取り出し(2桁)
    int timeCode = copyData.substring(1, 3).toInt();

    //speedの取り出し 右回転(3桁)
    int rightSpeedCode = copyData.substring(3, 6).toInt();

    //speedの取り出し 左回転(3桁)
    int leftSpeedCode = copyData.substring(6, 9).toInt();

    command[i].orderId = orderId;
    command[i].timeNum = timeCode;
    command[i].rightSpeed = rightSpeedCode;
    command[i].leftSpeed = leftSpeedCode;
    copyData = copyData.substring(9, copyData.length());
  }
}

void motorControl(struct motorCommand command) {

  String image = command.orderId;
  int timeNum = command.timeNum;
  int leftSpeed = command.leftSpeed;
  int rightSpeed = command.rightSpeed;
  timeNum = timeNum * 100; //単位msに変換

  //前進
  if (image.equals("1")) {
    //左タイヤ
    digitalWrite(MOTOR_FRONT_LEFT, HIGH);
    digitalWrite(MOTOR_BACK_LEFT, LOW);

    //右タイヤ
    digitalWrite(MOTOR_FRONT_RIGHT, HIGH);
    digitalWrite(MOTOR_BACK_RIGHT, LOW);

    //出力調整
    analogWrite(MOTOR_POWER_LEFT, leftSpeed);
    analogWrite(MOTOR_POWER_RIGHT, rightSpeed);

    delay(timeNum);

    sleepMotor();
  }

  //後退
  else if (image.equals("2")) {
    //左タイヤ
    digitalWrite(MOTOR_FRONT_LEFT, LOW);
    digitalWrite(MOTOR_BACK_LEFT, HIGH);

    //右タイヤ
    digitalWrite(MOTOR_FRONT_RIGHT, LOW);
    digitalWrite(MOTOR_BACK_RIGHT, HIGH);

    //出力調整
    analogWrite(MOTOR_POWER_LEFT, leftSpeed);
    analogWrite(MOTOR_POWER_RIGHT, rightSpeed);

    delay(timeNum);

    sleepMotor();
  }

  //左回転
  else if (image.equals("3")) {
    //左タイヤ
    digitalWrite(MOTOR_FRONT_LEFT, LOW);
    digitalWrite(MOTOR_BACK_LEFT, HIGH);

    //右タイヤ
    digitalWrite(MOTOR_FRONT_RIGHT, HIGH);
    digitalWrite(MOTOR_BACK_RIGHT, LOW);

    //出力調整
    analogWrite(MOTOR_POWER_LEFT, leftSpeed);
    analogWrite(MOTOR_POWER_RIGHT, rightSpeed);

    delay(timeNum);

    sleepMotor();
  }

  //右回転
  else if (image.equals("4")) {
    //左タイヤ
    digitalWrite(MOTOR_FRONT_LEFT, HIGH);
    digitalWrite(MOTOR_BACK_LEFT, LOW);

    //右タイヤ
    digitalWrite(MOTOR_FRONT_RIGHT, LOW);
    digitalWrite(MOTOR_BACK_RIGHT, HIGH);

    //出力調整
    analogWrite(MOTOR_POWER_LEFT, leftSpeed);
    analogWrite(MOTOR_POWER_RIGHT, rightSpeed);

    delay(timeNum);

    sleepMotor();
  }

  //停止
  else if (image.equals("5")) {
    //左タイヤ
    digitalWrite(MOTOR_FRONT_LEFT, LOW);
    digitalWrite(MOTOR_BACK_LEFT, LOW);

    //右タイヤ
    digitalWrite(MOTOR_FRONT_RIGHT, LOW);
    digitalWrite(MOTOR_BACK_RIGHT, LOW);

    delay(timeNum);

    sleepMotor();
  }

}

void sleepMotor() {
  digitalWrite(MOTOR_FRONT_LEFT, LOW);
  digitalWrite(MOTOR_BACK_LEFT, LOW);
  digitalWrite(MOTOR_FRONT_RIGHT, LOW);
  digitalWrite(MOTOR_BACK_RIGHT, LOW);

  delay(500);
}
