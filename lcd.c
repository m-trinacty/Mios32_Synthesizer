/*
 * lcd.c
 *
 *  Created on: Jul 10, 2020
 *      Author: Martin
 */
#include <mios32.h>
#include "synth.h"
#include "lcd.h"
#include "lfo.h"

char dbgBuf[20];
void LCD_FilterCutoff(u16 cutoff){
	MIOS32_LCD_Clear();
	MIOS32_LCD_CursorSet(0,0);
	MIOS32_LCD_PrintFormattedString("Filter");

	MIOS32_LCD_CursorSet(0,1);
	MIOS32_LCD_PrintFormattedString("Cutoff: %d",cutoff);
}
void LCD_FilterResonance(u16 q){
	MIOS32_LCD_Clear();
	MIOS32_LCD_CursorSet(0,0);
	MIOS32_LCD_PrintFormattedString("Filter");

	MIOS32_LCD_CursorSet(0,1);
	MIOS32_LCD_PrintFormattedString("Resonance: %d",q);
}
void LCD_FilterMode(u8 mode){
	switch ((mode>>5)) {
			case 0:
				sprintf(dbgBuf, "Low Pass");
				break;
			case 1:
				sprintf(dbgBuf, "Band Pass");
				break;
			case 2:
				sprintf(dbgBuf, "High Pass");
				break;
			default:
				sprintf(dbgBuf, "High Pass");
				break;
	}
	MIOS32_LCD_Clear();
	MIOS32_LCD_CursorSet(0,0);
	MIOS32_LCD_PrintFormattedString("Filter");

	MIOS32_LCD_CursorSet(0,1);
	MIOS32_LCD_PrintFormattedString("Mode: %s",dbgBuf);
}
void LCD_FilterEnvelope(u8 eg){
	MIOS32_LCD_Clear();
	MIOS32_LCD_CursorSet(0,0);
	MIOS32_LCD_PrintFormattedString("Filter");

	MIOS32_LCD_CursorSet(0,1);

	MIOS32_LCD_PrintFormattedString("Envelope : %d",eg);
}
void LCD_OSCWaveform(int wave,int osc){
	switch (wave) {
			case 0:
				sprintf(dbgBuf, "SAW");
				break;

			case 1:
				sprintf(dbgBuf, "PULSE");
				break;

			case 2:
				sprintf(dbgBuf, "SINE");
				break;

			case 3:
				sprintf(dbgBuf, "FM");
				break;

			case 4:
				sprintf(dbgBuf, "PWM");
				break;
		}

	MIOS32_LCD_Clear();
	MIOS32_LCD_CursorSet(0,0);
	switch (osc) {
		case 0:
			MIOS32_LCD_PrintFormattedString("OSC 1");
			break;
		case 1:

			MIOS32_LCD_PrintFormattedString("OSC 2");
			break;
		default:
			break;
	}


	MIOS32_LCD_CursorSet(0,1);
	MIOS32_LCD_PrintFormattedString("Wave : %s",dbgBuf);
}
void LCD_OSCVolume(int vol,int osc){

	MIOS32_LCD_Clear();
	MIOS32_LCD_CursorSet(0,0);
	switch (osc) {
			case 0:
				MIOS32_LCD_PrintFormattedString("OSC 1");
				break;
			case 1:

				MIOS32_LCD_PrintFormattedString("OSC 2");
				break;
			case 2:

				MIOS32_LCD_PrintFormattedString("OSC SUB");
				break;
			case 3:

				MIOS32_LCD_PrintFormattedString("MASTER");
				break;
			default:
				break;
		}


	MIOS32_LCD_CursorSet(0,1);
	MIOS32_LCD_PrintFormattedString("Volume : %d",vol);
}
void LCD_PWM(int width){
	MIOS32_LCD_Clear();
	MIOS32_LCD_CursorSet(0,0);
	MIOS32_LCD_PrintFormattedString("PWM");

	MIOS32_LCD_CursorSet(0,1);
	MIOS32_LCD_PrintFormattedString("Witdth : %d",width);
}
void LCD_FM(int fm){
	MIOS32_LCD_Clear();
	MIOS32_LCD_CursorSet(0,0);
	MIOS32_LCD_PrintFormattedString("FM");

	MIOS32_LCD_CursorSet(0,1);
	MIOS32_LCD_PrintFormattedString("Modulation : %d",fm);
}
void LCD_DELAYLength(int length){
	MIOS32_LCD_Clear();
	MIOS32_LCD_CursorSet(0,0);
	MIOS32_LCD_PrintFormattedString("DELAY");

	MIOS32_LCD_CursorSet(0,1);
	MIOS32_LCD_PrintFormattedString("Length : %d",length);
}
void LCD_DELAYFeedback(int feedback){
	MIOS32_LCD_Clear();
	MIOS32_LCD_CursorSet(0,0);
	MIOS32_LCD_PrintFormattedString("DELAY");

	MIOS32_LCD_CursorSet(0,1);
	MIOS32_LCD_PrintFormattedString("Feedback : %d",feedback);
}


