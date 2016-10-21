#include <Servo.h>

int sensePin_vb = D1;

int servoPin_vb = D2;

Servo servo_vb;

void setup() {

  Serial.begin(115200);
  pinMode(sensePin_vb,INPUT);
  
  servo_vb.attach(servoPin_vb);
  servo_vb.write(90);  
}

void loop() {
  
  int sensedata_vb = digitalRead(sensePin_vb);
  Serial.println(sensedata_vb);
  
  if (sensedata_vb == 1) {
    
    servo_vb.write(45);
    delay(200);
    servo_vb.write(135);
    delay(200);
    servo_vb.write(45);
    
  } else { //sensedata_vb == 0
 
    servo_vb.write(90);

  }
}



