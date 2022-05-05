/*
 * lfo.h
 *
 *  Created on: Jun 7, 2020
 *      Author: Martin
 */

#ifndef LFO_H_
#define LFO_H_
//global defines
#define WAVETABLE_SIZE_LFO 128

u8 LFO_phase;

//phase of lfo

u32 wave_acc_LFO;
u32 wave_it_LFO;
u8 LFO_mode;

typedef enum {
  WAVEFORM_LFO_SINE,
  WAVEFORM_LFO_PULSE,
  WAVEFORM_LFO_SAW
} lfo_waveform_t;


extern s32 LFO_FreqSet(u8 speed);
extern u32 LFO_Out(u8 mode,u8 tick);

extern u32 LFO_OutWavetable(u8 mode);

#endif /* LFO_H_ */
