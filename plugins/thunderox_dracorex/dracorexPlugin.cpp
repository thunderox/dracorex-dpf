
// ThunderOx dracorex synth - By Nick S. Bailey <tb303@gmx.com>
// DISTRHO Plugin Framework (DPF) Copyright (C) 2012-2019 Filipe Coelho <falktx@falktx.com>

#include "voice.hpp"

#include "DistrhoPlugin.hpp"
#include <string.h>
#include <string>
#include <iostream>
#include <math.h>
#include <sstream> 
#include <fstream>
#include <dirent.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <vector>

float buf0,buf1;
float f, pc, q;             //filter coefficients
float bf0, bf1, bf2, bf3, bf4;  //filter buffers (beware denormals!)
float t1, t2;              //temporary buffers
float selectivity, gain1, gain2, ratio, cap; 

using namespace std;

const int max_notes = 12;

START_NAMESPACE_DISTRHO

class dracorexPlugin : public Plugin
{
	public:
	
	
		// set up voices
		voice voices[max_notes];	
		vector <wavetable> wavetables;
		

			
		// Parameters
		float fParameters[kParameterCount+3];
		
		// Audio Struct
		struct audio_stereo
		{ 
			float left,right; 
		};
		float srate = getSampleRate();
		
		int midi_keys[128];
			 
		int current_voice = 0;
		int old_voice = 0;
		
		float* pitch_bend = new float();
		float* channel_after_touch = new float();

			
		// dracorex Audio Buffer
		vector <audio_stereo> audio_buffer; 
					
		dracorexPlugin() : Plugin(kParameterCount, 0, 0)
		{

			srate = getSampleRate();

			audio_buffer.resize(srate*5);
	
			for (int x=0; x<128; x++) { midi_keys[x] = -1; }
			
			// clear all parameters
			std::memset(fParameters, 0, sizeof(float)*kParameterCount);

			// we can know buffer-size right at the start
			fParameters[kParameterBufferSize] = getBufferSize();

		}

		const char* getDescription() const override
		{
			return "dracorex is a polyphonic wavetable synthesizer plugin.";
		}
		
		const char* getHomePage() const override
		{
			return "https://thunderox.com/synths/dracorex";
		}

		const char* getLabel() const
		{
			return "Thunderox dracorex";
		}

		const char* getMaker() const
		{
			return "Nick S. Bailey";
		}

		const char* getLicense() const
		{
			return "ISC";
		}

		//Get the plugin version, in hexadecimal.
    
		uint32_t getVersion() const override
		{
			return d_version(1, 0, 0);
		}

		int64_t getUniqueId() const
		{
			return d_cconst('T', 'O', 'd', 'r');
		}



