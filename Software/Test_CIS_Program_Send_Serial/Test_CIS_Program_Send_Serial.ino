

#define ANALOG_PIN A9
#define SENSOR_LE 2
#define SENSOR_CLK 3
#define LED_DEBUG 13
#define TIME 200

#define NB_PIXELS 22
#define NB_PIXELS_MAX 2700

#define sensibility_1 650
#define sensibility_2 750
#define sensibility_3 900


int tab[NB_PIXELS_MAX];

void setup() {
  pinMode(SENSOR_LE, OUTPUT); 
  pinMode(SENSOR_CLK, OUTPUT);
  pinMode(LED_DEBUG, OUTPUT);
  Serial.begin(9600);
/*
  for(int i=0; i<16; i++){
    pinMode(i+6, OUTPUT);
    digitalWrite(i+6, HIGH);
  }
  digitalWrite(13, LOW);
  digitalWrite(14, LOW);
  */
  digitalWrite(LED_DEBUG, HIGH);
}

void loop() {
  
  clearData();
  
  clear_CIS();
  read_CIS();
  
  //digitalWrite(LED_DEBUG, !digitalRead(LED_DEBUG));
  
  send_data_serial(); 
  
  delay(TIME);
}



void clearData(){
  for(int i=0; i<NB_PIXELS_MAX; i++){
    tab[i]=0;
  }
}

void send_data_serial(){
  
  for(int i=NB_PIXELS; i>0; i--){
    
    if(tab[i] >= sensibility_3){
      Serial.print("XXX");
    }
    else if(tab[i] >= sensibility_2){
      Serial.print("XX ");
    }
    //else if(tab[i] >= sensibility_1){
    //  Serial.print(" X ");
    //}
    else{
      Serial.print(tab[i]);
    }
    Serial.print("  ");
  }
  Serial.println();
}

void read_CIS(){

  digitalWrite(SENSOR_LE, HIGH);
  digitalWrite(SENSOR_CLK, HIGH);            
  digitalWrite(SENSOR_CLK, LOW); 
  digitalWrite(SENSOR_LE, LOW);

  for(int i=0; i<NB_PIXELS; i++){
    int cpt = 0;
    for(int j=0; j<(NB_PIXELS_MAX/NB_PIXELS);j++){
      digitalWrite(SENSOR_CLK, HIGH);        
      digitalWrite(SENSOR_CLK, LOW);
      cpt += analogRead(ANALOG_PIN);
    }
    tab[i] = cpt/(NB_PIXELS_MAX/NB_PIXELS);
  }
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






