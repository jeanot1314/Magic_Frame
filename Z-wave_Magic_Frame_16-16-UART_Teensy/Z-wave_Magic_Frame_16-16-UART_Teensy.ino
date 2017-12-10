/*
  This code read turn on LED through a STP16CP driver.
  For every LED, it read the oposite 3 sensors

  If the value is above the threshold LOW, it mean an object obstruct the LED.

  Author : Jean Perardel
*/

#define PIN_SDI 44
#define PIN_CLK 45
#define PIN_LE 46

#define THRESHOLD_HIGH 900
#define THRESHOLD_LOW 400
#define NB_LED 16
#define NB_SENSORS 16
#define NB_SENSOR_PER_LED 3

int led = 13;
char test = 0xFF;
int sensorsValues[NB_LED][NB_SENSOR_PER_LED];
boolean detect[NB_SENSORS * NB_SENSOR_PER_LED];
int flagValue = -100;

// the setup routine runs once when you press reset:
void setup() {
  // initialize the digital pin as an output.
  Serial.begin(9600);
  Serial3.begin(9600);
  pinMode(PIN_SDI, OUTPUT);
  pinMode(PIN_CLK, OUTPUT);
  pinMode(PIN_LE, OUTPUT);
  pinMode(led, OUTPUT);
  digitalWrite(PIN_LE, LOW);
  digitalWrite(PIN_SDI, LOW);
  digitalWrite(PIN_CLK, LOW);
}

// the loop routine runs over and over again forever:
void loop() {
  
  clearTable(); // Set the tables to 0

  for (int l = 0; l < NB_LED; l++) {

    LED_IR(l); //Turn ON a LED (as it's common anode, LOW is on and HIGH is OFF)
    delay(2); // Wait a little for the sensors
    if (l > 0) { // Don't read the value before the first sensor for the first LED
      readSensors(l, l - 1, 0);
    }
    readSensors(l, l, 1);
    if (l < NB_LED - 1) { // Don't read the value after the last sensor for the last LED
      readSensors(l, l + 1, 2);
    }
  }
  digitalWrite(led, !digitalRead(led));
  printDebug();

  for (int i = 0; i < (NB_SENSORS * NB_SENSOR_PER_LED); i++) {
    if (detect[i] == 0 && detect[i + 1] == 0) {
      if ( i <= flagValue - 6 || flagValue+6 <= i) {
        flagValue = i;
        Serial3.write(0xFF);
        byte data = map(i, 1 , 47, 0, 255);
        Serial3.write(data);
        Serial.println(data);
        //Serial3.write(0x00);
        delay(200);
      } 
    }
  }
  delay(100);
}

void printDebug(void) {
  Serial.println();
  Serial.println("------------------tableau--------------------");
  //for (int i = 0; i < NB_LED; i++) {
  for (int i = NB_LED-1; i >= 0; i--) {
    for (int j = 0; j < NB_SENSOR_PER_LED; j++) {
      if (sensorsValues[i][j] == 100) {
        Serial.print("===");
        if (j == 0) {
          detect[i * 3 + 0] = 1;
        }
        if (j == 1) {
          detect[i * 3 + 1] = 1;
        }
        if (j == 2) {
          detect[i * 3 + 2] = 1;
        }
      }
      else if (sensorsValues[i][j] == 999) {
        Serial.print("XXX");
      }
      else {
        Serial.print(sensorsValues[i][j]);
      }

      Serial.print(" - ");
    }
    Serial.print("   LED = ");
    Serial.print(i);
    Serial.println();
  }
  Serial.print("Data = ");
  for (int i = 0; i < (NB_LED * 3); i++) {
    Serial.print(detect[i]);
    Serial.print(" ");
  }
  Serial.println();
}

void clearTable() {
  for (int i = 0; i < NB_LED; i++) {
    for (int j = 0; j < NB_SENSOR_PER_LED; j++) {
      sensorsValues[i][j] = 999;
    }
  }

  for (int i = 0; i < (NB_SENSORS * NB_SENSOR_PER_LED); i++) {
    detect[i] = 0;
  }
  detect[0] = 1;
  detect[(NB_SENSORS * NB_SENSOR_PER_LED) - 1] = 1;
}


