#include "msp.h"

MSPClass MSP;	

void MSPClass::begin(unsigned long baudrate, uint8_t rx, uint8_t tx) {
	isHwSerial = false;
	
	swSerial = new SoftwareSerial(baudrate, rx, tx);
}

void MSPClass::beginSerial(unsigned long baudrate) {
	isHwSerial = true;
	
	hwSerial = &Serial;
	hwSerial->begin(baudrate);
}

#if defined (__AVR_ATmega1280__) || defined (__AVR_ATmega128__) || defined (__AVR_ATmega2560__)
void MSPClass::beginSerial1(unsigned long baudrate) {
	isHwSerial = true;
	
	hwSerial = &Serial1;
	hwSerial->begin(baudrate);
}

void MSPClass::beginSerial2(unsigned long baudrate) {
	isHwSerial = true;
	
	hwSerial = &Serial2;
	hwSerial->begin(baudrate);
}

void MSPClass::beginSerial3(unsigned long baudrate) {
	isHwSerial = true;
	
	hwSerial = &Serial3;
	hwSerial->begin(baudrate);
}
#endif

void MSPClass::sendCMD(uint8_t cmd) {
	sendCMD(cmd, 0, 0);
}

void MSPClass::sendCMD(uint8_t cmd, size_t size, uint8_t *data) {
	uint8_t writeBuff[MSP_OUT_BUF_SIZE];
	uint8_t curser = 0;
	
	// write PREAMBLE to buff
	for (int i = 0; i < strnlen(PREAMBLE, MSP_OUT_BUF_SIZE); i++) {
		writeBuff[curser++] = *(PREAMBLE + i);
	}
	
	// write Direction to buff
	writeBuff[curser++] = '<';
	// write Size to buff
	writeBuff[curser++] = (uint8_t)size;
	// write Command to buff
	writeBuff[curser++] = cmd;
	
	// write Data to buff
	for (int i = 0; i < strnlen(data, MSP_OUT_BUF_SIZE); i++) {
		writeBuff[curser++] = *(data + i);
	}
	
	// write checksum to buff
	writeBuff[curser++] = calcChecksum(size, cmd, data);
	
	// write command to serial
	if (isHwSerial) {
		for (int i = 0; i < curser; i++)
			hwSerial->write(writeBuff[i]);
	} else {
		for (int i = 0; i < curser; i++)
			swSerial->write(writeBuff[i]);
	}
}

void MSPClass::receiveCMD(char ch) {
	switch (receiveStatus) {
	case (RECEIVE_STATUS::IDLE):
		receiveStatus = (ch == '$') ? (RECEIVE_STATUS::HEADER_START) : (RECEIVE_STATUS::IDLE);
		break;
	case (RECEIVE_STATUS::HEADER_START):
		receiveStatus = (ch == 'M') ? (RECEIVE_STATUS::HEADER_M) : (RECEIVE_STATUS::IDLE);
		break;
	case (RECEIVE_STATUS::HEADER_M):
		receiveStatus = (ch == '>') ? (RECEIVE_STATUS::HEADER_DIR) : (RECEIVE_STATUS::IDLE);
		break;
	case (RECEIVE_STATUS::HEADER_DIR):
		comData[cntComData].size = ch;
		receiveStatus = RECEIVE_STATUS::HEADER_SIZE;
		break;
	case (RECEIVE_STATUS::HEADER_SIZE):
		comData[cntComData].command = ch;
		inBufCurser = 0;
		receiveStatus = RECEIVE_STATUS::CMD;
		break;
	case (RECEIVE_STATUS::CMD):
		comData[cntComData].data[inBufCurser++] = ch;
		
		if (inBufCurser > MSP_IN_BUF_SIZE) {
			receiveStatus = RECEIVE_STATUS::IDLE;
			inBufCurser = 0;
		} else if (inBufCurser > (comData[cntComData].size - 1)) {
			receiveStatus = RECEIVE_STATUS::DATA;
		}
		
		break;
	case (RECEIVE_STATUS::DATA):
		if (!verifyChecksum(comData[cntComData].command, comData[cntComData].size, comData[cntComData].data, ch)) {
			receiveStatus = RECEIVE_STATUS::IDLE;
		} else {
			cntComData = (cntComData + 1) % (sizeof(comData) / sizeof(*comData));
		}
		
		break;
	}
}

uint8_t MSPClass::available() {
	return cntComData;
}

MSPClass::CommandData MSPClass::retriveCMD() {
	if (cntComData) {
		return comData[--cntComData];
	} else {
		return comData[0];
	}
}

/*
 * Protected methods
 */

uint8_t MSPClass::calcChecksum (uint8_t cmd, size_t size, uint8_t *data) {
	uint8_t checksum = 0;
	
	checksum ^= size;
	checksum ^= cmd;
	
	for (size_t i = 0; i < size; i++) {
		checksum ^= *(data + i);
	}
	
	return checksum;
}

bool MSPClass::verifyChecksum (uint8_t cmd, size_t size, uint8_t *data, uint8_t checksum) {
	return (calcChecksum(cmd, size, data) == checksum) ? true : false;
}