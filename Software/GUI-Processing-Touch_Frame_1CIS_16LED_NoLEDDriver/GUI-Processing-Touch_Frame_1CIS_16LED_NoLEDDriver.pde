
/*
This code is to show the positions of the black zone for the CCD sensor for every LED 
It's been tested on a Macbook and on a Raspberry Pi
It work with the touch screen Arduino code with the OPTION 3 activated
*/


import processing.serial.*;
Serial port;

String buff = "";
//int dataPresent;
int cursor1,cursor2;
int[][] tab= {
              {0,0,0},{0,0,0},{0,0,0},{0,0,0},{0,0,0},{0,0,0},{0,0,0},{0,0,0},{0,0,0},
              {0,0,0},{0,0,0},{0,0,0},{0,0,0},{0,0,0},{0,0,0},{0,0,0},{0,0,0},{0,0,0},
              {0,0,0},{0,0,0},{0,0,0},{0,0,0},{0,0,0},{0,0,0},{0,0,0},{0,0,0},{0,0,0},
              {0,0,0},{0,0,0},{0,0,0},{0,0,0},{0,0,0},{0,0,0},{0,0,0},{0,0,0},{0,0,0},
              {0,0,0},{0,0,0},{0,0,0},{0,0,0},{0,0,0},{0,0,0},{0,0,0},{0,0,0},{0,0,0},
              {0,0,0},{0,0,0},{0,0,0},{0,0,0},{0,0,0},{0,0,0},{0,0,0},{0,0,0},{0,0,0},
              {0,0,0},{0,0,0},{0,0,0},{0,0,0},{0,0,0},{0,0,0},{0,0,0},{0,0,0},{0,0,0},
              {0,0,0},{0,0,0},{0,0,0},{0,0,0},{0,0,0},{0,0,0},{0,0,0},{0,0,0},{0,0,0},
              {0,0,0},{0,0,0},{0,0,0},{0,0,0},{0,0,0},{0,0,0},{0,0,0},{0,0,0},{0,0,0},
              {0,0,0},{0,0,0},{0,0,0},{0,0,0},{0,0,0},{0,0,0},{0,0,0},{0,0,0},{0,0,0},
              {0,0,0},{0,0,0},{0,0,0},{0,0,0},{0,0,0},{0,0,0},{0,0,0},{0,0,0},{0,0,0}
            };
int cpt=0;
int val, led, mini, maxi;
int[] values;
int flag=0;
 
void setup(){
  size(1000,1800); // Adapt the screen size for your need, here is for my 32 inch TV in rotate mode 90°
  println("Available serial ports:");
  println(Serial.list());
  port = new Serial(this, Serial.list()[0], 9600); // Change the serial port to correspond to your USB (here 0). The list will be print in the serial debug
  println(Serial.list()[0]);
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
    if (port.read() == 0xff) { // if we receive 0xFF, we read the next byte. 
      led = port.read(); 
      if(led == 111 || cpt >= 80){cpt=0; clearTable();} // If the byte is 111 it's the start of a communication
      
      if(led < 16){ 
        cpt++;
        tab[cpt][0] = led;
        tab[cpt][1] = port.read(); 
        tab[cpt][2] = port.read();
        print("Case : "); //Print in the monitor the data received
        print(cpt); print("    LED : ");
        print(tab[cpt][0]); print("  ");
        print(tab[cpt][1]); print("  ");
        println(tab[cpt][2]); 
      }
      if(led == 112){break;} // the 112 mean it's the end of communication
    }
  }
  
  if(led == 112){
    //println("Enter line mode");
    for(int i=1;i<50;i++){
      if(0 <= tab[i][0] && tab[i][0] < 16 && tab[i][1]>0){
        stroke(150, 150, 150);
        fill(150, 150, 150);
        triangle(width - (tab[i][0])*width/15, height*0.01, ((width*(50-tab[i][1])/50 )*0.6)+ width*0.2, height*0.99, ((width*(50-tab[i][2])/50)*0.6)+ width*0.2, height*0.99);
	//draw the shadow triangles 
      }
    }
    for(int i=1;i<50;i++){
      if(0 <= tab[i][0] && tab[i][0] < 16 && tab[i][1]>0){
        stroke(250, 0, 0);
        line(width - (tab[i][0])*width/15, height*0.01, ((width*(50-((tab[i][1] + tab[i][2])/2))/50)*0.6)+ width*0.2, height*0.99);
	// Draw the red lines here 
      }
    }
    //delay(500);
    //fill(0);
    //led = 0;
  } 
  stroke(0,0,0);
  drawscreen();
}  

void clearTable(){ 
  for(int i=0; i< 50;i++){
    tab[i][0] = 0;
    tab[i][1] = 0;
    tab[i][2] = 0;
  }
}

void drawscreen(){
  fill(0,0,255);
  text("Detection touch screen",width/2-65,20);
  rectMode(CENTER);  
  fill(200, 100, 200);
  for(int i=0; i<= 15;i++){
    rect((i)*width/15, height*0.01, 20, 20); // Draw the 16 LED pink rectangle 
  }
  fill(180, 180, 180);
  rect( width*0.5, height*0.99, width*0.6, height*0.04); // Draw the sensor rectangle 
  noFill();
  rect( width*0.5, height*0.5, width*0.5, height*0.72);
  line(width*0.25, height*0.5, width*0.75, height*0.5);
}


