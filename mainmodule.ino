#include <LiquidCrystal_I2C.h>
#include <Wire.h>
#include <I2Cdev.h>     
#include <MPU6050.h>
#include <RTClib.h>

#define I2C_ADDR    0x27
#define LCD_COLUMNS 20
#define LCD_LINES   4

LiquidCrystal_I2C lcd(I2C_ADDR, LCD_COLUMNS, LCD_LINES);

int trigpinshort = 7;
int trigpinlong  = 3;
int echopinshort = 8;
int echopinlong  = 4;

int a, b;
MPU6050 mpu(0x69);
RTC_DS1307 rtc;

int16_t ax, ay, az;
int16_t gx, gy, gz;

struct MyData {
  byte X;
  byte Y;
  byte Z;
};

int pir = 13;
int ldr = A0;

MyData data;

void setup() {
  Serial.begin(9600);
  Wire.begin();

  pinMode(trigpinshort, OUTPUT);
  pinMode(trigpinlong,  OUTPUT);
  pinMode(echopinshort, INPUT);
  pinMode(echopinlong,  INPUT);
  
  mpu.initialize();
  pinMode(pir, INPUT);
  pinMode(5, OUTPUT);

  lcd.init();
  lcd.backlight();  
  rtc.begin();   
}

void loop() {
  display();
}

int ultrasensor() {
  // Short range
  digitalWrite(trigpinshort, HIGH);  
  delayMicroseconds(10);
  digitalWrite(trigpinshort, LOW);
  int timeshort = pulseIn(echopinshort, HIGH);

  // Long range
  digitalWrite(trigpinlong, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigpinlong, LOW);
  int timelong = pulseIn(echopinlong, HIGH);

  if(timeshort <= 5800) {
    return timeshort / 58;
  } else if(timeshort >= 5800 && timelong <= 8700) {
    return (0.4 * timeshort + 0.6 * timelong) / 58;
  } else {
    return timelong / 58;
  }
}

int acclerometer() {
  mpu.getMotion6(&ax, &ay, &az, &gx, &gy, &gz);
  data.Z = map(gz, -17000, 17000, 0, 255);  
  return data.Z;
}

String stealth() {
  a = digitalRead(pir);
  b = analogRead(ldr);

  // Scale LDR to PWM range
  analogWrite(5, map(b, 0, 1023, 255, 0));
    
  if(a == 1) {
    return "PRESENT";
  } else {
    return "ABSENT";
  }
}

void display() {
  DateTime now = rtc.now();

  lcd.setCursor(0, 0);
  lcd.print("Distance: ");
  lcd.print(ultrasensor());

  lcd.setCursor(0, 1);
  lcd.print("Z-axis: ");
  lcd.print(acclerometer());
  lcd.print(" ");
  lcd.print(stealth());

  lcd.setCursor(0, 2);
  lcd.print(now.hour());
  lcd.print(":");
  lcd.print(now.minute());
  lcd.print(":");
  lcd.print(now.second());

  lcd.setCursor(0, 3);
  lcd.print("LDR: ");
  lcd.print(b);

  delay(1000);
}
