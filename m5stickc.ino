#include <M5StickCPlus.h>
#include "BluetoothSerial.h"

// float accX = 0.0F;
// float accY = 0.0F;
// float accZ = 0.0F;

float gyroX = 0.0F;
float gyroY = 0.0F;
float gyroZ = 0.0F;

// float pitch = 0.0F;
// float roll  = 0.0F;
// float yaw   = 0.0F;

enum position {
  original,
  up,
  down,
  left,
  right
};
const char* positionStr[] = {"original", "up", "down","left","right"};

int nUp = 0;
int nDown = 0;
int nLeft = 0;
int nRight = 0;
int nStop = 0;
int nClick = 0;
int threshold = 4;
enum position headState;
int micVol = 0;

int headup = 0;
int btnA = 39; // SELECT BUTTON 
int btnB = 37; // MENU BUTTON
int micPin = 26; // External Microphone MAX4466

#if !defined(CONFIG_BT_ENABLED) || !defined(CONFIG_BLUEDROID_ENABLED)
#error Bluetooth is not enabled! Please run `make menuconfig` to and enable it
#endif

#if !defined(CONFIG_BT_SPP_ENABLED)
#error Serial Bluetooth not available or not enabled. It is only available for the ESP32 chip.
#endif

BluetoothSerial SerialBT;

void setup() {
    M5.begin();             // Init M5StickC Plus.  初始化 M5StickC Plus
    M5.Imu.Init();          // Init IMU.  初始化IMU
    M5.Lcd.setRotation(1);  // Rotate the screen. 将屏幕旋转
    M5.Lcd.fillScreen(BLACK);
    M5.Lcd.setTextSize(4);

    Serial.begin(115200);
    pinMode(btnA,INPUT_PULLUP);
    pinMode(btnB,INPUT_PULLUP);
    headState = original;
    SerialBT.begin("contactlessMouse"); //Bluetooth device name

    
}

void loop() {
    static float temp = 0;
    M5.IMU.getGyroData(&gyroX, &gyroY, &gyroZ);

    micVol = analogRead(micPin);
    if(micVol > 3500 || micVol < 1){
      nClick++;
    }else{
      nClick = 0;
    }


    // Head move? add position count: add normal count;
    if((gyroY > 0) || (gyroY < -14) || (gyroZ < 0) || (gyroZ > 10)){
      if (gyroY > 0){
        nUp++;
      }else if(gyroY < -14){
        nDown++;
      }
      if(gyroZ < 0){
        nLeft++;
      }else if(gyroZ > 10){
        nRight++;
      }
    }else{
      nStop++;
    }

    // Any position > threshold: perform action
    if(nUp >= threshold){
      if(headState == down){
        headState = original;
      }else{
        headState = up;
      }
      nReset();
    }else if(nDown >= threshold){
      if(headState == up){
        headState = original;
      }else{
        headState = down;
      }
      nReset();
    }

    if(nRight >= threshold){
      if(headState == left){
        headState = original;
      }else{
        headState = right;
      }
      nReset();
    }else if(nLeft >= threshold){
      if(headState == right){
        headState = original;
      }else{
        headState = left;
      }
      nReset();
    }

    // reset all position if idle
    if(nStop >= 3){
      nReset();
    }

    if (headup>0 && headup<3){
      Serial.printf("Value: %5.2f   %5.2f   %5.2f\n", gyroX, gyroY, gyroZ);
      Serial.println(positionStr[headState]);
      // bluetoothSend(positionStr[headState]);
      if (headState == up){
        M5.Lcd.setCursor(30, 50);
        M5.Lcd.printf("   UP   ");
        bluetoothSend("m:u");
      }else if(headState == down){
        M5.Lcd.setCursor(30, 50);
        M5.Lcd.printf("  DOWN  ");
        bluetoothSend("m:d");
      }else if(headState == left){
        M5.Lcd.setCursor(30, 50);
        M5.Lcd.printf("  LEFT  ");
        bluetoothSend("m:l");
      }else if(headState == right){
        M5.Lcd.setCursor(30, 50);
        M5.Lcd.printf(" RIGHT  ");
        bluetoothSend("m:r");
      }else if(headState == original){
        M5.Lcd.setCursor(30, 50);
        M5.Lcd.printf(" normal ");
        bluetoothSend("o");
      }
      
      if(nClick == 1){
        M5.Lcd.printf(" CLICK  ");
        bluetoothSend("c");
      }

    }
    
    if (digitalRead(btnA) == LOW) {
      headup++;
    }
    if (digitalRead(btnB) == LOW) {
      headup = 0;
      headState = original;
      nReset();
    }

    delay(150);
}

void nReset(){
  nUp = 0;
  nDown = 0;
  nLeft = 0;
  nRight = 0;
  nStop = 0;
}

void bluetoothSend(String line)
{
    // Serial.printf("%s", line);
    unsigned l = line.length();
    for(int i=0; i<l; i++)
    {
        if(line[i]!='\0')
            SerialBT.write(byte(line[i]));
    }
    SerialBT.write(10); // \n
}
