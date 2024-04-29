//Include Libraries
#include "UbidotsEsp32Mqtt.h"
#include <ESP32Servo.h>
#include <Stepper.h>

//Define Constants
const char *UBIDOTS_TOKEN = "BBUS-jC6pw6z6O0C0KZ8DruSs8senOB5qyr";
// const char *WIFI_SSID = "Allyns iPhone";      // Put here your Wi-Fi SSID
// const char *WIFI_PASS = "esp32Cam";      // Put here your Wi-Fi password
const char *WIFI_SSID = "eir37829968";  // Put here your Wi-Fi SSID
const char *WIFI_PASS = "DcSbcaJktK";   // Put here your Wi-Fi password
const char *DEVICE_LABEL = "robot";
char *var_labels[] = {
  "servo1",
  "servo2",
  "servo3",
  "servo4",
  "servo5",
};

const int stepsPerRevolution = 200;
Stepper myStepper = Stepper(stepsPerRevolution, 32, 25, 33, 26);
Servo shoulderServoStrong;  // create servo object to control a servo
Servo shoulderServoWeak;    // create servo object to control a servo
Servo elbowServo;           // create servo object to control a servo
Servo toolServo;
Servo wristServo;

int shoulderStrongPos = 0;  // variable to store the servo position
int elbowPos = 0;           // variable to store the servo position
int shoulderWeakPos = 0;    // variable to store the servo position
int toolServoPos = 0;       // variable to store the servo position
int wristPos = 0;

#if defined(CONFIG_IDF_TARGET_ESP32S2) || defined(CONFIG_IDF_TARGET_ESP32S3)
// int servoPin2 = 17;
// int servoPin3 = 16;
// int servoPin4 =

#elif defined(CONFIG_IDF_TARGET_ESP32C3)
int servoPin = 7;
#else
const int relay = 28;
int shoulderStrongPin = 18;  //bottom right
int elbowPin = 17;           //elbow?
int shoulderWeakPin = 16;    //bottom left
int toolPin = 14;            //gripper
int wristPin = 15;           //rotate tool



#endif
int prevAngleShoudlerStrong = 0;
int prevAngleElbow = 0;
int previousAngle3 = 0;
int prevAngleTool = 0;
int prevAngelWrist = 0;
int prevBaseAngle = 0;

#define DIMENSION_OF(x) (sizeof(x) / sizeof(x[0]))

float var_last_values[DIMENSION_OF(var_labels)] = {};
Ubidots ubidots(UBIDOTS_TOKEN);


//Callback function
void callback(char *topic, byte *payload, unsigned int length) {
  char *topic_cpy = strdup(topic);
  char *payload_str = (char *)malloc(length + sizeof(""));
  char *topic_item = strtok(topic_cpy, "/");
  char *label = NULL;
  float value = NAN;
  size_t index = DIMENSION_OF(var_labels);
  size_t i;

  memcpy(payload_str, payload, length);
  payload_str[length] = '\0';

  while ((NULL != topic_item) && (NULL == label)) {
    for (i = 0; i < DIMENSION_OF(var_labels); i++) {
      if (0 == strcmp(var_labels[i], topic_item)) {
        label = topic_item;
        value = atof(payload_str);
        index = i;
        break;
      }
    }
    topic_item = strtok(NULL, "/");
  }

  if (index < DIMENSION_OF(var_labels)) {
    var_last_values[index] = value;
    switch (index) {
      case 0:
        Serial.print("Servo 1 val: ");
        Serial.println(var_last_values[index]);
        moveServo(1, var_last_values[index]);
        break;
      case 1:
        Serial.print("Servo 2 val: ");
        Serial.println(var_last_values[index]);
        moveServo(2, var_last_values[index]);
        break;
      case 2:
        Serial.print("Servo 3 val: ");
        Serial.println(var_last_values[index]);
        moveServo(3, var_last_values[index]);
        break;
      case 3:
        Serial.print("Servo 4 val: ");
        Serial.println(var_last_values[index]);
        moveServo(4, var_last_values[index]);
        break;
      case 4:
        Serial.print("Stepper val: ");
        Serial.println(var_last_values[index]);
        moveServo(5, var_last_values[index]);
    }
  }
  free(topic_cpy);
  free(payload_str);
}

