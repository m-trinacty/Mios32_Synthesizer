/*
 * lfo.c
 *
 *  Created on: Jun 7, 2020
 *      Author: Martin
 */
#include <mios32.h>
#include "lfo.h"
#include "synth.h"

#include "lfoTab.h"

#define LFO_FRQ 48000/128

s32 LFO_FreqSet(u8 speed){
	// calculate new accumulator
	wave_acc_LFO = (u32) (((LFO_val[speed] / (LFO_FRQ))
				* (float) (1 << FP_PRECISSION)));

	return 0; // no error
}
u32 LFO_OutWavetable(u8 mode){
	u32 out=0;
	switch (mode) {
		case WAVEFORM_LFO_SINE:
			out=LFO_sinTab[wave_it_LFO>>FP_PRECISSION]<<7;
			break;
		case WAVEFORM_LFO_PULSE:
			out=LFO_squareTab[wave_it_LFO>>FP_PRECISSION]<<7;
			break;
		case WAVEFORM_LFO_SAW:
			out=LFO_sawTab[wave_it_LFO>>FP_PRECISSION]<<7;
			break;
	}

	return out;
}

u32 LFO_Out(u8 mode,u8 tick){
	u32 out=0;
	switch (mode) {
		case WAVEFORM_LFO_SINE:
			out=LFO_sinTab[tick];
			break;
		case WAVEFORM_LFO_PULSE:
			out=LFO_squareTab[tick];
			break;
		case WAVEFORM_LFO_SAW:
			out=LFO_sawTab[tick];
			break;
	}

	return out;
}