		void initParameter(uint32_t index, Parameter& parameter) override
		{
			switch (index)
			{

				case dracorex_VOLUME:
					parameter.name   = "volume";
					parameter.symbol = "volume";
					parameter.hints = kParameterIsAutomatable;
					parameter.ranges.min = 0.0f;
					parameter.ranges.max = 1.0f;
					parameter.ranges.def = 0.8f;
					fParameters[dracorex_VOLUME] = parameter.ranges.def;
					break;
					
				case dracorex_OSC1_WAVE_A:
					parameter.name   = "osc1_wave_a";
					parameter.symbol = "osc1_wave_a";
					parameter.hints = kParameterIsAutomatable;
					parameter.ranges.min = 0.0f;
					parameter.ranges.max = 12.0f;
					parameter.ranges.def = 9.0f;
					fParameters[dracorex_OSC1_WAVE_A] = parameter.ranges.def;
					break;
					
				case dracorex_AMP_ATTACK:
					parameter.name   = "amp_attack";
					parameter.symbol = "amp_attack";
					parameter.hints = kParameterIsAutomatable;
					parameter.ranges.min = 0.0f;
					parameter.ranges.max = 1.0f;
					parameter.ranges.def = 1.0f;
					fParameters[dracorex_AMP_ATTACK] = parameter.ranges.def;
					break;
					
				case dracorex_AMP_DECAY:
					parameter.name   = "amp_decay";
					parameter.symbol = "amp_decay";
					parameter.hints = kParameterIsAutomatable;
					parameter.ranges.min = 0.0f;
					parameter.ranges.max = 1.0f;
					parameter.ranges.def = 0.0f;
					fParameters[dracorex_AMP_DECAY] = parameter.ranges.def;
					break;
					
				case dracorex_AMP_SUSTAIN:
					parameter.name   = "amp_sustain";
					parameter.symbol = "amp_sustain";
					parameter.hints = kParameterIsAutomatable;
					parameter.ranges.min = 0.0f;
					parameter.ranges.max = 1.0f;
					parameter.ranges.def = 0.0f;
					fParameters[dracorex_AMP_SUSTAIN] = parameter.ranges.def;
					break;
					
				case dracorex_AMP_RELEASE:
					parameter.name   = "amp_release";
					parameter.symbol = "amp_release";
					parameter.hints = kParameterIsAutomatable;
					parameter.ranges.min = 0.0f;
					parameter.ranges.max = 0.0f;
					parameter.ranges.def = 1.0f;
					fParameters[dracorex_AMP_RELEASE] = parameter.ranges.def;
					break;
					
					
				case dracorex_WAVE_ATTACK:
					parameter.name   = "wave_attack";
					parameter.symbol = "wave_attack";
					parameter.hints = kParameterIsAutomatable;
					parameter.ranges.min = 0.0f;
					parameter.ranges.max = 1.0f;
					parameter.ranges.def = 1.0f;
					fParameters[dracorex_WAVE_ATTACK] = parameter.ranges.def;
					break;
					
				case dracorex_WAVE_DECAY:
					parameter.name   = "wave_attack";
					parameter.symbol = "wave_attack";
					parameter.hints = kParameterIsAutomatable;
					parameter.ranges.min = 0.0f;
					parameter.ranges.max = 1.0f;
					parameter.ranges.def = 0.0f;
					fParameters[dracorex_AMP_DECAY] = parameter.ranges.def;
					break;
					
				case dracorex_WAVE_SUSTAIN:
					parameter.name   = "wave_attack";
					parameter.symbol = "wave_attack";
					parameter.hints = kParameterIsAutomatable;
					parameter.ranges.min = 0.0f;
					parameter.ranges.max = 1.0f;
					parameter.ranges.def = 0.0f;
					fParameters[dracorex_WAVE_SUSTAIN] = parameter.ranges.def;
					break;
					
				case dracorex_WAVE_RELEASE:
					parameter.name   = "wave_attack";
					parameter.symbol = "wave_attack";
					parameter.hints = kParameterIsAutomatable;
					parameter.ranges.min = 0.0f;
					parameter.ranges.max = 0.0f;
					parameter.ranges.def = 1.0f;
					fParameters[dracorex_WAVE_RELEASE] = parameter.ranges.def;
					break;
					
			}

		}

		/* --------------------------------------------------------------------------------------------------------
		* Internal data 
		Get the current value of a parameter.
 		The host may call this function from any context, including realtime processing.
		*/

		float getParameterValue(uint32_t index) const override
		{
			return fParameters[index];

		}

		/**
  		Change a parameter value.
 		The host may call this function from any context, including realtime processing.
		When a parameter is marked as automable, you must ensure no non-realtime operations are performed.
		@note This function will only be called for parameter inputs.
		*/

		void setParameterValue(uint32_t index, float value) override
		{
			fParameters[index] = value;

		}
		
		//===============================================================

