#include <Mouse.h>
#include <Keyboard.h>
//#include <Joystick.h> // the joystick is now transmit on serial

#include <SPI.h> //Call SPI library so you can communicate with the nRF24L01+
#include <nRF24L01.h>
#include <RF24.h>

#define button 4

void read_Bluetooth();
void print_Bluetooth();
void send_Serial(byte id_num);

//Bluetooth
byte ID = 0;
unsigned char bt_rx[4]; // The 4 byte reception bluetooth buffer
unsigned char bt_tx = '1'; // Calibration data
const int pinCE = 9; //This pin is used to set the nRF24 to standby (0) or active mode (1)
const int pinCSN = 10; //This pin is used to tell the nRF24 whether the SPI communication is a command or message to send out
RF24 wirelessSPI(pinCE, pinCSN); // Declare object from nRF24 library (Create your wireless SPI)

// Here we have 5 bluetooth nodes communicating with the central. The first address is used for the Wii Nunchuck
//Create pipe addresses for the 5 nodes to recieve data, the "LL" is for LongLong type
const uint64_t rAddress[] = {0xB00B1E50C3LL, 0xB00B1E50D6LL, 0xB00B1E50D2LL, 0xB00B1E50A6LL, 0xB00B1E50A0LL};
//Create pipe addresses for the 5 nodes to transmit data, the "LL" is for LongLong type
const uint64_t wAddress[] = {0xB00B1E50A4LL, 0xB00B1E50B5LL, 0xB00B1E50B1LL, 0xB00B1E50A8LL, 0xB00B1E50A1LL};

void setup()
{
  pinMode(button, INPUT); // Safety button, in case of HID debugging
  digitalWrite(button, LOW);

  Serial.begin(9600);  //start serial to communication

  wirelessSPI.begin();  //Start the nRF24 module
  wirelessSPI.openReadingPipe(1, rAddress[0]);     //open pipe o for recieving meassages with pipe address
  wirelessSPI.openReadingPipe(2, rAddress[1]);     //open pipe o for recieving meassages with pipe address
  wirelessSPI.openReadingPipe(3, rAddress[2]);     //open pipe o for recieving meassages with pipe address
  wirelessSPI.openReadingPipe(4, rAddress[3]);     //open pipe o for recieving meassages with pipe address
  wirelessSPI.openReadingPipe(5, rAddress[4]);     //open pipe o for recieving meassages with pipe address
  wirelessSPI.startListening();                 // Start listening for messages
}

void loop()
{
  ID = 0;
  read_Bluetooth();
  if (digitalRead(button) == HIGH && 1 <= ID && ID <= 10) { // From nodes 1 to 10
    //print_Bluetooth();
    send_Serial(ID);
  }
  delay(1); // check if necessary
}


void send_Serial(byte id_num) {

  int tmp[4];
  tmp[0] = (unsigned char)bt_rx[0];
  tmp[1] = (unsigned char)bt_rx[1];
  tmp[2] = (unsigned char)bt_rx[2];
  tmp[3] = (unsigned char)bt_rx[3];

  if (id_num == 1  && bt_rx[3] == 0) {
    Serial.print('J');
    Serial.print(','); // before the joystick didn't had any virgule, check if still ok
  }
  if (id_num == 2) {
    Serial.print('A');
    Serial.print(',');
  }
  if (id_num == 3) {
    Serial.print('B');
    Serial.print(',');
  }
  if (id_num == 4) {
    Serial.print('C');
    Serial.print(',');
  }
  if (id_num == 5) {
    Serial.print('D');
    Serial.print(',');
  }
  // Here we can add some node if necessary
  if ((2 <= id_num && id_num <= 10) || (id_num == 1  && bt_rx[3] == 0)) {
    Serial.print(tmp[0]);
    Serial.print(",");
    Serial.print(tmp[1]);
    Serial.print(",");
    Serial.print(tmp[2]);
    Serial.print(",");
    Serial.print(",");
    Serial.print('\n');

    delay(20); //***** check if necessary
  }

  if (id_num == 1 && bt_rx[3] == 1)
  {
    calibrate_nodes();
  }
}

void calibrate_nodes() {
  int timeout = 100; // To have a non blocking calibration **** need to check the timeout
  wirelessSPI.stopListening(); //Stop listening, stop recieving data.

  for (int i = 1; i < 5; i++) {
    wirelessSPI.openWritingPipe(wAddress[i]);
    timeout = 100;
    while (!wirelessSPI.write(&bt_tx, 1) && timeout > 0) {
      //Serial.print("Failed sending ");
      //Serial.println(i);
      //delayMicroseconds(100);
      delay(1);
      timeout--;
    }
    if (timeout > 0 ) {
      //Serial.print("Success sending ");
      //Serial.println(i);
    } else {
      //Serial.println("FAILED 0");
    }
  }
  wirelessSPI.startListening(); //Switch back to a reciever
}

void read_Bluetooth() {
  while (wirelessSPI.available(&ID)) { //Check if recieved data
    wirelessSPI.read(&bt_rx, 4); //read one byte of data and store it in gotByte variable
  }
}

void print_Bluetooth() {

  Serial.print("BT rx : ");
  Serial.print(ID);
  Serial.print(" - ");
  Serial.print(bt_rx[0], DEC);
  Serial.print(" - ");
  Serial.print(bt_rx[1], DEC);
  Serial.print(" - ");
  Serial.print(bt_rx[2], DEC);
  Serial.print(" - ");
  Serial.print(bt_rx[3], DEC);
  Serial.println();
}


