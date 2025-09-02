char data; // Bluetooth command
 // --- Car motor pins --
#define LEFT_MOTOR_IN1 4
 #define LEFT_MOTOR_IN2 5
 #define RIGHT_MOTOR_IN1 6
 #define RIGHT_MOTOR_IN2 7
 // --- Conveyor motor pins --
#define CONVEYOR_PWM 10   // ENB (PWM speed)
 #define CONVEYOR_DIR 9    // IN3 (Direction)
 #define IN4 8             // Not used, tied to GND
 // --- HC-SR05 Ultrasonic Sensor (rear) --
#define TRIG_PIN 12
 #define ECHO_PIN 11
 const int SAFE_DISTANCE = 15; // cm, minimum allowed distance behind
 // --- ACS712 sensor --
#define ACS_PIN A0
 const float vRef = 5.0;
 const int adcMax = 1023;
 const float sensitivity = 0.185; // adjust according to your module
 float zeroV = 2.5;
 bool conveyorRunning = false;
 bool latchedOvercurrent = false;
 // Reset override timer
 unsigned long conveyorResetTime = 0;
 const unsigned long resetIgnoreDuration = 200; // ms to ignore overcurrent after 
reset
 void setup() {
  Serial.begin(9600);
  // Car motors
  pinMode(LEFT_MOTOR_IN1, OUTPUT);
  pinMode(LEFT_MOTOR_IN2, OUTPUT);
  pinMode(RIGHT_MOTOR_IN1, OUTPUT);
  pinMode(RIGHT_MOTOR_IN2, OUTPUT);
  stopMotors();
  // Conveyor motor
  pinMode(CONVEYOR_PWM, OUTPUT);
  pinMode(CONVEYOR_DIR, OUTPUT);
  stopConveyor();
  // Ultrasonic sensor
  pinMode(TRIG_PIN, OUTPUT);
  pinMode(ECHO_PIN, INPUT);
  // Calibrate ACS712 zero offset
  long sum = 0;
  for (int i = 0; i < 200; i++) { sum += analogRead(ACS_PIN); delay(3); }
  zeroV = (sum / 200.0) * vRef / adcMax;
  Serial.print("ZeroV="); Serial.println(zeroV, 3);
  delay(2000); // sensor stabilization
 }
 void loop() {
  // --- Conveyor current protection --
  if (conveyorRunning) {
    if (millis() - conveyorResetTime >= resetIgnoreDuration && !
 latchedOvercurrent) {
      float current = readCurrentAvg(50);
      static unsigned long overStart = 0;
      if (current > 1.0) { // overcurrent
        if (overStart == 0) overStart = millis();
        if (millis() - overStart > 200) { // sustained 200ms
          stopConveyor();
          latchedOvercurrent = true;
          Serial.println("
 ⚠
 ️
        }
      } else {
        overStart = 0;
      }
    }
  }
 Overcurrent >1A, conveyor stopped");
  // --- Bluetooth command handling --
  if (Serial.available() > 0) {
    data = Serial.read();
    switch (data) {
      // Car controls
      case 'F': forward();   break;
      case 'B': backward();  break;
      case 'L': turnLeft();  break;
      case 'R': turnRight(); break;
      case 'S': stopMotors(); break;
      // Conveyor controls
      case 'W': // ON
        if (!latchedOvercurrent) {
          startConveyor();
          Serial.println("Conveyor ON");
        }
        break;
      case 'w': // OFF
        stopConveyor();
        Serial.println("Conveyor OFF");
        break;
      case 'U': // RESET after trip
        if (latchedOvercurrent) {
          latchedOvercurrent = false;
          startConveyor();
          conveyorResetTime = millis();
          Serial.println("Conveyor RESET and ON");
        }
        break;
    }
  }
 }
 // --- Car motor functions --
void forward() {
  digitalWrite(LEFT_MOTOR_IN1, HIGH);
  digitalWrite(LEFT_MOTOR_IN2, LOW);
  digitalWrite(RIGHT_MOTOR_IN1, HIGH);
  digitalWrite(RIGHT_MOTOR_IN2, LOW);
 }
 void backward() {
  int dist = getDistanceCM();
  if (dist > SAFE_DISTANCE) {
    digitalWrite(LEFT_MOTOR_IN1, LOW);
    digitalWrite(LEFT_MOTOR_IN2, HIGH);
    digitalWrite(RIGHT_MOTOR_IN1, LOW);
    digitalWrite(RIGHT_MOTOR_IN2, HIGH);
  } else {
    stopMotors();
    Serial.print("
 ⚠
 ️
 Obstacle behind! Distance=");
    Serial.print(dist);
    Serial.println(" cm. Backward blocked.");
  }
 }
 void turnLeft() {
  digitalWrite(LEFT_MOTOR_IN1, HIGH);
  digitalWrite(LEFT_MOTOR_IN2, LOW);
  digitalWrite(RIGHT_MOTOR_IN1, LOW);
  digitalWrite(RIGHT_MOTOR_IN2, LOW);
 }
 void turnRight() {
  digitalWrite(LEFT_MOTOR_IN1, LOW);
  digitalWrite(LEFT_MOTOR_IN2, LOW);
  digitalWrite(RIGHT_MOTOR_IN1, HIGH);
  digitalWrite(RIGHT_MOTOR_IN2, LOW);
 }
 void stopMotors() {
  digitalWrite(LEFT_MOTOR_IN1, LOW);
  digitalWrite(LEFT_MOTOR_IN2, LOW);
  digitalWrite(RIGHT_MOTOR_IN1, LOW);
  digitalWrite(RIGHT_MOTOR_IN2, LOW);
 }
 // --- Conveyor functions --
void startConveyor() {
  digitalWrite(CONVEYOR_DIR, HIGH);   // spin forward (IN3 HIGH, IN4 LOW)
  analogWrite(CONVEYOR_PWM, 64);      // 25% speed
  conveyorRunning = true;
 }
 void stopConveyor() {
  analogWrite(CONVEYOR_PWM, 0);
  digitalWrite(CONVEYOR_DIR, LOW);    // ensure safe stop
  conveyorRunning = false;
 }
 // --- ACS712 helper --
float readCurrentAvg(int n) {
  long acc = 0;
  for (int i = 0; i < n; i++) acc += analogRead(ACS_PIN);
  float v = (acc / (float)n) * vRef / adcMax;
  return fabs((v - zeroV) / sensitivity);
 }
 // --- Ultrasonic helper --
int getDistanceCM() {
  digitalWrite(TRIG_PIN, LOW);
  delayMicroseconds(2);
  digitalWrite(TRIG_PIN, HIGH);
  delayMicroseconds(10);
  digitalWrite(TRIG_PIN, LOW);
  long duration = pulseIn(ECHO_PIN, HIGH, 20000); // 20ms timeout
}
  int distance = duration * 0.034 / 2; // cm
  return distance;