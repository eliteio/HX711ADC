#include "HX711ADC.h"

HX711ADC::HX711ADC(byte dout, byte pd_sck, byte gain) :
  PD_SCK(pd_sck), DOUT(dout) {
	switch (gain) {
		case 128:		// channel A, gain factor 128
			GAIN = 1;
			break;
		case 64:		// channel A, gain factor 64
			GAIN = 3;
			break;
		case 32:		// channel B, gain factor 32
			GAIN = 2;
			break;
    default:
      GAIN = 1;
      break;
	}
}

HX711ADC::HX711ADC() {
}

HX711ADC::~HX711ADC() {
}

void HX711ADC::begin() {
	pinMode(PD_SCK, OUTPUT);
	pinMode(DOUT, INPUT);
	digitalWrite(PD_SCK, LOW);
}

void HX711ADC::begin(byte dout, byte pd_sck, byte gain) {
 	PD_SCK = pd_sck;
	DOUT = dout;
	pinMode(PD_SCK, OUTPUT);
	pinMode(DOUT, INPUT);
  set_gain(gain);
}

void HX711ADC::set_gain(byte gain) {
	switch (gain) {
		case 128:		// channel A, gain factor 128
			GAIN = 1;
			break;
		case 64:		// channel A, gain factor 64
			GAIN = 3;
			break;
		case 32:		// channel B, gain factor 32
			GAIN = 2;
			break;
    default:
      GAIN = 1;
      break;
	}

	digitalWrite(PD_SCK, LOW);
	//read();
}

long HX711ADC::read(time_t timeout) {
	// wait for the chip to become ready
	for (time_t ms=millis(); !is_ready() && (millis() - ms < timeout);) {
		// Will do nothing on Arduino but 
    // prevent resets of ESP8266 (Watchdog Issue)
    // or keeps cloud housekeeping running on Particle devices
		yield();
	}
  // still not ready after timeout periode, report error Not-A-Number
  if (!is_ready()) return NAN;

	unsigned long value = 0;
	uint8_t data[3] = { 0 };
	uint8_t filler = 0x00;

	// pulse the clock pin 24 times to read the data
	data[2] = shiftIn(DOUT, PD_SCK, MSBFIRST);
	data[1] = shiftIn(DOUT, PD_SCK, MSBFIRST);
	data[0] = shiftIn(DOUT, PD_SCK, MSBFIRST);

	// set the channel and the gain factor for the next reading using the clock pin
	for (unsigned int i = 0; i < GAIN; i++) {
		digitalWrite(PD_SCK, HIGH);
		digitalWrite(PD_SCK, LOW);
	}

	// Replicate the most significant bit to pad out a 32-bit signed integer
	if (data[2] & 0x80) {
		filler = 0xFF;
	} else {
		filler = 0x00;
	}

	// Construct a 32-bit signed integer
	value = static_cast<unsigned long>(filler)  << 24
		  | static_cast<unsigned long>(data[2]) << 16
		  | static_cast<unsigned long>(data[1]) << 8
		  | static_cast<unsigned long>(data[0]) ;

	return static_cast<long>(value);
}

long HX711ADC::read_average(byte times) {
  if (times <= 0) return NAN;
	long sum = 0;
	for (byte i = 0; i < times; i++) {
		sum += read();
		yield();
	}
	return sum / times;
}

double HX711ADC::get_value(byte times) {
	return read_average(times) - OFFSET;
}

float HX711ADC::get_units(byte times) {
	return get_value(times) / SCALE;
}

void HX711ADC::tare(byte times) {
	double sum = read_average(times);
	set_offset(sum);
}

void HX711ADC::set_scale(float scale) {
  if (scale) {
	  SCALE = scale;
  }
  else {
    SCALE = 1;
  }
}

float HX711ADC::get_scale() {
	return SCALE;
}

void HX711ADC::set_offset(long offset) {
	OFFSET = offset;
}

long HX711ADC::get_offset() {
	return OFFSET;
}

void HX711ADC::power_down() {
	digitalWrite(PD_SCK, LOW);
	digitalWrite(PD_SCK, HIGH);
}

void HX711ADC::power_up() {
	digitalWrite(PD_SCK, LOW);
}