		float fastishP2F (float pitch)
		{
		long convert;
		float *p=(float *)&convert;
		float fl,fr,warp,out;

		pitch *=0.0833333; //pitch scaling. remove this line for pow(2,a)
		fl = floor(pitch);
		fr = pitch - fl;
		float fr2 = fr*fr;
		warp = fr*0.696 + fr2*0.225 + fr*fr2*0.079;  // chebychev approx
		//warp = fr*0.65 + fr*fr*0.35; // chebychev approx

		out = fl+warp;
		out *= 8388608.0; //2^23;
		out += 127.0 * 8388608.0; //2^23;

		convert = out; //magic

		return *p;
		}




		void run(const float** inputs, float** outputs, uint32_t frames,
             const MidiEvent* midiEvents, uint32_t midiEventCount) override
		{

			// if (!fParameters[dracorex_DSP_RUN]) return; // Playing sound during preset load is a bad idea.	
			
			// DO MIDI STUFF -------------------------------------------------------------------------
			
			int midi_channel = 0;
			
			for (uint32_t i=0; i<midiEventCount; ++i)
			{
				const uint8_t* ev = midiEvents[i].data;
			      	if ((int)ev[0]  == 0x90 + midi_channel && (int)ev[2] > 0)
				{
					int note = (int)ev[1];
					voices[0].active = true;
					voices[0].amp_env.state = ENV_STATE_ATTACK;
					voices[0].wave_env.state = ENV_STATE_ATTACK;
					voices[0].amp_env.level = 0;
					voices[0].wave_env.level = 0;
					voices[0].osc1.frequency = fastishP2F(note + fParameters[dracorex_OSC1_TUNING]);
					voices[0].osc2.frequency = fastishP2F(note + fParameters[dracorex_OSC2_TUNING]);
					
				}
				
				if ((int)ev[0] == 0x80 || (int)ev[0] == 0x90 && (int)ev[2] == 0)
				{
					int note = (int)ev[1];
					voices[0].amp_env.state = ENV_STATE_RELEASE;
					voices[0].wave_env.state = ENV_STATE_RELEASE;
				}
			
			}	        
			
			// DO AUDIO STUFF -------------------------------------------------------------------------
			
			float* out_left = outputs[0];
			float* out_right = outputs[1];
			
			memset( out_left, 0, sizeof(double)*(frames*0.5) );
			memset( out_right, 0, sizeof(double)*(frames*0.5) );
			
			int wn_a = fParameters[dracorex_OSC1_WAVE_A];
			int wn_b = fParameters[dracorex_OSC1_WAVE_B];
		
			voices[0].osc1.wave_a = wavetables[wn_a].buffer;
			voices[0].osc1.wave_b = wavetables[wn_b].buffer;
			// voices[0].osc1.frequency = 10 + ( fParameters[dracorex_OSC1_TUNING] * 50);
			voices[0].osc1.wave_mix = fParameters[dracorex_OSC1_PITCH_ADSR2];
			
			voices[0].osc2.wave_a = wavetables[wn_a].buffer;
			voices[0].osc2.wave_b = wavetables[wn_b].buffer;
			// voices[0].osc2.frequency = 10 + ( fParameters[dracorex_OSC1_TUNING] * 6);
			
			voices[0].play(out_left, out_right, frames);
		}


		//----------------------------------------------
		 				
		void initProgramName(uint32_t index, String& program_name) override
		{
		}
	
		
		//----------------------------------------------

	private:

		bool preset_loading = false;
		int bpm;
		uint32_t buffer_length;

};

