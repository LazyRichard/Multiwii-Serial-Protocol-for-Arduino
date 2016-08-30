#pragma once

#if __cplusplus <= 199711L
  #error This library needs at least a C++11 compliant compiler
#endif

#include <Arduino.h>
#include <SoftwareSerial.h>

#define MSP_OUT_BUF_SIZE 256
#define MSP_IN_BUF_SIZE 64

enum class MSP_CMD : uint8_t {
	MSP_API_VERSION 	= 1,
	MSP_FC_VARIANT 		= 2,
	MSP_FC_VERSION		= 3,
	MSP_BOARD_INFO		= 4,
	MSP_BUILD_INFO		= 5,
	
	MSP_NAME			= 10,
	MSP_SET_NAME		= 11,
	
	MSP_MODE_RANGES		= 34,
	MSP_SET_MODE_RANGE	= 35,
	
	MSP_FEATURE			= 36,
	MSP_SET_FEATURE		= 37
};

class MSPClass {
public:
	void begin(unsigned long, uint8_t, uint8_t);
	void beginSerial(unsigned long);
#if defined (__AVR_ATmega1280__) || defined (__AVR_ATmega128__) || defined (__AVR_ATmega2560__)
	void beginSerial1(unsigned long);
	void beginSerial2(unsigned long);
	void beginSerial3(unsigned long);
#endif
	struct CommandData {
		uint8_t command;
		uint8_t size;
		uint8_t data[MSP_IN_BUF_SIZE];
	};
	
	void sendCMD(uint8_t);
	void sendCMD(uint8_t, size_t, uint8_t*);
	
	void receiveCMD(char);
	
	uint8_t available();
	
	CommandData retriveCMD();
	
protected:
	enum class RECEIVE_STATUS : uint8_t {
		IDLE, HEADER_START, HEADER_M, HEADER_DIR, HEADER_SIZE, CMD, DATA
	};
	
	CommandData comData[5];
	uint8_t cntComData = 0;
	uint8_t inBufCurser = 0;
	
	RECEIVE_STATUS receiveStatus = RECEIVE_STATUS::IDLE;

	char *PREAMBLE = "$M";
	bool isHwSerial;

	SoftwareSerial *swSerial;
	HardwareSerial *hwSerial;
	
	uint8_t calcChecksum (uint8_t, size_t, uint8_t*);
	bool verifyChecksum (uint8_t, size_t, uint8_t*, uint8_t);
};

extern MSPClass MSP;