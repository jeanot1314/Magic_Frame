

#define ANALOG_PIN A9
#define SENSOR_LE 2
#define SENSOR_CLK 3
#define LED_DEBUG 13
#define TIME 200

#define NB_PIXELS 20
#define NB_PIXELS_MAX 2700

int tab[NB_PIXELS_MAX];

void setup() {
  pinMode(SENSOR_LE, OUTPUT); 
  pinMode(SENSOR_CLK, OUTPUT);
  pinMode(LED_DEBUG, OUTPUT);
  //pinMode(8, OUTPUT); // turn a LED on 
  //digitalWrite(8, LOW);  
  Serial.begin(9600);
  
}

void loop() {

  clearData();

  clear_CIS();  

  read_CIS();

  send_processing();     
  //print_message();
  //digitalWrite(13, !digitalRead(13));
  delay(TIME);
}


void clearData(){
  for(int i=0; i<NB_PIXELS_MAX; i++){
    tab[i]=0;
  }
}

void print_message(){
  for(int i=0; i<NB_PIXELS; i++){
    Serial.print(tab[i]);
    Serial.print("  ");
  }
  Serial.println();
}

void send_processing(){

  int val = 1026; // First data,
  Serial.write( 0xff);
  Serial.write( (val >> 8) & 0xff);
  Serial.write( val & 0xff);
  delay(1);
  
  for(int i=0; i<NB_PIXELS; i++){
    val = tab[i]; 
    Serial.write( 0xff);
    Serial.write( (val >> 8) & 0xff);
    Serial.write( val & 0xff);
    delay(1);
  }
  
  val = 1027; // last data
  Serial.write( 0xff);
  Serial.write( (val >> 8) & 0xff);
  Serial.write( val & 0xff);
  //delay(TIME); 
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


