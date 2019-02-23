#pragma once

#if defined(PARTICLE)
#  include <Particle.h>
#else
#  if ARDUINO >= 100
#    include "Arduino.h"
#  else
#    include "WProgram.h"
#  endif
#endif
#include <math.h>

class HX711ADC
{
	private:
		byte PD_SCK;	// Power Down and Serial Clock Input Pin
		byte DOUT;		// Serial Data Output Pin
		byte GAIN;		// amplification factor
		long OFFSET = 0;	// used for tare weight
		float SCALE = 1;	// used to return weight in grams, kg, ounces, whatever

	public:
		// define clock and data pin, channel, and gain factor
		// channel selection is made by passing the appropriate gain: 128 or 64 for channel A, 32 for channel B
		// gain: 128 or 64 for channel A; channel B works with 32 gain factor only
		HX711ADC(byte dout, byte pd_sck, byte gain = 128);

		HX711ADC();

		virtual ~HX711ADC();

		// Allows to set the pins and gain later than in the constructor
  
    void begin();
		void begin(byte dout, byte pd_sck, byte gain = 128);

		// check if HX711 is ready
		// from the datasheet: When output data is not ready for retrieval, digital output pin DOUT is high. Serial clock
		// input PD_SCK should be low. When DOUT goes to low, it indicates data is ready for retrieval.
		inline bool is_ready() { return !digitalRead(DOUT); };

		// set the gain factor; takes effect only after a call to read()
		// channel A can be set for a 128 or 64 gain; channel B has a fixed 32 gain
		// depending on the parameter, the channel is also set to either A or B
		void set_gain(byte gain = 128);

		// waits for the chip to be ready and returns a reading (1sec default timeout)
		long read(time_t timeout = 1000);

		// returns an average reading; times = how many times to read
		long read_average(byte times = 10);

		// returns (read_average() - OFFSET), that is the current value without the tare weight; times = how many readings to do
		double get_value(byte times = 1);

		// returns get_value() divided by SCALE, that is the raw value divided by a value obtained via calibration
		// times = how many readings to do
		float get_units(byte times = 1);

		// set the OFFSET value for tare weight; times = how many times to read the tare value
		void tare(byte times = 10);

		// set the SCALE value; this value is used to convert the raw data to "human readable" data (measure units)
		void set_scale(float scale = 1.f);

		// get the current SCALE
		float get_scale();

		// set OFFSET, the value that's subtracted from the actual reading (tare weight)
		void set_offset(long offset = 0);

		// get the current OFFSET
		long get_offset();

		// puts the chip into power down mode
		void power_down();

		// wakes up the chip after power down mode
		void power_up();

#if defined(PARTICLE) 
    // to keep the Particle cloud happy when the library blocks
    inline void yield() { Particle.process(); }; 
#endif
};
