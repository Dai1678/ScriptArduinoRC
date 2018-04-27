#include <SoftwareSerial.h>
#include "Arduino.h"
#include "motorCommand.h"
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
   imageId : ラジコンに対する動作命令
      1 : 前進  2 : 後退  3 : 左回転  4 : 右回転
   Time : 動作命令の実行時間
   Speed : 動作命令のモータパワー値

   例: 102100 (1命令分) (6byte)

   1桁目 -> imageId
   2,3桁目 -> Time
   4,5,6桁目 -> Speed

   モータ値100で2秒間前進
  -------------------------------------------------*/

void loop() {
  //Serial.available()で取得できるのは最大64byteなので、現状は一度に10命令分までしか実行できない
  if(BT.available() == 0){
    return;
}
  String data = BT.readStringUntil('\0');
  if (data.substring(0, 2) == "ff") {
    int lineNum = data.substring(2, data.length()).toInt();
    String BTCommandStr[lineNum];
    //Serial.print("header" + String(lineNum) + "\n");
    for (int i = 0; i < lineNum; i++ ) {
      BTCommandStr[i] = BT.readStringUntil('\0');
    }
    //Serial.print(String(BTCommandStr[1])+"\n");
    // BTCommandStr が行の中身
    //lineNumが行数
      
    struct motorCommand commands[100];
    int j = 0;
    for (int i = 0; i < lineNum; i++) {
      struct motorCommand tmpCommand[6];
      String str = BTCommandStr[i];
      checkData(str,tmpCommand);
        
      Serial.print(String(tmpCommand[0].orderId)+"\n");
      //Serial.print(String(BTCommandStr[i])+"\n");

        // TODO 値渡し用に時間稼ぐと動くのでは？ 
        delay(100);
        commands[j] = tmpCommand[0];
        commands[j+1] = tmpCommand[1];
        commands[j+2] = tmpCommand[2];
        commands[j+3] = tmpCommand[3];
        commands[j+4] = tmpCommand[4];
        commands[j+5] = tmpCommand[5];
        j = j + 6;
     }

    for (int i = 0; i < sizeof(commands); i++) {
      //Serial.print(String(commands[i].orderId)+"\n");
      motorControl(commands[i]);
    }
    sleepMotor();

      //シングル
    } else {
      struct motorCommand command[6];
      checkData(data,command);
      for (int i = 0; i < sizeof(command); i++) {
        motorControl(command[i]);  
      }
      sleepMotor();
    }
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

    Serial.print(String(command[i].orderId)+"\n");
  }
}

//TODO モーター出力値の調整
void motorControl(struct motorCommand command) {

  String image = command.orderId;
  int timeNum = command.timeNum;
  int leftSpeed = command.leftSpeed;
  int rightSpeed = command.rightSpeed;

  //Serial.print(image + "\n" + String(timeNum) + "\n" + String(leftSpeed) + "\n" + String(rightSpeed) + "\n");

  timeNum = timeNum * 1000; //単位msに変換

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
  }

}

void sleepMotor() {
  digitalWrite(MOTOR_FRONT_LEFT, LOW);
  digitalWrite(MOTOR_BACK_LEFT, LOW);
  digitalWrite(MOTOR_FRONT_RIGHT, LOW);
  digitalWrite(MOTOR_BACK_RIGHT, LOW);

  delay(100);
}