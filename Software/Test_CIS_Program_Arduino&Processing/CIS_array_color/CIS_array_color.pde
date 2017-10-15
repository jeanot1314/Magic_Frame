
/*
This program show a line of 20 square to represent the colors of 20 pixels 
received by the Arduino plugged on the CIS
The datas are received like this : 
data 0 = 1026
data 1 = pixel 1
data 2 = pixel 2
...
data 20 = pixel 20
data 21 = 1027
*/

import processing.serial.*;
Serial port;

String buff = "";
int cursor1,cursor2;
int[] tab= {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};
int cpt=0;
int val;
int[] values;
int flag=0;
 
void setup(){
  size(1000,600);
  println("Available serial ports:");
  println(Serial.list());
  port = new Serial(this, Serial.list()[4], 9600);
  println(Serial.list()[3]);
  values = new int[width];
  port.bufferUntil('\n'); 
  rectMode(CENTER); 
}

int getY(int val) {
  return (int)(val / 1023.0f * height) - 1;
}

void draw(){ 
  background(240, 240, 240);
  
  while (port.available() >= 1) {
    if (port.read() == 0xff) {
      val = (port.read() << 8) | (port.read());
      //print("value :"); 
      //println(val);
      
    }
    tab[cpt] = val;
    print(cpt);
    print("  ");
    println(val);
    if(val == 1026)
    {
      cpt=0; 
    }
    if(cpt == 21)
    {
      cpt=0;
    }
    cpt++;
    //flag=0;
  }

  if(tab[21] == 1027 && tab[20] <= 1024){  
    //fill(0); 
    //rect(width*0.29, height*0.9, width/100*2, width/100*2);
    for(int i=2; i<19;i++){
      //fill(tab[i]/4);  // convert the 1024 analog 10bits to 255 8bits values
      //rect(width*0.9, height/12 + i*24, 24, 24);
      for(int j=1; j<=3; j++){
        //stroke(150, 150, 150);
        fill(150, 150, 150);
        if(tab[i]<= 650){
          //line(width*0.15, height*0.5,  width*0.9, height/12 + i*24);
          triangle(width*0.15, height*0.5,  width*0.9, height/12 + i*24 - 12, width*0.9, height/12 + i*24 +12);
        }
      }
    }
    for(int i=2; i<19;i++){
      
      if(tab[i]/4 < 125){
        fill(0);
      }else{
        fill(tab[i]/4);  // convert the 1024 analog 10bits to 255 8bits values
      }
      rect(width*0.9, height/12 + i*24, 24, 24);
    }
    //fill(255);
    //rect(width*0.71, height*0.9, width/100*2, width/100*2); 
  
  }
  stroke(0,0,0);
  drawscreen();
}  

void drawscreen(){
  //fill(0,0,255);
  //text("Detection touch screen",width/2-65,20);
  rectMode(CENTER);  
  fill(200, 100, 200);
  rect(width*0.15, height*0.5, 40, 40);
}

void drawcursor(){
}

void mousePressed(){
}