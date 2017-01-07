/*
Digital Torque Wrench
	Using PARALLEL BEAM LOAD CELL TAL220(10Kg), Load Cell Amp HX711 Arduino Pro mini
	and Arduino IIC/I2C/TWI 1602 Serial LCD Module Display
See https://www.sparkfun.com/products/13329
Coded by M.Miyazaki
Date: 2017/01/06
*/
 
 // https://github.com/bogde/HX711
#include "HX711.h"
const int DT_PIN = A1;
const int SCK_PIN = A0;
HX711 scale;

//i2c LCD
#include <Wire.h>
#include "rgb_lcd.h"
rgb_lcd lcd;
float measmnt;

//Buzzer
#include <MsTimer2.h>
int td ;  // variable period [mSec]
int steps = 20; // steps of period of Buzzer
int tdwarnW = 500;  // warnning weight period of Buzzer[mSec]
int tdmaxW = 50;  // max weight period of Buzzer[mSec]
float stepPeriod = (tdwarnW - tdmaxW) / steps;
int maxWeight = 3000;
int warnWeight =1000;
float stepWeight = (maxWeight - warnWeight)/ steps;
int messageCnt=0;

void buzzer() {
  static boolean output = HIGH;
  if( int(measmnt) >= maxWeight ){	//Continuous sound
  	 analogWrite(6, 25);		// Almost any value can be used except 0 and 255
  } else if (int(measmnt) >= warnWeight){	//Intermittent sound varied with the measurement of the load
  	td = tdwarnW -int((measmnt - float(warnWeight))/stepWeight*stepPeriod);
    MsTimer2::set(td, buzzer);   // set new period
    MsTimer2::start();           // start again
	if (output == HIGH){
		analogWrite(6, 25);     // Almost any value can be used except 0 and 255
  	} else {
  		analogWrite(6, 0);
  	}
  	output = !output;
  } else {							//No sound
  	analogWrite(6, 0);
  }
}

void setup() {
	Serial.begin(38400);

    // set up the LCD's number of columns and rows:
    lcd.begin(16, 2);
    lcd.clear();
    
    lcd.print("start");
	Serial.println("start");
	scale.begin(DT_PIN, SCK_PIN);
	Serial.print("read:");
	Serial.println(scale.read());

	scale.set_scale();
	scale.tare();

	Serial.print("calibrating...");
    lcd.setCursor(0, 2);
	lcd.print("calibrating...");
	delay(5000);
	Serial.println(scale.get_units(10));

	scale.set_scale(436.00);
	scale.tare();

	Serial.print("read (calibrated):");
	Serial.println(scale.get_units(10));

  	pinMode(6, OUTPUT);
	MsTimer2::set(td, buzzer); 
	MsTimer2::start();

}


void loop() {
	measmnt = scale.get_units(5);
	Serial.println(measmnt, 1);
    // set the cursor of LCD to (0,0):
    lcd.clear();
    lcd.print("max:");
    lcd.print(maxWeight/1000);
    lcd.print("Kg,");
    lcd.print(measmnt,1);
    lcd.print("g");
	if( int(measmnt) >= maxWeight ){	//blink characters of "Stop!!!"
		lcd.setCursor(0, 2);
		if( messageCnt < 1){
			lcd.print("Stop!!!");
			messageCnt++;
		} else if( messageCnt < 2){
			lcd.print(" ");
			messageCnt++;
		} else {
			messageCnt=0;
		}
    	
  	} else if (int(measmnt) >= warnWeight){ //blink characters of "Warning!!"
		lcd.setCursor(0, 2);
	Serial.println(messageCnt, 1);
		if( messageCnt < 1){
	    	lcd.print("Warning!!");
			messageCnt++;
		} else if( messageCnt < 2){
			lcd.print(" ");
			messageCnt++;
		} else {
			messageCnt=0;
		}
  	}

	scale.power_down();
	delay(5);
	scale.power_up();
}