void moveServo(int servo, int angle_value) {


  switch (servo) {
    case 1:
      if (angle_value >= 170) {
        angle_value = 170;
      } else if (angle_value <= 10) {
        angle_value = 10;
      }
      angle_value = 180 - angle_value;
      Serial.println(angle_value);
      if (angle_value > prevAngleShoudlerStrong) {
        for (int i = prevAngleShoudlerStrong; i < angle_value; i += 1) {
          shoulderServoStrong.write(i);
          shoulderServoWeak.write(i * 0.66);
          Serial.print(i);
          delay(20);
        }
      } else {
        for (int i = prevAngleShoudlerStrong; i > angle_value; i -= 1) {
          shoulderServoStrong.write(i);
          shoulderServoWeak.write(i * 0.66);
          Serial.print(i);
          delay(20);
        }
      }
      prevAngleShoudlerStrong = angle_value;
      break;
    case 2:
      if (angle_value >= 170) {
        angle_value = 170;
      } else if (angle_value <= 10) {
        angle_value = 10;
      }
      Serial.println(angle_value);
      if (angle_value > prevAngleElbow) {
        for (int i = prevAngleElbow; i < angle_value; i++) {
          elbowServo.write(i);
          Serial.print(i);
          delay(15);
        }
      } else {
        for (int i = prevAngleElbow; i > angle_value; i--) {
          elbowServo.write(i);
          Serial.print(i);
          delay(15);
        }
      }
      prevAngleElbow = angle_value;
      break;
    case 3:
      if (angle_value >= 170) {
        angle_value = 170;
      } else if (angle_value <= 10) {
        angle_value = 10;
      }
      Serial.println(angle_value);
      if (angle_value > prevAngelWrist) {
        for (int i = prevAngelWrist; i < angle_value; i++) {
          wristServo.write(i);
          Serial.print(i);
          delay(15);
        }
      } else {
        for (int i = prevAngelWrist; i > angle_value; i--) {
          wristServo.write(i);
          Serial.print(i);
          delay(15);
        }
      }
      prevAngelWrist = angle_value;
      // myservo2.detach();
      break;
    case 4:
      if (angle_value >= 170) {
        angle_value = 170;
      } else if (angle_value <= 10) {
        angle_value = 10;
      }
      toolServo.attach(toolPin, 600, 2400);
      Serial.println("Servo 4 attached");
      Serial.println(angle_value);
      if (angle_value > prevAngleTool) {
        for (int i = prevAngleTool; i < angle_value; i++) {
          toolServo.write(i);
          Serial.print(i);
          delay(15);
        }
      } else {
        for (int i = prevAngleTool; i > angle_value; i--) {
          toolServo.write(i);
          Serial.print(i);
          delay(15);
        }
      }
      prevAngleTool = angle_value;
      toolServo.detach();
      Serial.println("\nServo 4 detached");
      break;
    case 5:
      if(angle_value == prevBaseAngle){
        break;
      }
      int desiredAngle = angle_value - prevBaseAngle;
      Serial.println(desiredAngle);
      myStepper.setSpeed(5);
      myStepper.step(desiredAngle);
      prevBaseAngle = angle_value;
      break;
  }
}


//Subscribe function
void subscribe_to_vars(char **labels, size_t n_labels) {
  size_t i;
  for (i = 0; i < n_labels; i++) {
    char *label = labels[i];
    ubidots.subscribeLastValue(DEVICE_LABEL, label);
  }
}

//Print last values function
void print_last_values(void) {
  char str_buff[64] = "";
  size_t i;

  for (i = 0; i < DIMENSION_OF(var_labels); i++) {
    snprintf(str_buff, sizeof(str_buff), "%s: %f", var_labels[i], var_last_values[i]);
    Serial.println(str_buff);
  }
}

void setup() {
  shoulderServoStrong.attach(shoulderStrongPin, 1000, 2000);  // attaches the servo on pin 18 to the servo object
  elbowServo.attach(elbowPin, 1000, 2000);                    // attaches the servo on pin 18 to the servo object
  shoulderServoWeak.attach(shoulderWeakPin, 1000, 2000);      // attaches the servo on pin 18 to the servo object
  wristServo.attach(wristPin, 500, 2500);
  Serial.begin(115200);
  pinMode(relay, OUTPUT);
  ubidots.connectToWifi(WIFI_SSID, WIFI_PASS);
  ubidots.setCallback(callback);
  ubidots.setup();
  ubidots.reconnect();
  subscribe_to_vars(var_labels, DIMENSION_OF(var_labels));
}

void loop() {
  if (!ubidots.connected()) {
    ubidots.reconnect();
    subscribe_to_vars(var_labels, DIMENSION_OF(var_labels));
  }
  ubidots.loop();
}

void moveServo() {
}
