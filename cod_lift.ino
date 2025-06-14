#include <Wire.h>
#include <LiquidCrystal_I2C.h>

LiquidCrystal_I2C lcd(0x27, 16, 2); 

const int motorPin1 = 2;
const int motorPin2 = 10;
const int motorPin3 = 4;
const int motorPin4 = 5;
const int enA = 6;
const int enB = 3;

const int dcIN1 = 7;
const int dcIN2 = 8;
const int dcENA = 9;

const int trigPin = A2;
const int echoPin = A3;

const int buttons[6] = {A1, A0, 1, 11, 12, 13}; 
const int buttonToFloor[6] = {
  2, 
  1, 
  0,
  2,
  1,
  0 
};

const int floorHeights[3] = {0, 17, 35};

int motorSteps[4][4] = {
  {1, 0, 1, 0},
  {0, 1, 1, 0},
  {0, 1, 0, 1},
  {1, 0, 0, 1}
};

int currentStep = 0;

void stepMotor(int step) {
  digitalWrite(motorPin1, motorSteps[step][0]);
  digitalWrite(motorPin2, motorSteps[step][1]);
  digitalWrite(motorPin3, motorSteps[step][2]);
  digitalWrite(motorPin4, motorSteps[step][3]);
}

void moveStepper(bool up, int steps) {
  for (int i = 0; i < steps; i++) {
    stepMotor(currentStep);
    currentStep = (currentStep + (up ? 1 : 3)) % 4;
    delay(5);
  }
}

void openDoor() {
  digitalWrite(dcIN1, HIGH);
  digitalWrite(dcIN2, LOW);
  analogWrite(dcENA, 255);
  delay(1500); 
  digitalWrite(dcIN1, LOW);
  digitalWrite(dcIN2, LOW);
  delay(1000); 
  digitalWrite(dcIN1, LOW);
  digitalWrite(dcIN2, HIGH);
  delay(1500); 
  digitalWrite(dcIN1, LOW);
  digitalWrite(dcIN2, LOW);
}

long readDistanceCM() {
  digitalWrite(trigPin, LOW);
  delayMicroseconds(2);
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);
  long duration = pulseIn(echoPin, HIGH);
  return duration * 0.034 / 2; 
}

void goToFloor(int targetHeight, int floorNumber) {
  long current = readDistanceCM();
  
  if (abs(current - targetHeight) <= 2) {
    lcd.clear();
    lcd.print("Deja la etajul");
    lcd.setCursor(0, 1);
    lcd.print(floorNumber + 1);
    openDoor();
    return;
  }

  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Merg spre etaj ");
  lcd.print(floorNumber + 1);

  while (abs(current - targetHeight) > 2) {
    current = readDistanceCM();
    bool up = (current > targetHeight);
    moveStepper(up, 1);
  }

  lcd.clear();
  lcd.print("Am ajuns la ");
  lcd.setCursor(0, 1);
  lcd.print("etajul ");
  lcd.print(floorNumber + 1);
  openDoor();
}

void setup() {
  lcd.init();
  lcd.backlight();
  lcd.print("Lift functional");

  pinMode(motorPin1, OUTPUT);
  pinMode(motorPin2, OUTPUT);
  pinMode(motorPin3, OUTPUT);
  pinMode(motorPin4, OUTPUT);
  pinMode(enA, OUTPUT);
  pinMode(enB, OUTPUT);
  analogWrite(enA, 255);
  analogWrite(enB, 255);

  pinMode(dcIN1, OUTPUT);
  pinMode(dcIN2, OUTPUT);
  pinMode(dcENA, OUTPUT);
  analogWrite(dcENA, 255);

  pinMode(trigPin, OUTPUT);
  pinMode(echoPin, INPUT);

  for (int i = 0; i < 6; i++) {
    pinMode(buttons[i], INPUT_PULLUP);
  }
}

void loop() {
  for (int i = 0; i < 6; i++) {
    if (digitalRead(buttons[i]) == LOW) {
      delay(200); // debounce
      int floor = buttonToFloor[i];
      lcd.clear();
      lcd.print("Comanda: Etaj ");
      lcd.print(floor + 1);
      delay(500);
      goToFloor(floorHeights[floor], floor);
    }
  }
}