
#include "voice.hpp"

//----------------------------------------------------------------------------------------------------------------

voice::voice()
{


			

};


//----------------------------------------------------------------------------------------------------------------

voice::~voice()
{

};



//----------------------------------------------------------------------------------------------------------------
// PLAY VOICE

float voice::play(float* left_buffer, float* right_buffer,  uint32_t frames)
{

	for (uint32_t x=0; x<frames; x++)
	{
		left_buffer[x] = osc1.tick();
		right_buffer[x] = left_buffer[x];
	}

	return 0;	

};