Plugin* createPlugin()
{

	dracorexPlugin* dracorex = new dracorexPlugin();
	
	// LOAD WAVETABLES;

	string user_path = getenv("USER");
	stringstream wave_path;
	stringstream wave_file;
	struct dirent *d;
	struct stat st;

	DIR *dr;
	wave_path.str("");
	wave_path << "/home/" << user_path << "/.lv2/thunderox_dracorex.lv2/waves";
			
	dr = opendir(wave_path.str().c_str());
		 
	if (dr != NULL)
	{
		for( d=readdir(dr); d!=NULL; d=readdir(dr)) // List all files here
		{
			wave_file.str("");
			wave_file << wave_path.str() << "/" << d->d_name;

			FILE* fp = fopen (wave_file.str().c_str(),"r");
			fseek(fp, 0, SEEK_END); // We can use rewind(fp); also
				
			if (ftell(fp) == 17720)
			{					
				long length = (ftell(fp) - 80 )/ 4;
				
				wavetable new_waveform;
					
				float* source_waveform_buffer = (float *)malloc(length*sizeof(float)) ;
				new_waveform.buffer = (float *)malloc((12*length)*sizeof(float)) ;
							
				// LOAD RAW WAVEFORM
							
				fseek(fp, 80, SEEK_SET);
				fread(source_waveform_buffer ,1, length*sizeof(float), fp);
				fclose(fp);	
			
								
				//------ FILTER 8 DIFFERENT VERSINS OF WAVEFORM TO REDUCE ALIASING

				// Moog 24 dB/oct resonant lowpass VCF
				// References: CSound source code, Stilson/Smith CCRMA paper.
				// Modified by paul.kellett@maxim.abel.co.uk July 2000

				// Set coefficients given frequency & resonance [0.0...1.0]
				
				float frequency = 0.8;
				float resonance = 0;	
				float in;


				for (int wave=0; wave<12; wave++)
				{
				
					float minpeak = 0;
					float maxpeak = 0;
					float maxvol = 0;
				
					for (int x=0; x<length; x++)
					{
						in = source_waveform_buffer[x];

						q = 1.0f - frequency;
						pc = frequency + 0.8f * frequency * q;
						f = pc + pc - 1.0f;
						q = resonance * (1.0f + 0.5f * q * (1.0f - q + 5.6f * q * q));
				
						// Filter (in [-1.0...+1.0])

						in -= q * bf4;                          //feedback
						t1 = bf1;  bf1 = (in + bf0) * pc - bf1 * f;
						t2 = bf2;  bf2 = (bf1 + t1) * pc - bf2 * f;
						t1 = bf3;  bf3 = (bf2 + t2) * pc - bf3 * f;
						bf4 = (bf3 + t1) * pc - bf4 * f;
						bf4 = bf4 - bf4 * bf4 * bf4 * 0.166667f;    //clipping
						bf0 = in;

						new_waveform.buffer[x + (wave*length)] = bf4;
							
						if (bf4 < 0 && bf4 < minpeak) minpeak = bf4;
						if (bf4 > 0 && bf4 > maxpeak) maxpeak = bf4;
					
					}

					if (-minpeak > maxpeak) maxvol = -minpeak;
						else maxvol = maxpeak;

					if (-minpeak == maxpeak) maxvol = maxpeak;

					float amp = 0.8 / maxvol;

					for (int y=0; y<length; y++)	// Normalise
					{
						new_waveform.buffer[y+(wave*length)] = new_waveform.buffer[y+(wave*length)]*amp;
					}

					frequency /= 1.8;
					
				}
			new_waveform.length = length;
			new_waveform.name = d->d_name;
			
			for (int x=0; x<length; x++)
			{
				new_waveform.buffer[x] = source_waveform_buffer[x] * 0.8;
				//memcpy(new_waveform.buffer,source_waveform_buffer,length*sizeof(float));
			}		
			dracorex->wavetables.push_back(new_waveform);
			free(source_waveform_buffer);
			}
		}	
	}	
	
	
	for (int v=0; v<max_notes; v++)
	{
		dracorex->voices[v].osc1.sample_rate = dracorex->getSampleRate();
		dracorex->voices[v].osc2.sample_rate = dracorex->getSampleRate();
		dracorex->voices[v].fParameters = dracorex->fParameters;
	
	}
	
	struct alphasort_wavetables
	{
		inline bool operator() (const wavetable& struct1, const wavetable& struct2)
		{
			return (struct1.name < struct2.name);
		}
	};
	
	sort(dracorex->wavetables.begin(),dracorex->wavetables.end(),alphasort_wavetables() );	
	
	return dracorex;
}      

END_NAMESPACE_DISTRHO





