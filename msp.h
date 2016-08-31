#pragma once

#if __cplusplus <= 199711L
  #error This library needs at least a C++11 compliant compiler
#endif

#include <Arduino.h>
#include <SoftwareSerial.h>

#define MSP_OUT_BUF_SIZE 256
#define MSP_IN_BUF_SIZE 64

enum class MSP_CMD : uint8_t {
	MSP_API_VERSION 			 = 1,	// out message
	MSP_FC_VARIANT 				 = 2,	// out message
	MSP_FC_VERSION				 = 3,	// out message
	MSP_BOARD_INFO				 = 4,	// out message
	MSP_BUILD_INFO				 = 5,	// out message
	
	MSP_NAME					 = 10,	// out message
	MSP_SET_NAME				 = 11,	// in message
	
	MSP_MODE_RANGES				 = 34,	// out message
	MSP_SET_MODE_RANGE			 = 35,	// in message
	
	MSP_FEATURE					 = 36,	// out message
	MSP_SET_FEATURE				 = 37,	// in message
	
	MSP_BOARD_ALIGNMENT 		 = 38,	// out message
	MSP_SET_BOARD_ALIGNMENT		 = 39,	// in message
	
	MSP_CURRENT_METER_CONFIG	 = 40,	// out message
	MSP_SET_CURRENT_MEGER_CONFIG = 41,	// in message
	
	MSP_MIXER					 = 42,	// out message
	MSP_SET_MIXER				 = 43,	// in message
	
	MSP_RX_CONFIG				 = 44,	// out message
	MSP_SET_RX_CONFIG			 = 45,	// in message
	
	MSP_LED_COLORS				 = 46,	// out message
	MSP_SET_LED_COLORS			 = 47,	// in message
	
	MSP_LED_STRIP_CONFIG		 = 48,	// out message
	MSP_SET_LED_STRIP_CONFIG	 = 49,	// in message
	
	MSP_RSSI_CONFIG				 = 50,	// out message
	MSP_SET_RSSI_CONFIG			 = 51,	// in message
	
	MSP_ADJUSTMENT_RANGES		 = 52,	// out message
	MSP_SET_ADJUSTMENT_RANGE	 = 53,	// in message
	
	MSP_REBOOT					 = 68,	// in message
	
	MSP_DATAFLASH_SUMMARY		 = 70,	// out message
	MSP_DATAFLASH_READ			 = 71,	// out message
	MSP_DATAFLASH_ERASE			 = 72,	// in message
	
	MSP_LOOP_TIME				 = 73,	// out message
	MSP_SET_LOOP_TIME			 = 74,	// in message
	
	MSP_FAILSAFE_CONFIG			 = 75,	// out message
	MSP_SET_FAILSAFE_CONFIG		 = 76,	// in message
	
	MSP_RXFAIL_CONFIG			 = 77,	// out message
	MSP_SET_RXFAIL_CONFIG		 = 78,	// in message
	
	MSP_SDCARD_SUMMARY			 = 79,	// out message
	
	MSP_BLACKBOX_CONFIG			 = 80,	// out message
	MSP_SET_BLACKBOX_CONFIG		 = 81,	// in message
	
	MSP_TRANSPONDER_CONFIG		 = 82,	// out message
	MSP_SET_TRANSPONDER_CONFIG	 = 83,	// in message
	
	MSP_OSD_CHAR_READ			 = 86,	// out message
	MSP_OSD_CHAR_WRITE			 = 87,	// in message
	
	MSP_VTX_CONFIG				 = 88,	// out message
	MSP_SET_VTX_CONFIG			 = 89,	// in message
	
	//
	// Multiwii original MSP commands
	//
	MSP_STATUS					 = 101,	// out message
	MSP_RAW_IMU					 = 102,	// out message
	MSP_SERVO					 = 103,	// out message
	MSP_MOTOR					 = 104,	// out message
	MSP_RC						 = 105,	// out message
	MSP_RAW_GPS					 = 106,	// out message
	MSP_COMP_GPS				 = 107,	// out message
	MSP_ATTITUDE				 = 108,	// out message
	MSP_ALTITUDE				 = 109,	// out message
	MSP_ANALOG					 = 110,	// out message
	MSP_RC_TUNING				 = 111,	// out message
	MSP_PID						 = 112,	// out message
	MSP_BOX						 = 113, // out message
	MSP_MISC					 = 114,	// out message
	MSP_MOTOR_PINS				 = 115,	// out message
	MSP_BOXNAMES				 = 116,	// out message
	MSP_PIDNAMES				 = 117,	// out message
	MSP_WP						 = 118,	// out message
	MSP_BOXIDS					 = 119,	// out message
	MSP_SERVO_CONFIGURATIONS	 = 120,	// out message
	MSP_NAV_STATUS				 = 121,	// out message
	MSP_NAV_CONFIG				 = 122,	// out message
	MSP_3D						 = 124,	// out message
	MSP_RC_DEADBAND				 = 125,	// out message
	MSP_SENSOR_ALIGNMENT		 = 126,	// out message
	MSP_LED_STRIP_MODECOLOR		 = 127,	// out message
	MSP_VOLTAGE_METERS			 = 128,	// out message
	MSP_BATTERY_STATES			 = 130,	// out message
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
	
	void receiveCMD();
	
	int available();
	uint8_t availableData();
	
	CommandData retrieveCMD();
	
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