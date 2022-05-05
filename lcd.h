/*
 * lcd.h
 *
 *  Created on: Jul 10, 2020
 *      Author: Martin
 */

#ifndef LCD_H_
#define LCD_H_

extern void LCD_FilterCutoff(u16 cutoff);
extern void LCD_FilterResonance(u16 q);
extern void LCD_FilterMode(u8 mode);
extern void LCD_FilterEnvelope(u8 eg);
extern void LCD_OSCWaveform(int wave,int osc);
extern void LCD_OSCVolume(int vol,int osc);
extern void LCD_OSCDetune(int detune,int osc);
extern void LCD_Attack(int attack);
extern void LCD_Decay(int decay);
extern void LCD_Sustain(int sustain);
extern void LCD_Release(int release);
extern void LCD_LFORun(int run);
extern void LCD_LFORate(int rate);
extern void LCD_LFOMode(int mode);
extern void LCD_LFOTarget(int target);
extern void LCD_LFOTop(int top);
extern void LCD_PWM(int width);
extern void LCD_FM(int fm);
extern void LCD_DELAYLength(int length);
extern void LCD_DELAYFeedback(int feedback);
extern void LCD_Filter(u16 cutoff,u8 q,u8 mode,u8 eg);
extern void LCD_ADSR(u32 attack,u8 decay,u8 sustain, u8 release);
extern void LCD_OSC_DETUNE(int detune1, int detune2);
extern void LCD_LFO(int run, int speed, int mode, int target);
extern void LCD_OSC1(int vol, int wave);
extern void LCD_OSC2(int vol, int wave);

#endif /* LCD_H_ */