void LCD_OSCDetune(int detune,int osc){

	MIOS32_LCD_Clear();
	MIOS32_LCD_CursorSet(0,0);
	switch (osc) {
			case 0:
				MIOS32_LCD_PrintFormattedString("OSC 1");
				break;
			case 1:

				MIOS32_LCD_PrintFormattedString("OSC 2");
				break;
			default:
				break;
		}


	MIOS32_LCD_CursorSet(0,1);
	MIOS32_LCD_PrintFormattedString("Detune : %d",detune);
}
void LCD_Attack(int attack){
	MIOS32_LCD_Clear();
	MIOS32_LCD_CursorSet(0,0);

	MIOS32_LCD_PrintFormattedString("Envelope");



	MIOS32_LCD_CursorSet(0,1);
	MIOS32_LCD_PrintFormattedString("Attack : %d",attack);
}
void LCD_Decay(int decay){
	MIOS32_LCD_Clear();
	MIOS32_LCD_CursorSet(0,0);

	MIOS32_LCD_PrintFormattedString("Envelope");



	MIOS32_LCD_CursorSet(0,1);
	MIOS32_LCD_PrintFormattedString("Decay : %d",decay);
}
void LCD_Sustain(int sustain){
	MIOS32_LCD_Clear();
	MIOS32_LCD_CursorSet(0,0);

	MIOS32_LCD_PrintFormattedString("Envelope");



	MIOS32_LCD_CursorSet(0,1);
	MIOS32_LCD_PrintFormattedString("Sustain : %d",sustain);
}
void LCD_Release(int release){
	MIOS32_LCD_Clear();
	MIOS32_LCD_CursorSet(0,0);

	MIOS32_LCD_PrintFormattedString("Envelope");



	MIOS32_LCD_CursorSet(0,1);
	MIOS32_LCD_PrintFormattedString("Release : %d",release);
}
void LCD_LFORun(int run){
	MIOS32_LCD_Clear();
	MIOS32_LCD_CursorSet(0,0);

	MIOS32_LCD_PrintFormattedString("LFO");



	MIOS32_LCD_CursorSet(0,1);
	if (run) {

		MIOS32_LCD_PrintFormattedString("Running");
	}
	else {

		MIOS32_LCD_PrintFormattedString("Stopped");
	}
}
void LCD_LFORate(int rate){
	MIOS32_LCD_Clear();
	MIOS32_LCD_CursorSet(0,0);

	MIOS32_LCD_PrintFormattedString("LFO");

	MIOS32_LCD_CursorSet(0,1);

	MIOS32_LCD_PrintFormattedString("Rate : %d",rate);
}
void LCD_LFOMode(int mode){
	MIOS32_LCD_Clear();
	MIOS32_LCD_CursorSet(0,0);

	MIOS32_LCD_PrintFormattedString("LFO");

	MIOS32_LCD_CursorSet(0,1);
		switch (mode) {
			case 0:
				sprintf(dbgBuf, "SIN");
				break;

			case 1:
				sprintf(dbgBuf, "PULSE");
				break;

			case 2:
				sprintf(dbgBuf, "SAW");
				break;
		}

	MIOS32_LCD_CursorSet(0,1);

	MIOS32_LCD_PrintFormattedString("Mode : %s",dbgBuf);
}
void LCD_LFOTarget(int target){

	MIOS32_LCD_Clear();
	MIOS32_LCD_CursorSet(0,0);

	MIOS32_LCD_PrintFormattedString("LFO");
	MIOS32_LCD_CursorSet(0,1);
		switch (target) {
				case 0:
					sprintf(dbgBuf, "VOLUME");
					break;

				case 1:
					sprintf(dbgBuf, "FILTER");
					break;

				case 2:
					sprintf(dbgBuf, "PITCH");
					break;
			}

	MIOS32_LCD_CursorSet(0,1);

	MIOS32_LCD_PrintFormattedString("Target : %s",dbgBuf);
}
void LCD_LFOTop(int top){

	MIOS32_LCD_Clear();
	MIOS32_LCD_CursorSet(0,0);

	MIOS32_LCD_PrintFormattedString("LFO");
	MIOS32_LCD_CursorSet(0,1);

	MIOS32_LCD_PrintFormattedString("Top : %d",top);
}

