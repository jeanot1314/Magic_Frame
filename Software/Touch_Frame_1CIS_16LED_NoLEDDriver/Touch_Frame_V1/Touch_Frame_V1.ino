
//=======================================
//====Touch Program table program========
//=======================================


/*
Speed test with 16 LED et 100 Pixels : 72ms so almost 14Hz
Vitesse with 2 analog : 88ms so 11.3 Hz
You have to choose the number of LEDs and the output you want
*/

/*
Choose the OPTION you want : 

OPTION 1 : Send the sensor data on the serial port for every LED (no position calcul). Very useful for debug. 
OPTION 2 : Send the positions on the serial port
OPTION 3 : Send the sensor data on the Processing interface for debugging (no position calcul)
OPTION 4 : Send the positions on Python GUI  
OPTION 5 : Send the position as a mouse. You need to activate this option on the tools menu for the Teensy or Leonardo board
OPTION 6 : Send the position as a keyboard. You need to activate this option on the tools menu for the Teensy or Leonardo board
OPTION 7 : Send the position as a mouse. "Point and click mode"
*/

#define OPTION 3 // Choose the output option here

#define NB_POINT_MAX 4 // Multi-touch, max number of position

#define CALIBRATION_MIN 500
#define CALIBRATION_MAX 700 // When a finger is on the table, the value of the sensor area drop down between those 2 values

#define SCREEN_WIDTH 800.00 // mm
#define SCREEN_HEIGHT 393.00 // 26.2mm * 15 
#define CIS_HEIGHT 220.00 // mm
#define DISTANCE_BETWEEN_LED 26.20 // mm

#define NB_LED 16
#define NB_PIXELS 50 // Pixels / sensor. Adapt to control your speed  and precision 
#define NB_PIXELS_MAX 2700
#define NB_BLACK_ZONE_MAX 10 // Multi-touch, max number of position / LED

#define DELTA 45 // Multi-touch, what is the area of a touch in mm
#define DELTA_ANGLE 0.01 // remove detection of black zone too close (safety) 


#define ANALOG_PIN_Sensor1 A9 // Analog Pin of the first sensor
#define ANALOG_PIN_Sensor2 A8 // Analog pin of the second sensor
#define SENSOR_LE 2 // Latch pin of the CISs
#define SENSOR_CLK 3 // Clock pin os the CISs

#define TIME 0 // Slow down the all program, useful for option 1, 2 and 3. 
#define PROCESSING_DELAY 100

 //First tab of storage from the CIS sensor
int tab[NB_LED][NB_PIXELS];

// Second tab with the data Y=AX+B with [led][0] = B and [led][1] = A ([led][2] = A' for the multi-touch)
// it need to be volatile, as the B value are defined in the setup
float tab_position[NB_LED][NB_BLACK_ZONE_MAX+1][2];

//Third tab to store the different position : 0 = sommeX, 1 = sommeY, 2 = referenceX, 3 = referenceY, 4 = cpt_intersections
//It need to be volatile as the value are used in the "send" functions 
long position[NB_POINT_MAX][3];


void setup() {
  init_Pin();

  Serial.begin(9600);

#if OPTION == 5
  Mouse.screenSize(SCREEN_WIDTH, SCREEN_HEIGHT);  // configure screen size for mouse mode
#endif
  
}

