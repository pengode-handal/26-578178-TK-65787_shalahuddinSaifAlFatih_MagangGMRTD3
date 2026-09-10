#include <ESP32Servo.h>
#include <Adafruit_MPU6050.h>
#include <Adafruit_Sensor.h>
#include <Wire.h>

// Pin servo
#define SERVO1_PIN 12
#define SERVO2_PIN 13
#define SERVO3_PIN 25
#define SERVO4_PIN 26
#define SERVO5_PIN 27
// Range rotation
#define RANGE_ROTATION 250

// Pin PIR
#define PIR_PIN 33

Servo servo1;
Servo servo2;
Servo servo3;
Servo servo4;
Servo servo5;

Adafruit_MPU6050 mpu;

// set semua servo ke sudut deg
void setAllServos(int deg) {
servo1.write(deg);
servo2.write(deg);
servo3.write(deg);
servo4.write(deg);
servo5.write(deg);
}

// map manual bertipe float, tanpa ke long
float mapFloat(float x, float in_min, float in_max, float out_min, float out_max) {
return (x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
}

void setup() {
// init Serial Monitor
Serial.begin(115200);
// init MPU I2C
Wire.begin(); 
// cek koneksi MPU6050 
while (!mpu.begin()) {
    Serial.println("MPU6050 not connected!");
    delay(1000);
}
Serial.println("MPU6050 ready!");

// set PIR input
pinMode(PIR_PIN, INPUT);
// set max gyro range (-250 sampe 250 deg/s)
mpu.setGyroRange(MPU6050_RANGE_250_DEG);

// Connect servo
servo1.attach(SERVO1_PIN);
servo2.attach(SERVO2_PIN);
servo3.attach(SERVO3_PIN);
servo4.attach(SERVO4_PIN);
servo5.attach(SERVO5_PIN);

// posisi default servo
setAllServos(90);
Serial.println("All system ready!");
}

// nilai yaw awal
int yawNow = 0;
void loop() {

sensors_event_t acc;
sensors_event_t gyro; 
sensors_event_t temp;

// get data dari MPU6050
mpu.getEvent(&acc,&gyro,&temp);

// PIR cek motion
if (digitalRead(PIR_PIN) == HIGH) {
// kalau kedeteksi
Serial.println("Motion detected!");
// set semua servo ke posisi 135(bebas)
setAllServos(135);
delay(500);
// balik ke posisi default
setAllServos(90);
delay(700);
} else {
// mengubah data gyro jadi value untuk muter servo 
// input gyro dalam rad/s, jadi RANGE_ROTATION deg/s di conevert ke rad/s ((RANGE_ROTATION*PI/180)).
// value servo nya di mapping dari (-RANGE_ROTATION*PI/180) sampai (RANGE_ROTATION*PI/180) biar jadi -90 sampai 90 deg
int roll = (int)mapFloat((double)gyro.gyro.x, -(RANGE_ROTATION*PI/180), (RANGE_ROTATION*PI/180), 180, 0);
int pitch = (int)mapFloat((double)gyro.gyro.y, -(RANGE_ROTATION*PI/180), (RANGE_ROTATION*PI/180), 0, 180);
int yaw = (int)mapFloat((double)gyro.gyro.z, -(RANGE_ROTATION*PI/180), (RANGE_ROTATION*PI/180), 0, 180);
// servo 1 dan 2 berputar sesuai roll
servo1.write(roll);
servo2.write(roll);
// servo 3 dan 4 berputar sesuai pitch
servo3.write(pitch);
servo4.write(pitch);

// servo 5 berputar sesuai yaw dengan tambahan delay 1 detik lalu balik ke posisi default
if (gyro.gyro.z != 0){
    // cek kalau yaw berubah maka servo 5 berputar, kalau ga ini di skip
    if (yawNow != yaw){
    servo5.write(yaw);
    delay(1000);
    servo5.write(90);
    yawNow = yaw;
    }
}
}
}