void LCD_Filter(u16 cutoff,u8 q,u8 mode,u8 eg){

	switch ((mode>>5)) {
		case 0:
			sprintf(dbgBuf, "LP");
			break;
		case 1:
			sprintf(dbgBuf, "BP");
			break;
		case 2:
			sprintf(dbgBuf, "HP");
			break;
		default:
			sprintf(dbgBuf, "HP");
			break;
	}
	MIOS32_LCD_Clear();
	MIOS32_LCD_CursorSet(0,0);
	MIOS32_LCD_PrintFormattedString("Filter %s EG:%d",dbgBuf,eg);
	MIOS32_LCD_CursorSet(0,1);
	MIOS32_LCD_PrintFormattedString("C: %d",cutoff>>8);
	MIOS32_LCD_CursorSet(7,1);
	MIOS32_LCD_PrintFormattedString("R: %d",q<<1);

}
void LCD_ADSR(u32 attack,u8 decay,u8 sustain,u8 release){
	MIOS32_LCD_Clear();
	MIOS32_LCD_CursorSet(0,0);
	MIOS32_LCD_PrintFormattedString("EG A: %d",attack);

	MIOS32_LCD_CursorSet(10,0);
	MIOS32_LCD_PrintFormattedString("D: %d",decay);
	MIOS32_LCD_CursorSet(3,1);
	MIOS32_LCD_PrintFormattedString("S: %d",sustain);
	MIOS32_LCD_CursorSet(10,1);
	MIOS32_LCD_PrintFormattedString("R: %d",release);

}
void LCD_OSC_DETUNE(int detune1, int detune2){
	MIOS32_LCD_Clear();
	MIOS32_LCD_CursorSet(0,0);
	MIOS32_LCD_PrintFormattedString("OSC 1: %d     ",detune1);

	MIOS32_LCD_CursorSet(0,1);
	MIOS32_LCD_PrintFormattedString("OSC 2: %d     ",detune2);
}
void LCD_LFO(int run, int speed, int mode, int target){
	MIOS32_LCD_Clear();
	MIOS32_LCD_CursorSet(0,0);
	if (run) {
		MIOS32_LCD_PrintFormattedString("LFO on RATE: %d   ",speed);
	}
	else{
		MIOS32_LCD_PrintFormattedString("LFO off           ");
	}

	MIOS32_LCD_CursorSet(0,1);
	switch (mode) {
		case 0:
			sprintf(dbgBuf, "SIN");
			break;

		case 1:
			sprintf(dbgBuf, "PULSE");
			break;

		case 2:
			sprintf(dbgBuf, "SAW");
			break;
	}
	MIOS32_LCD_PrintFormattedString("%s",dbgBuf);
	MIOS32_LCD_CursorSet(7,1);
	switch (target) {
			case 0:
				sprintf(dbgBuf, "VOLUME");
				break;

			case 1:
				sprintf(dbgBuf, "FILTER");
				break;
		}

	MIOS32_LCD_PrintFormattedString("%s",dbgBuf);
}

void LCD_OSCSUBVolume(int vol){
	MIOS32_LCD_Clear();
	MIOS32_LCD_CursorSet(0,0);
	MIOS32_LCD_PrintFormattedString("OSC SUB");

	MIOS32_LCD_CursorSet(8,0);
	MIOS32_LCD_PrintFormattedString("vol: %d",vol);

}
void LCD_OSC1(int vol, int wave){
	MIOS32_LCD_Clear();
	MIOS32_LCD_CursorSet(0,0);
	switch (wave) {
		case 0:
			sprintf(dbgBuf, "SAW");
			break;

		case 1:
			sprintf(dbgBuf, "PULSE");
			break;

		case 2:
			sprintf(dbgBuf, "SINE");
			break;

		case 3:
			sprintf(dbgBuf, "FM");
			break;

		case 4:
			sprintf(dbgBuf, "PWM");
			break;
	}
	MIOS32_LCD_PrintFormattedString("OSC 1");

	MIOS32_LCD_CursorSet(0,1);

	MIOS32_LCD_PrintFormattedString("%s",dbgBuf);

	MIOS32_LCD_CursorSet(7,1);

	MIOS32_LCD_PrintFormattedString("vol: %d", vol);

}

void LCD_OSC2(int vol, int wave){
	MIOS32_LCD_Clear();
	MIOS32_LCD_CursorSet(0,0);
	switch (wave) {
		case 0:
			sprintf(dbgBuf, "SAW");
			break;

		case 1:
			sprintf(dbgBuf, "PULSE");
			break;

		case 2:
			sprintf(dbgBuf, "SINE");
			break;

		case 3:
			sprintf(dbgBuf, "FM");
			break;

		case 4:
			sprintf(dbgBuf, "PWM");
			break;
	}
	MIOS32_LCD_PrintFormattedString("OSC 2");

	MIOS32_LCD_CursorSet(0,1);

	MIOS32_LCD_PrintFormattedString("%s",dbgBuf);

	MIOS32_LCD_CursorSet(7,1);

	MIOS32_LCD_PrintFormattedString("vol: %d", vol);

}