void readSensors(int led, int sensornb, int nb) {
  int data = 0;
  int sensor;

  switch (sensornb) {
    case 0: sensor = 1; break;
    case 1: sensor = 0; break;
    case 2: sensor = 3; break;
    case 3: sensor = 2; break;
    case 4: sensor = 5; break;
    case 5: sensor = 4; break;
    case 6: sensor = 7; break;
    case 7: sensor = 6; break;

    case 8: sensor = 1; break;
    case 9: sensor = 0; break;
    case 10: sensor = 3; break;
    case 11: sensor = 2; break;
    case 12: sensor = 5; break;
    case 13: sensor = 4; break;
    case 14: sensor = 7; break;
    case 15: sensor = 6; break;

  }
  data = analogRead(sensor);
  if (data < THRESHOLD_LOW) {
    sensorsValues[led][nb] = 100;
  }
  else if (data > 1000) {
    sensorsValues[led][nb] = 999;
  }
  else if (data > THRESHOLD_HIGH) {
    sensorsValues[led][nb] = 999;
  }
  else {
    sensorsValues[led][nb] = data;
  }
}

void LED_IR(int numled) {

  char STP_config[4] = {0, 0, 0, 0};
  switch (numled) {

    case 7: STP_config[0] = 0x00; STP_config[1] = 0x00; STP_config[2] = 0x01; STP_config[3] = 0x00; break;
    case 6: STP_config[0] = 0x00; STP_config[1] = 0x00; STP_config[2] = 0x02; STP_config[3] = 0x00; break;
    case 5: STP_config[0] = 0x00; STP_config[1] = 0x00; STP_config[2] = 0x04; STP_config[3] = 0x00; break;
    case 4: STP_config[0] = 0x00; STP_config[1] = 0x00; STP_config[2] = 0x08; STP_config[3] = 0x00; break;
    case 3: STP_config[0] = 0x00; STP_config[1] = 0x00; STP_config[2] = 0x10; STP_config[3] = 0x00; break;
    case 2: STP_config[0] = 0x00; STP_config[1] = 0x00; STP_config[2] = 0x20; STP_config[3] = 0x00; break;
    case 1: STP_config[0] = 0x00; STP_config[1] = 0x00; STP_config[2] = 0x40; STP_config[3] = 0x00; break;
    case 0: STP_config[0] = 0x00; STP_config[1] = 0x00; STP_config[2] = 0x80; STP_config[3] = 0x00; break;
    case 8: STP_config[0] = 0x00; STP_config[1] = 0x00; STP_config[2] = 0x00; STP_config[3] = 0x01; break;
    case 9: STP_config[0] = 0x00; STP_config[1] = 0x00; STP_config[2] = 0x00; STP_config[3] = 0x02; break;
    case 10: STP_config[0] = 0x00; STP_config[1] = 0x00; STP_config[2] = 0x00; STP_config[3] = 0x04; break;

    case 11: STP_config[0] = 0x00; STP_config[1] = 0x00; STP_config[2] = 0x00; STP_config[3] = 0x08; break;

    case 12: STP_config[0] = 0x00; STP_config[1] = 0x00; STP_config[2] = 0x00; STP_config[3] = 0x10; break;
    case 13: STP_config[0] = 0x00; STP_config[1] = 0x00; STP_config[2] = 0x00; STP_config[3] = 0x20; break;
    case 14: STP_config[0] = 0x00; STP_config[1] = 0x00; STP_config[2] = 0x00; STP_config[3] = 0x40; break;
    case 15: STP_config[0] = 0x00; STP_config[1] = 0x00; STP_config[2] = 0x00; STP_config[3] = 0x80; break;
  }

  //send_data(STP_config[0]);
  //send_data(STP_config[1]);
  send_data(STP_config[3]);
  send_data(STP_config[2]);
  latch();
}

void send_data(char data) {
  for (int i = 0; i < 8; i++) {
    if (data & 0x01 << i) {
      digitalWrite(PIN_SDI, HIGH);
      //delayMicroseconds(10);
    }
    digitalWrite(PIN_CLK, HIGH);
    //delayMicroseconds(10);
    //delay(500);
    digitalWrite(PIN_SDI, LOW);
    digitalWrite(PIN_CLK, LOW);
    //delay(500);
  }
}

void latch(void) {
  digitalWrite(PIN_LE, HIGH);
  //delayMicroseconds(10);
  digitalWrite(PIN_LE, LOW);
}



