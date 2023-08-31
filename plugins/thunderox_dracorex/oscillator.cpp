
#include "oscillator.hpp"

//---------------------------------------------------------------------------------------------------

oscillator::oscillator()
{
	wave_mix = 0;
	start_phase = true;
	frequency = 1;
	index = 0;
	increment = 1;
	bandlimit_offset = 0;
}

//---------------------------------------------------------------------------------------------------

oscillator::~oscillator()
{

}


//-----------------------------------------------------------
// Play wavetable 

float oscillator::tick()
{
	sample_rate = 44100;
	tuning = 1;
	float frq = frequency * tuning;
	int length = 4410;
	int index_int = index;
	
	if (frequency < 1) frequency = 1;

	float result1 = 0, result2 = 0;

	if (index == index_int) 
	{
		result1 = wave_a[bandlimit_offset+index_int];
		result2 = wave_b[bandlimit_offset+index_int];		
	}
	else
	{
		float fraction1 = index - index_int;
		float fraction2 = 1 - fraction1;



		if (index < length-2)
		{	

			result1 =  wave_a[index_int+bandlimit_offset] * fraction2;
			result1 += wave_a[index_int+1+bandlimit_offset] * fraction1;
			result2 =  wave_b[index_int+bandlimit_offset] * fraction2;
			result2 += wave_b[index_int+1+bandlimit_offset] * fraction1;	
		}
		else 
		{
			result1 =  wave_a[index_int+bandlimit_offset] * fraction2;
			result1 += wave_a[bandlimit_offset] * fraction1;
			result2 =  wave_b[index_int+bandlimit_offset] * fraction2;
			result2 += wave_b[bandlimit_offset] * fraction1;
		}
	}	

	index += increment;

	if (index > length - 1 )
	{	
		index -= length;	
		start_phase = true;

		int tst = (frq/100)*8;
		if (tst<0) tst = 0;
		if (tst>7) tst = 7;
		bandlimit_offset = (tst*4410);
	}

	increment = (sample_rate * frequency) / sample_rate;
	
	cout << result1 << endl;

	return result1; // (result2 * wave_mix) + (result1 * (1-wave_mix));

}





	
	
	