void loop() {
  //initialize the tab
  init_tab();
  boolean flag_detect = 0;

  //for(int i = 0; i < 100; i++){ // Speed test

  for(int l=0; l<NB_LED; l++){
    //Turn ON a LED (as it's common anode, LOW is on and HIGH is OFF)
    LED_IR(l, LOW);
    // Clear the sensor, necessary, I don't know why...
    clear_CIS();
    // Read and store the data, return 1 if something was seen
    if(read_CIS(l) == 1){
      flag_detect = 1;
    }
    // Turn off the LED
    LED_IR(l, HIGH);
  }
  //}


  // if the CIS has a value under the MAX Calibration
  if(flag_detect == 1){
    //Choose the option you want on the #define OPTION X above
 
#if OPTION == 1
    send_data_serial(); // Send the sensor data on the serial port for every LED
#endif

#if OPTION == 2
    calcul_multi_position(); // calcul the positions of an object. store the X and Y of every point in position[][] tab
    send_position_serial(); // Send the position on serial
#endif    

#if OPTION == 3
    send_data_processing(); // send data on the Processing interface for debugging
#endif  

#if OPTION == 4
    calcul_multi_position(); // calcul the positions of an object. store the X and Y of every point in position[][] tab
    send_data_python(); // envoi de la position sur python pour le GUI final  
#endif

#if OPTION == 5    
    calcul_multi_position(); // calcul the positions of an object. store the X and Y of every point in position[][] tab
    send_mouse(); // send the data as a mouse. You need to activate this option on the tools menu on the Teensy  
#endif

#if OPTION == 6    
    calcul_multi_position(); // calcul the positions of an object. store the X and Y of every point in position[][] tab
    send_keyboard(); // send the data as a keyboard. You need to activate this option on the tools menu on the Teensy
#endif

#if OPTION == 7    
    calcul_multi_position(); // calcul the positions of an object. store the X and Y of every point in position[][] tab
    send_mouse_click(); // send the data as a mouse. You need to activate this option on the tools menu on the Teensy  
#endif
  }

#if OPTION == 6 
  else{
      Keyboard.release(KEY_LEFT);
      Keyboard.release(KEY_UP);
      Keyboard.release(KEY_DOWN);
      Keyboard.release(KEY_RIGHT);
      Keyboard.release(KEY_Q);
      Keyboard.release(KEY_D);  
  }
#endif

  delay(TIME); // in case you want to slow down the code
}



