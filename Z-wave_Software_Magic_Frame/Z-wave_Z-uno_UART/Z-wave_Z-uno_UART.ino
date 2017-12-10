
// 13 pin - user LED of Z-Uno board
#define LED_PIN 13
// Potentiometer pin number
#define POT_PIN 6

// channel number
#define ZUNO_CHANNEL_NUMBER_ONE   1

// Last saved potentiometer value
byte lastValue;
boolean flagStart = 0;;

ZUNO_SETUP_CHANNELS(ZUNO_SENSOR_MULTILEVEL_GENERAL_PURPOSE(getter));

// the setup routine runs once when you press reset:
void setup() {
  Serial1.begin(9600);
  Serial.begin(9600);
  pinMode(LED_PIN, OUTPUT); // setup pin as output
  pinMode(POT_PIN, INPUT);  // setup potentiometer pin as input
}
// the loop routine runs over and over again forever:
void loop() {
  byte header = 0x00;
  
  if (Serial1.available() > 0) {
    header = Serial1.read();
    delay(1);
    Serial.println("Value detected on serial");
    if (header == 0xFF) {
      lastValue = Serial1.read();
      Serial.print("The value is : ");
      Serial.println(lastValue);
      zunoSendReport(ZUNO_CHANNEL_NUMBER_ONE);
      delay(100);
    }
  }
}

byte getter(void) {
  byte tempVariable;
  tempVariable = (byte)((((word) lastValue) * 100) / 0xff);
  return tempVariable;
}
