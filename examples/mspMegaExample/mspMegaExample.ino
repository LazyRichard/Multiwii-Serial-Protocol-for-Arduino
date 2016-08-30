#include "msp.h"

MSPClass::CommandData cData;

// 사용자 지정 명령어
enum class CustomCommand : uint8_t {
	CUSTOM_COMMAND1 = 1;
	CUSTOM_COMMAND2 = 2;
}

void setup() {
	Serial.begin(9600);
	MSP.beginSerial1(115200);

	Serial.println("Start");
}

void loop() {
	currTime = millis();
	
	if (MSP.available()) {
		cData = MSP.retriveCMD();
		
		Serial.print(cData.command); Serial.print(" "); Serial.print(cData.size); Serial.print(" ");
		
		for (int i = 0; i < cData.size; i++) {
			Serial.print(cData.data[i]); Serial.print(" ");
		}
		
		Serial.print("\n");
	}
}

void serialEvent() {
	Serial.println("Serial Event!");
	
	while (Serial.available()) {
		char ch = Serial.read();
		
		switch (ch) {
		case 'q':
			MSP.sendCMD(static_cast<uint8_t>(MSP_CMD::MSP_FC_VARIANT));
			break;
		case 'w':
			// 사용자 지정 명령어
			MSP.sendCMD(static_cast<uint8_t>(CuscomCommand::CUSTOM_COMMAND1));
		}
	}
}

void serialEvent1() {
	char ch;
	Serial.println("Serial1 Event!");
	
	while (Serial1.available()) {
		ch = Serial1.read();
		//Serial.print(ch, DEC); Serial.print(".");
		MSP.receiveCMD(ch);
	}
}