void calcul_multi_position(){


//-------PART 1 : calculate the affine function Y = A * X +B and store A and B into tab_position 

  float positionX, positionY;
  int flag_reference=0; // Reference flag to switch between adding a new reference point or validating an existing point
  int cpt_reference=0;

  for(int i=0; i<NB_LED; i++){
    int mini = 0, maxi = 0; // store the minimal and maximal value of every black zone 
    int flag_delta = 0; //Check if we are at the starting of a black zone or at the end
    int k=0; //Number of black zone per LED. Increment the tab_position to store the affine "A" -> for the multi-point
    for(int j=0; j<NB_PIXELS; j++){
      if((CALIBRATION_MIN <= tab[i][j] && tab[i][j] <= CALIBRATION_MAX) || (flag_delta == 1 && tab[i][j] <= CALIBRATION_MAX + 150) ){ // éviter la perturbation d'un seul pixel ========================
        if(flag_delta == 0){
          mini = j;
          flag_delta = 1;
        }
        maxi = j;
      }
      else{
        if(flag_delta == 1){// && (maxi - mini) > NB_PIXELS/50){ //Safety you can add to avoid too small detection

          Serial.print("  LED : ");
          Serial.print(i+1);
          Serial.print("   Pixel : ");
          Serial.print(mini);
          Serial.print(" - ");
          Serial.print(maxi);
          Serial.print("  Value : ");
          Serial.print(tab[i][mini]); 
          Serial.print(" - ");
          Serial.println(tab[i][maxi]);

          if(k <= NB_BLACK_ZONE_MAX){
            k++; // increase the number of black zone per LED (for multi-touch)
          }
          //Calcul and store the "a" in Y = A*X + B so Y = tab_position[led][1&] * X + tab_position[led][0]
          // Here we have : a = (Y - B)/X                   

          tab_position[i][k][1] = maxi - mini;
          tab_position[i][k][0] = ((((((maxi+mini)/2)*CIS_HEIGHT)/NB_PIXELS)+(SCREEN_HEIGHT/2)-(CIS_HEIGHT/2) - tab_position[i][0][0]) / SCREEN_WIDTH);
        }
        flag_delta = 0;
      }
    }
  }  


//--------------PART 2 : check the crossing of the functions to calculate the detection points

  for(int i=0; i<NB_POINT_MAX; i++){
    for(int j=0; j<3; j++){
      position[i][j] = 0; 
    }  
  }

  int cpt_point = 0;

  for(int i=0; i<NB_LED; i++){ 
    int cpt = 0;
    float positionX_ref = 0;
    float positionY_ref = 0;

    for(int j=i+1; j<NB_LED; j++){ // the j=i+1 to don't interact twice the LED

      // If we have stored a value, we check if the other LED see it. If yes, we adapt the position and remove the function (to don't use it twice) 
      if(cpt >= 2){
        if(tab_position[j][1][0] != 0){

          float compareY = tab_position[j][1][0] * positionX_ref + (tab_position[j][0][0]);
          
          //if the Y of he new line is close to the reference
          if(positionY_ref - DELTA/2 <= compareY && compareY <= positionY_ref + DELTA/2){
   /*         
            Serial.print(i+1);
            Serial.print("   ");
            Serial.print(j+1);
            Serial.print("   ");
            Serial.print(cpt);
            Serial.print("   ");
            Serial.print(compareY);
            Serial.print("   ");
            Serial.print(positionY_ref);
            Serial.print("   ");
            Serial.print(tab_position[j][1][0]);
            Serial.print("   ");
            Serial.println(tab_position[j][0][0]);
   */
            positionY_ref = ((positionY_ref * cpt) + compareY)/ (cpt + 1); // adapt the reference position

            // We drop down the detection tab k to don't use the same detection twice
            for(int k=1; k< NB_BLACK_ZONE_MAX-1; k++){
              tab_position[j][k][0] = tab_position[j][k+1][0];
            }
            cpt++;           
          }
        }
      }

      // store a value 
      if(tab_position[i][1][0] != 0 && tab_position[j][1][0] != 0 && cpt == 0){

        // when we have two points we calculate the contact		
        positionX_ref = (tab_position[j][0][0] - tab_position[i][0][0]) / (tab_position[i][1][0] - tab_position[j][1][0]);
        positionY_ref = ((((tab_position[i][1][0])*(positionX_ref)) + tab_position[i][0][0]) + (((tab_position[j][1][0])*(positionX_ref)) + tab_position[j][0][0]))/2;
        //Serial.print(" j0 ");Serial.print(tab_position[j][0][0]);Serial.print(" i0 ");Serial.print(tab_position[i][0][0]);Serial.print(" i1 ");Serial.print(tab_position[i][1][0]);Serial.print(" j1 ");Serial.print(tab_position[j][1][0]);
        //Serial.print(" Xref : ");Serial.print(positionX_ref);Serial.print(" Yref : ");Serial.println(positionY_ref);
        // We remove the value used
        for(int k=1; k< NB_BLACK_ZONE_MAX-1; k++){   
          tab_position[i][k][0] = tab_position[i][k+1][0];
          tab_position[j][k][0] = tab_position[j][k+1][0];
        }
        cpt = 2; // the cpt has now two intersections
      }
    }  
    if(cpt >= 2){
      //if(positionX_ref < 350 || cpt > 5){ // you can add for debugging, 
        position[cpt_point][0] = cpt;
        position[cpt_point][1] = positionX_ref;
        position[cpt_point][2] = positionY_ref;
        cpt = -1;
        cpt_point++;
        /*
	Serial.print(" LED: ");
        Serial.print(i);
        Serial.println(" ");
        */
      //}
    }
    
  } 
}


void send_mouse(){

#if OPTION == 5 
  if(position[0][1] != 0 && position[0][2] !=0){

    position[0][1] = map(position[0][1], 0, SCREEN_WIDTH, SCREEN_WIDTH, 0); // map depending on your output need
    position[0][2] = map(position[0][2], 0, SCREEN_HEIGHT, SCREEN_HEIGHT, 0);

    Mouse.moveTo(position[0][1], position[0][2]);  
  }
#endif

}

void send_mouse_click(){

#if OPTION == 7 
  if(position[0][1] != 0 && position[0][2] !=0){

    position[0][1] = map(position[0][1], 0, SCREEN_WIDTH, SCREEN_WIDTH, 0); // map depending on your output need
    position[0][2] = map(position[0][2], 0, SCREEN_HEIGHT, SCREEN_HEIGHT, 0);

    Mouse.moveTo(position[0][1], position[0][2]);  
    Mouse.click(MOUSE_LEFT);  
  }
#endif

}


