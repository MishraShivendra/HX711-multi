#include "HX711-multi.h"
#include <cstdio>

// 
// D0   = 16;
// D1   = 5;
// D2   = 4;
// D3   = 0;
// D4   = 2;
// D5   = 14;
// D6   = 12;
// D7   = 13;
// D8   = 15;
// D9   = 3;
// D10  = 1;
//
// Pins to the load cell amp
#define CLK   12    // clock pin to the load cell amp
#define DOUT1 13    // data pin to the first lca
#define DOUT2 14    // data pin to the second lca



#define BOOT_MESSAGE "Sample Scale"

#define TARE_TIMEOUT_SECONDS 4

byte DOUTS[2] = {DOUT1, DOUT2};

#define CHANNEL_COUNT sizeof(DOUTS)/sizeof(byte)

long int results[CHANNEL_COUNT];

HX711MULTI scales;

void setup() {
	Serial.begin(115200);
	Serial.println(BOOT_MESSAGE);
	Serial.flush();
	scales.setup(CHANNEL_COUNT, DOUTS, CLK);

	tare();
}


void tare() {
	bool tareSuccessful = false;

	unsigned long tareStartTime = millis();
	while (!tareSuccessful && millis()<(tareStartTime+TARE_TIMEOUT_SECONDS*1000)) 
	{
		tareSuccessful = scales.tare(20,10000);  //reject 'tare' if still ringing
	}

}

void sendRawData() {
	scales.read(results);

	for (int i=0; i<scales.get_count(); ++i) {
		float w = (-results[i])/20000.0;
		if( w < 0.0)
			w = 0;

		Serial.print( w,2 );  
		Serial.print( (i!=scales.get_count()-1)?"\t":"\n");
	}  
	delay(10);
}

void loop() 
{

	sendRawData(); //this is for sending raw data, for where everything else is done in processing
	//on serial data (any data) re-tare
	if (Serial.available()>0) {
		while (Serial.available()) {
			Serial.read();
		}
		tare();
	}

}