void send_keyboard(){ // interact as a keyboard, write the input/output you need

#if OPTION == 6
  Keyboard.release(KEY_LEFT);
  Keyboard.release(KEY_UP);
  Keyboard.release(KEY_DOWN);
  Keyboard.release(KEY_RIGHT);
  Keyboard.release(KEY_Q);
  Keyboard.release(KEY_D); 

  for(int i=0; i<NB_POINT_MAX; i++){
  
    if(position[i][1] != 0 && position[i][2] !=0){

      float positionXfinal_ref = position[i][1]/SCREEN_WIDTH;
      float positionYfinal_ref = position[i][2]/SCREEN_HEIGHT;
  

      if(0.5 < positionXfinal_ref && positionXfinal_ref < 0.75 && positionYfinal_ref < 0.5){
        if((positionXfinal_ref-0.5) <= positionYfinal_ref/4){
          Keyboard.press(KEY_RIGHT);
        }else{
          Keyboard.press(KEY_DOWN);
        }
      }else if(positionXfinal_ref > 0.75 && positionYfinal_ref < 0.5){
        if(((positionXfinal_ref-0.75) + positionYfinal_ref/4) <= 0.25){
          Keyboard.press(KEY_DOWN);
        }else{
          Keyboard.press(KEY_LEFT);
        }
      }else if(0.5 < positionXfinal_ref && positionXfinal_ref < 0.75 && positionYfinal_ref > 0.5){
        if(((positionXfinal_ref-0.5) + (positionYfinal_ref-0.5)/2) <= 0.25){
          Keyboard.press(KEY_RIGHT);
        }else{
          Keyboard.press(KEY_UP);
        }
      }else if(positionXfinal_ref > 0.75 && positionYfinal_ref > 0.5){
        if((positionXfinal_ref-0.75) <= (positionYfinal_ref-0.5)/2){
          Keyboard.press(KEY_UP);
        }else{
          Keyboard.press(KEY_LEFT);
        }
      }else if(5*SCREEN_WIDTH/10 > position[i][1] && position[i][1] >= 3*SCREEN_WIDTH/10 && position[i][2] < SCREEN_HEIGHT/2){
        Keyboard.press(KEY_D);

      }else if(3*SCREEN_WIDTH/10 > position[i][1] && position[i][2] < SCREEN_HEIGHT/2){
        Keyboard.press(KEY_Q); 

      }else if(5*SCREEN_WIDTH/10 > position[i][1] && position[i][1] >= 3*SCREEN_WIDTH/10 && position[i][2] > SCREEN_HEIGHT/2){
        Keyboard.press(KEY_S);

      }else if(3*SCREEN_WIDTH/10 > position[i][1] && position[i][2] > SCREEN_HEIGHT/2){
        Keyboard.press(KEY_E); 
      }
    }
  }
#endif
}


void send_data_python(){
  for(int i=0; i<NB_POINT_MAX; i++){
    if(position[i][1] != 0 && position[i][2] !=0){
      Serial.print("XX"); 
      Serial.print(position[i][1]);
      Serial.print("XX");
      Serial.print(position[i][2]);
      Serial.println("XX");
    }
  }
}


void send_position_serial(){

  Serial.println("-------------");
  for(int i=0; i<NB_POINT_MAX; i++){
    if(position[i][1] != 0 && position[i][2] !=0){

      Serial.print("  X: ");
      Serial.print(position[i][1]);
      Serial.print("  Y: ");
      Serial.print(position[i][2]);
    }
  }
  Serial.println();
}


void send_data_serial(){

  Serial.println("------------------------------");
  for(int j=0; j<NB_LED; j++){
    int mini = 0, maxi = 0, flag_delta = 0;
    for(int i=0; i<NB_PIXELS; i++){
      if(CALIBRATION_MIN <= tab[j][i] && tab[j][i] <= CALIBRATION_MAX){
        if(flag_delta == 0){
          mini = i;
          flag_delta = 1;
        }
        maxi = i;
      }
      else{
        if(flag_delta == 1){
          Serial.print("  LED : ");
          Serial.print(j+1);
          Serial.print("   Pixel : ");
          Serial.print(mini);
          Serial.print(" - ");
          Serial.print(maxi);
          Serial.print("  Value : ");
          Serial.print(tab[j][mini]); 
          Serial.print(" - ");
          Serial.println(tab[j][maxi]);
        }
        flag_delta = 0;
      }
    }
  }    
}


void send_data_processing(){

  int val = 111; // Start a communication 
  Serial.write( 0xff);
  Serial.write( val & 0xff);
  delay(1);

  for(int j=0; j<NB_LED; j++){
    int mini = 0, maxi = 0, flag_delta = 0;  // have to test this 
    for(int i=NB_PIXELS-1; i>=0; i--){
      if(CALIBRATION_MIN <= tab[j][i] && tab[j][i] <= CALIBRATION_MAX){
        if(flag_delta == 0){
          mini = i;
          flag_delta = 1;
        }
        maxi = i;
      }
      else{
        if(flag_delta == 1){
          Serial.write( 0xff);
          Serial.write( j & 0xff); 
          Serial.write( mini & 0xff);
          Serial.write( maxi & 0xff);
        } 
        flag_delta = 0;
      }
    }
  }

  val = 112; // end of communication
  Serial.write( 0xff);
  Serial.write( val & 0xff);
  delay(PROCESSING_DELAY);
}

void init_tab(){

  for(int i=0; i<NB_LED; i++){
    for(int j=0; j<NB_PIXELS; j++){
      tab[i][j]=0;
    }
  }

  for(int i=0; i<NB_LED; i++){
    tab_position[i][0][0]= i * DISTANCE_BETWEEN_LED;
    for(int j=0; j<NB_BLACK_ZONE_MAX; j++){
      for(int k=0; k < 2; k++){
        tab_position[i][j+1][k]=0;
      }
    }
  }
}


boolean read_CIS(int led_on){
  boolean flag_detect = 0;

  digitalWrite(SENSOR_LE, HIGH);
  digitalWrite(SENSOR_CLK, HIGH);            
  digitalWrite(SENSOR_CLK, LOW); 
  digitalWrite(SENSOR_LE, LOW);

  for(int i=0; i<NB_PIXELS; i++){
    for(int j=0; j<(NB_PIXELS_MAX/NB_PIXELS);j++){  // (2700 / 50) = 54
      digitalWrite(SENSOR_CLK, HIGH);        
      digitalWrite(SENSOR_CLK, LOW); 
    }
    
    tab[led_on][NB_PIXELS - 1 -i] = analogRead(ANALOG_PIN_Sensor1);
    //tab[led_on][i] = analogRead(ANALOG_PIN_Sensor1);
    //tab[led_on][(NB_PIXELS*2)-1-i] = analogRead(ANALOG_PIN_Sensor2); // uncomment to use the second sensor
    
    if(CALIBRATION_MIN <= tab[led_on][NB_PIXELS - 1 -i] && tab[led_on][NB_PIXELS - 1 -i] <= CALIBRATION_MAX){
      flag_detect = 1;
    }
  }
  return flag_detect;
}


void init_Pin(){
  for(int i = 6; i < 22; i++){
    pinMode(i, OUTPUT);
    digitalWrite(i, HIGH);
  }
  pinMode(SENSOR_LE, OUTPUT); 
  pinMode(SENSOR_CLK, OUTPUT);
}


void LED_IR(int numled, boolean state){
  digitalWrite(numled + 6, state); //as my LED number is 0 but the pin start at 6
}


void clear_CIS(){

  digitalWrite(SENSOR_LE, HIGH);
  digitalWrite(SENSOR_CLK, HIGH); 
  digitalWrite(SENSOR_CLK, LOW);
  digitalWrite(SENSOR_LE, LOW);

  for(int i = 0; i<NB_PIXELS_MAX; i++){ 
    digitalWrite(SENSOR_CLK, HIGH);    
    digitalWrite(SENSOR_CLK, LOW); 
  } 
}






