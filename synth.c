// 
/*TODO:LFO top LCD implement
 * LFO pitch target

 *
 */

/////////////////////////////////////////////////////////////////////////////
// Include files
/////////////////////////////////////////////////////////////////////////////
#include <mios32.h>

#include <math.h>

#include "synth.h"

#include <FreeRTOS.h>
#include <portmacro.h>
#include <stdint.h>
#include <lcd.h>
#include "synthTab.h"
#include <notestack.h>

#include "frqtab.h"

#include "lfo.h"
#include <jsw_rand.h>

#include <seq_bpm.h>
#include <seq_midi_out.h>

/////////////////////////////////////////////////////////////////////////////
// Local definitions
/////////////////////////////////////////////////////////////////////////////

#define SAMPLE_BUFFER_SIZE 128  // -> 128 L/R samples, 375 Hz refill rate (2.6~ mS period)

#define WAVETABLE_SIZE 2048//512
#define WAVETABLE_HALF 1023//512
#define SAMPLERATE  48000
#define WAVETABLE_FRQ  (SAMPLERATE/WAVETABLE_SIZE)

#define WAVETABLE_SIZE_LFO 128

#define OSC_CHN  2

#define NOTESTACK_SIZE 16

#define OSC  4
#define lcd 1

/////////////////////////////////////////////////////////////////////////////
// Local Variables
/////////////////////////////////////////////////////////////////////////////

static notestack_t notestack;

char dbgBuf[20];

static u32 timerCnt = 0;

// sample buffer
static u32 sample_buffer[SAMPLE_BUFFER_SIZE];

u32 pin_menu = 0;

u32 pin_CC1 = 0;

/////////////////////////////////////////////////////////////////////////////
// Local Prototypes
/////////////////////////////////////////////////////////////////////////////

void SYNTH_ReloadSampleBuffer(u32 state);

void SYNTH_OctaveSet(u8 osc, u8 value);

u32 SYNTH_MixSample();

void printfloat(float value);

void printfloat(float value) {
	float outF = value;
	int tmpInt1 = outF;
	float tmpFrac = outF - tmpInt1;
	int tmpInt2 = trunc(tmpFrac * 10000);
	sprintf(dbgBuf, "%d.%04d", tmpInt1, tmpInt2);

	MIOS32_MIDI_SendDebugMessage(dbgBuf);
}
s32 SYNTH_Pulse(u32 acc, u32 pos);
s32 SYNTH_Saw(u32 acc, u32 pos);
s32 SYNTH_Sample(u32 acc, u32 pos);
s32 SYNTH_Sin(u32 acc, u32 pos);
s32 SYNTH_FM(u32 acc, u32 pos);
s32 SYNTH_PWM(u32 acc, u32 pos, u32 width);

u32 FILTER_StateVariableResonance(u32 input, u16 cutoff, u32 Q, u8 mode);

u32 FILTER_StateVariable(u32 input, u16 cutoff, u8 mode);

u32 ENVELOPE(u8 voice, u32 time);

s32 SYNTH_OSC_output(synth_waveform_t waves[2], u32 acc1, u32 pos1, u32 acc2, u32 pos2, u32 accSub, u32 posSub, u8 vel);

u32 EFFECTS_delay(u32 input);

static s32 LFO_Tick(u32 bpm_tick);

/////////////////////////////////////////////////////////////////////////////
// initializes the synth
/////////////////////////////////////////////////////////////////////////////
s32 SYNTH_Init(u32 mode) {


	int a;

	int chn;
	for (chn = 0; chn < 2; chn++) {
		SYNTH_WaveformSet(chn, WAVEFORM_SAW);
	}

	int i;
	for (i = 0; i < POLY; i++) {
		voices[i].note = 0xff;
		voices[i].e_stage = ENVELOPE_OFF;
	}
	int b;
	for (b = 0; b < WAVETABLE_SIZE; b++) {
		sintab[b] = (s16) (128 * sin((float) b * M_TWOPI / (float) WAVETABLE_SIZE)) + 128;

	}
	int c;
	for (c = 0; c < 48000; c++) {
		delay_buffer[c] = 0;

	}
	delay_index = 0;

	SYNTH_LFOModeSet(0);
	SYNTH_VolOSCSet(64, 0, 1);
	SYNTH_VolOSCSet(120, 1, 1);
	SYNTH_VolOSCSet(20, 2, 1);
	SYNTH_VolOSCSet(127, 3, 1);

	threshold = 127 << 8;
	ENVELOPE_AttackSet(0);
	ENVELOPE_DecaySet(30);
	ENVELOPE_SustainSet(60);
	ENVELOPE_ReleaseSet(10);
	SYNTH_FilterFrequencySet(10, 1);
	SYNTH_FilterQSet(0);
	SEQ_BPM_ModeSet(SEQ_BPM_MODE_Master);
	SEQ_BPM_Init(0);

	SEQ_BPM_PPQN_Set(384);
	SEQ_BPM_Set(10.0);
	pitchbend = 0;
	SYNTH_DetuneOSCSet(63, 0, 1);

	SYNTH_DetuneOSCSet(55, 0, 1);
	SYNTH_LFOTopSet(1);
	return MIOS32_I2S_Start((u32 *) &sample_buffer[0], SAMPLE_BUFFER_SIZE, &SYNTH_ReloadSampleBuffer);
}
#define debug 1
void SYNTH_AINMenu(u32 pin, u32 pin_value) {
#if debug
	MIOS32_MIDI_SendDebugMessage("pot %d %d", pin, pin_value);
#endif
	if (pin == 0) {
		pin_menu = pin_value / 170;

#if debug
		MIOS32_MIDI_SendDebugMessage("menu value %d", pin_menu);
#endif
	}
	if (pin == 1) {
		pin_CC1 = pin_value >> 5;
#if debug
		MIOS32_MIDI_SendDebugMessage("change value %d", pin_CC1);
#endif
	}
	switch (pin_menu) {
	case 0:
		if (pin == 1) {
			SYNTH_WaveformSet(0, pin_CC1);
		} else {
			LCD_OSCWaveform(waveform_select[0], 0);
		}
		break;
	case 1:
		if (pin == 1) {
			SYNTH_VolOSCSet(pin_CC1, 0, 0);
		} else {
			LCD_OSCVolume(OSC1_vol, 0);
		}
		break;
	case 2:
		if (pin == 1) {
			SYNTH_DetuneOSCSet(pin_CC1, 0, 0);
		} else {
			LCD_OSCDetune(detune_idx1, 0);
		}
		break;
	case 3:
		if (pin == 1) {
			SYNTH_WaveformSet(1, pin_CC1);
		} else {
			LCD_OSCWaveform(waveform_select[1], 1);
		}
		break;

	case 4:
		if (pin == 1) {
			SYNTH_VolOSCSet(pin_CC1, 1, 0);
		} else {
			LCD_OSCVolume(OSC2_vol, 1);
		}
		break;
	case 5:
		if (pin == 1) {
			SYNTH_DetuneOSCSet(pin_CC1, 1, 0);
		} else {
			LCD_OSCDetune(detune_idx2, 1);
		}
		break;
	case 6:
		if (pin == 1) {
			SYNTH_PWMSet(pin_CC1);
		} else {
			LCD_PWM(PWM);
		}
		break;
	case 7:
		if (pin == 1) {
			SYNTH_FMSet(pin_CC1);
		} else {
			LCD_FM(FM_modulate_val1);
		}
		break;
	case 8:
		if (pin == 1) {
			ENVELOPE_AttackSet(pin_CC1);
		} else {
			LCD_Attack(attack_time);
		}
		break;
	case 9:
		if (pin == 1) {
			ENVELOPE_DecaySet(pin_CC1);
		} else {
			LCD_Decay(decay_time);
		}
		break;
	case 10:
		if (pin == 1) {
			ENVELOPE_SustainSet(pin_CC1);
		} else {
			LCD_Sustain(sustain);
		}
		break;
	case 11:
		if (pin == 1) {
			ENVELOPE_ReleaseSet(pin_CC1);
		} else {
			LCD_Release(release);
		}
		break;
	case 12:
		if (pin == 1) {
			SYNTH_FilterFrequencySet(pin_CC1, 0);
		} else {
			LCD_FilterCutoff(filter_cutoff >> 8);
		}
		break;
	case 13:
		if (pin == 1) {
			SYNTH_FilterQSet(pin_CC1);
		} else {
			LCD_FilterResonance(filter_Q << 1);
		}
		break;
	case 14:
		if (pin == 1) {
			SYNTH_FilterModeSet(pin_CC1);
		} else {
			LCD_FilterMode(filter_mode);
		}
		break;

	case 15:
		if (pin == 1) {
			SYNTH_FilterEnvelopeSet(pin_CC1);
		} else {
			LCD_FilterEnvelope(filter_envelope);
		}
		break;
	case 16:
		if (pin == 1) {
			SYNTH_LFORunSet(pin_CC1);
		} else {
			LCD_LFORun(LFO_on);
		}
		break;
	case 17:
		if (pin == 1) {
			SYNTH_LFORateSet(pin_CC1);
		} else {
			LCD_LFORate(LFO_rate);
		}
		break;
	case 18:
		if (pin == 1) {
			SYNTH_LFOModeSet(pin_CC1);
		} else {
			LCD_LFOMode(LFO_mode);
		}
		break;
	case 19:
		if (pin == 1) {
			SYNTH_LFOTarget(pin_CC1);
		} else {
			LCD_LFOTarget(LFO_target);
		}
		break;

	case 20:
		if (pin == 1) {
			SYNTH_LFOTopSet(pin_CC1);
		} else {
			LCD_LFOTop(LFO_top);
		}
		break;
	case 21:
		if (pin == 1) {
			SYNTH_DELAYLengthSet(pin_CC1);
		} else {
			LCD_DELAYLength(delay_val);
		}
		break;
	case 22:
		if (pin == 1) {
			SYNTH_DELAYFeedbackSet(pin_CC1);
		} else {
			LCD_DELAYFeedback(feedback_val);
		}
		break;

	case 23:
		if (pin == 1) {
			SYNTH_VolOSCSet(pin_CC1, 3, 0);
		} else {
			LCD_OSCVolume(vol, 3);
		}
		break;
	case 24:
		if (pin == 1) {
			SYNTH_VolOSCSet(pin_CC1, 2, 0);
		} else {
			LCD_OSCVolume(SUB_vol, 2);
		}
		break;

	default:
		break;
	}
}

static s32 LFO_Tick(u32 bpm_tick) {
	// whenever we reach a new 16th note (96 ticks @384 ppqn):

	switch (LFO_target) {
	case 0:
		SYNTH_VolOSCSet(LFO_Out(LFO_mode, LFO_phase) / LFO_top, 0, 1);
		SYNTH_VolOSCSet(LFO_Out(LFO_mode, LFO_phase) / LFO_top, 1, 1);
		SYNTH_VolOSCSet(LFO_Out(LFO_mode, LFO_phase) / LFO_top, 2, 1);
		break;
	case 1:
		SYNTH_FilterFrequencySet(LFO_Out(LFO_mode, LFO_phase) / LFO_top, 1);
		break;
	case 2:
		SYNTH_DetuneOSCSet(LFO_Out(LFO_mode, LFO_phase) / LFO_top, 0, 1);
		SYNTH_DetuneOSCSet(LFO_Out(LFO_mode, LFO_phase) / LFO_top, 1, 1);
		break;
	}
	if ((LFO_phase++) >= WAVETABLE_SIZE_LFO)
		LFO_phase = 0;
	if ((bpm_tick % (SEQ_BPM_PPQN_Get() / 4)) == 0) {
	}

	return 0; // no error
}

s32 LFO_Handler(void) {
	// handle requests

	u8 num_loops = 0;
	u8 again = 0;
	do {
		++num_loops;

		// note: don't remove any request check - clocks won't be propagated
		// so long any Stop/Cont/Start/SongPos event hasn't been flagged to the sequencer
		if (SEQ_BPM_ChkReqStop()) {
			//SEQ_PlayOffEvents();
		}

		if (SEQ_BPM_ChkReqCont()) {
			// release pause mode
			seq_pause = 0;
		}

		if (SEQ_BPM_ChkReqStart()) {
			//SEQ_Reset();
		}

		u16 new_song_pos;
		if (SEQ_BPM_ChkReqSongPos(&new_song_pos)) {
			//SEQ_PlayOffEvents();
		}

		u32 bpm_tick;
		if (SEQ_BPM_ChkReqClk(&bpm_tick) > 0) {
			again = 1; // check all requests again after execution of this part

			LFO_Tick(bpm_tick);
		}
	} while (again && num_loops < 10);

	return 0; // no error
}

/////////////////////////////////////////////////////////////////////////////
// This function updates the selected waveform
/////////////////////////////////////////////////////////////////////////////
s32 SYNTH_WaveformSet(u8 chn, synth_waveform_t waveform) {
	u8 val = waveform / 31;
	waveform_select[chn] = val;
#if lcd
	LCD_OSCWaveform(waveform_select[chn], chn);
#endif

	return 0; // no error
}
void SYNTH_FilterModeSet(int val) {

	filter_mode = val;
#if lcd
	LCD_FilterMode(filter_mode);
#endif
}
void SYNTH_FilterEnvelopeSet(int val) {
	filter_envelope = val;
#if lcd
	LCD_FilterEnvelope(filter_envelope);
#endif
}
void SYNTH_LFOTopSet(int val) {
#if debug
	MIOS32_MIDI_SendDebugMessage("lfo top: %d", LFO_top);
#endif

	LFO_top = val >= 1 ? val : 1;
#if lcd
	LCD_LFOTop(LFO_top);
#endif
}

/////////////////////////////////////////////////////////////////////////////
// This function sets a new frequency
/////////////////////////////////////////////////////////////////////////////

s32 SYNTH_FreqSet_OSC(u8 voice, float frq, int osc) {

	// calculate new accumulator
	switch (osc) {
	case 0:
		voices[voice].wave_acc_OSC1 = (u32) (((frq / (WAVETABLE_FRQ)) * (float) (1 << FP_PRECISSION)));
		break;
	case 1:
		voices[voice].wave_acc_OSC2 = (u32) (((frq / (WAVETABLE_FRQ)) * (float) (1 << FP_PRECISSION)));
		break;
	case 2:
		voices[voice].wave_acc_SUB = (u32) ((((frq / 2) / (WAVETABLE_FRQ)) * (float) (1 << FP_PRECISSION)));
		break;
	default:
		break;
	}

	return 0; // no error
}

void SYNTH_VolOSCSet(int val, int osc, int lfo) {
	switch (osc) {
	case 0:

		OSC1_vol = val;
		break;
	case 1:

		OSC2_vol = val;
		break;
	case 2:

		SUB_vol = val;
		break;
	case 3:

		vol = val;
		break;
	default:
		break;
	}
#if lcd
	if (!lfo) {

		switch (osc) {
		case 0:

			LCD_OSCVolume(OSC1_vol, osc);
			break;
		case 1:

			LCD_OSCVolume(OSC2_vol, osc);
			break;
		case 2:

			LCD_OSCVolume(SUB_vol, osc);
			break;
		case 3:
			LCD_OSCVolume(vol, osc);
			break;
		default:
			break;
		}
	}
#endif
}

void ENVELOPE_AttackSet(int val) {
	attack_time = val;
#if lcd
	LCD_Attack(attack_time);
#endif
}

void ENVELOPE_DecaySet(int val) {
	decay_time = val;
#if lcd
	LCD_Decay(decay_time);
#endif
}

void ENVELOPE_SustainSet(int val) {
	sustain = val;
#if lcd
	LCD_Sustain(sustain);
#endif
}

void ENVELOPE_ReleaseSet(int val) {
	release = val;
#if lcd
	LCD_Release(release);
#endif
}

void SYNTH_MasterVolumeSet(int val) {
	vol = val;
#if lcd
	LCD_OSCVolume(vol, 3);
#endif
}

void SYNTH_LFORunSet(int val) {
	LFO_on = val / 64;
	if (LFO_on)
		SEQ_BPM_Start();
	else
		SEQ_BPM_Stop();
#if lcd
	LCD_LFORun(LFO_on);
#endif
}

void SYNTH_LFORateSet(int val) {
	LFO_rate = val;
	SEQ_BPM_Set((float) (LFO_rate));
#if lcd
	LCD_LFORate(LFO_rate);
#endif

}

void SYNTH_LFOTarget(int val) {
	LFO_target = val / 50;
#if lcd
	LCD_LFOTarget(LFO_target);
#endif
}

void SYNTH_LFOModeSet(int val) {

	LFO_mode = (val - 2) / 42;
#if lcd
	LCD_LFOMode(LFO_mode);
#endif
}

void SYNTH_PWMSet(int val) {
	PWM = val + 1;
#if lcd
	LCD_PWM(PWM);
#endif
}

void SYNTH_FMSet(int val) {
	FM_modulate_val1 = val;
#if lcd
	LCD_FM(FM_modulate_val1);
#endif
}

void SYNTH_DELAYLengthSet(int val) {
	delay_val = val;
#if lcd
	LCD_DELAYLength(delay_val);
#endif
}

void SYNTH_DELAYFeedbackSet(int val) {
	feedback_val = val;
#if lcd
	LCD_DELAYFeedback(feedback_val);
#endif
}

void SYNTH_DetuneOSCSet(int val, int osc, int lfo) {
	switch (osc) {
	case 0:
		detune_idx1 = val;
		break;
	case 1:
		detune_idx2 = val;
		break;
	default:
		break;
	}

#if debug
	MIOS32_MIDI_SendDebugMessage("detune1 %d", detune_idx1);
#endif
	int i;
	for (i = 0; i < POLY; i++) {
		if (voices[i].e_stage != ENVELOPE_OFF) {
			SYNTH_FreqSet_OSC(i, frqtab[voices[i].note] * detune[val], osc); //* pow(2, (float)octave1 / (float)detune));
		}
	}
#if lcd

#endif

#if lcd
	if (!lfo) {

		LCD_OSCDetune(val, osc);
	}
#endif
}

void SYNTH_HandleMidi(mios32_midi_port_t port, mios32_midi_package_t midi_package) {

	// if note event over MIDI channel #1 controls note of both oscillators
	// Note On received?
	if (midi_package.chn == Chn1 && (midi_package.type == NoteOn || midi_package.type == NoteOff)) {

		// branch depending on Note On/Off event
		if (midi_package.event == NoteOn && midi_package.velocity > 0) {
			// push note into note stack
			//NOTESTACK_Push(&notestack, midi_package.note, midi_package.velocity);
			//SEQ_BPM_Start();
			int i;
			int k;
			for (i = 0; i < POLY; i++) {
				if (voices[i].note == 0xff && voices[i].e_stage == ENVELOPE_OFF && voices[i].e_stage != ENVELOPE_RELEASE) {
					voices[i].note = midi_package.note;
					voices[i].velocity = midi_package.velocity;
					voices[i].chn = i;
					voices[i].on_time = xTaskGetTickCount();
					MIOS32_MIDI_SendDebugMessage("Time:%d", voices[i].on_time);
					voices[i].on_note = true;
					voices[i].e_stage = ENVELOPE_ATTACK;
					SYNTH_FreqSet_OSC(i, frqtab[voices[i].note] * detune[detune_idx1], 0); //pow(2, octave1 / detune));

					SYNTH_FreqSet_OSC(i, frqtab[voices[i].note] * detune[detune_idx2], 1); //pow(2, octave2 / detune));
					SYNTH_FreqSet_OSC(i, frqtab[voices[i].note] * detune[detune_idx2], 2);

					break;
				}
			}
		}
		if (midi_package.type == NoteOff) {
			// remove note from note stack

			//NOTESTACK_Pop(&notestack, midi_package.note);
			//SEQ_BPM_Stop();
			int i;
			int chn;
			for (i = 0; i < POLY; ++i) {
				if (voices[i].note == midi_package.note && voices[i].e_stage != ENVELOPE_RELEASE) {
					voices[i].off_time = xTaskGetTickCount();
					voices[i].e_stage = ENVELOPE_RELEASE;

				}
			}
		}
		MIOS32_BOARD_LED_Set(1, 1);
	} //end of working part
	  // turn off LED (can also be used as a gate output!)*/
	MIOS32_BOARD_LED_Set(1, 0);

	if (midi_package.event == CC && midi_package.chn == Chn1) {
		switch (midi_package.cc_number) {

		case 9:      //2 OSC
		{
#if debug
			MIOS32_MIDI_SendDebugMessage("subvol: %d", midi_package.value);
#endif
			SYNTH_VolOSCSet(midi_package.value, 2, 0);
		}
			break;
		case 10:      //2 OSC
		{
#if debug
			MIOS32_MIDI_SendDebugMessage("oscvol1: %d", midi_package.value);
#endif
			SYNTH_VolOSCSet(midi_package.value, 0, 0);
		}
			break;
		case 11:      //2 OSC
		{
#if debug
			MIOS32_MIDI_SendDebugMessage("oscvol2: %d", midi_package.value);
#endif
			SYNTH_VolOSCSet(midi_package.value, 1, 0);
		}
			break;
		case 12:      //1 OSC
		{
#if debug
			MIOS32_MIDI_SendDebugMessage("OSC1: %d", midi_package.value);
#endif
			SYNTH_WaveformSet(0, midi_package.value);
		}
			break;
		case 13:        //2 OSC
		{
			SYNTH_WaveformSet(1, midi_package.value);
		}
			break;

		case 14:      //FM
			SYNTH_FMSet(midi_package.value);
			break;
		case 15:      //FM
			SYNTH_PWMSet(midi_package.value);
			break;
		case 7:            //2 OSC
		{
			vol = midi_package.value;
			MIOS32_MIDI_SendDebugMessage("vol %d", vol);

		}
			break;

		case 21:            //attack
			ENVELOPE_AttackSet(midi_package.value);
			break;

		case 22:            //attack
			ENVELOPE_DecaySet(midi_package.value);
			break;
		case 23:            //sus

			ENVELOPE_SustainSet(midi_package.value);
			break;
		case 24:            //relese
			ENVELOPE_ReleaseSet(midi_package.value);
			break;

		case 25:            //retune osc 1
			SYNTH_DetuneOSCSet(midi_package.value, 0, 0);
			break;

		case 26:      //retune osc 2
			SYNTH_DetuneOSCSet(midi_package.value, 1, 0);
			break;
		case 28:      //filter freq
			SYNTH_FilterFrequencySet(midi_package.value, 0);
			break;
		case 29:      //filter Q
			SYNTH_FilterQSet(midi_package.value);
			break;

		case 30:      //Mode
			SYNTH_FilterModeSet(midi_package.value);
			break;

		case 1:      //Modulator

			SYNTH_FilterFrequencySet(midi_package.value, 0);
			break;
		case 32:      //arpmode
			SYNTH_FilterEnvelopeSet(midi_package.value);
			break;

		case 33:      //arpmode
			SYNTH_LFORunSet(midi_package.value);
			break;
		case 34:      //bpm
			SYNTH_LFORateSet(midi_package.value);
			break;

		case 35:      //LFO
			SYNTH_LFOTarget(midi_package.value);
			break;

		case 36:      //LFO
			SYNTH_LFOModeSet(midi_package.value);
			break;
		case 37:      //delay
			SYNTH_LFOTopSet(midi_package.value);
			break;

		case 38:      //delay
			SYNTH_DELAYLengthSet(midi_package.value);
			break;
		case 39:      //delay
			SYNTH_DELAYFeedbackSet(midi_package.value);
			break;

		}

	}
	if (midi_package.event == PitchBend) {
		int i;
		int value = midi_package.value - 64;
		MIOS32_MIDI_SendDebugMessage("pitchbend %d", value);
		MIOS32_IRQ_Disable();
		for (i = 0; i < POLY; i++) {
			if (voices[i].e_stage != ENVELOPE_OFF) {

				SYNTH_FreqSet_OSC(i, frqtab[voices[i].note] * detune[detune_idx1] + value, 0);
				SYNTH_FreqSet_OSC(i, frqtab[voices[i].note] * detune[detune_idx2] + value, 1);
				SYNTH_FreqSet_OSC(i, (frqtab[voices[i].note] / 2) + value, 2);
			}
		}
		MIOS32_IRQ_Enable();
	}
}

/////////////////////////////////////////////////////////////////////////////
// This function is called by MIOS32_I2S when the lower (state == 0) or 
// upper (state == 1) range of the sample buffer has been transfered, so 
// that it can be updated
/////////////////////////////////////////////////////////////////////////////

void SYNTH_ReloadSampleBuffer(u32 state) {
	// transfer new samples to the lower/upper sample buffer range
	int i;
	u32 *buffer = (u32 *) &sample_buffer[state ? (SAMPLE_BUFFER_SIZE / 2) : 0];

	u32 chn1_value = 0;
	for (i = 0; i < SAMPLE_BUFFER_SIZE / 2; ++i) {
		MIOS32_IRQ_Disable();
		chn1_value = (vol * SYNTH_MixSample()) / 128;
		MIOS32_IRQ_Enable();

		*buffer++ = ((u32) chn1_value << 16) | (chn1_value & 0xffff);
	}
}

u32 SYNTH_MixSample() {
	int i;
	u8 x = 0;
	u32 out = 0;
	u32 tmpOut = 0;
	u32 env = 0;
	u32 filter_env = 0;

	u32 wave1 = 0, wave2 = 0;

	for (i = 0; i < POLY; i++) {
		if (voices[i].on_note && voices[i].e_stage != ENVELOPE_OFF) {
			env = ENVELOPE(i, xTaskGetTickCount());

			if ((voices[i].wave_it_OSC1 += voices[i].wave_acc_OSC1) >= (WAVETABLE_SIZE << FP_PRECISSION))
				voices[i].wave_it_OSC1 -= (WAVETABLE_SIZE << FP_PRECISSION);

			if ((voices[i].wave_it_OSC2 += voices[i].wave_acc_OSC2) >= (WAVETABLE_SIZE << FP_PRECISSION))
				voices[i].wave_it_OSC2 -= (WAVETABLE_SIZE << FP_PRECISSION);

			if ((voices[i].wave_it_SUB += voices[i].wave_acc_SUB) >= (WAVETABLE_SIZE << FP_PRECISSION))
				voices[i].wave_it_SUB -= (WAVETABLE_SIZE << FP_PRECISSION);

			tmpOut = ((env * (SYNTH_OSC_output(waveform_select, voices[i].wave_acc_OSC1, voices[i].wave_it_OSC1, voices[i].wave_acc_OSC2, voices[i].wave_it_OSC2, voices[i].wave_acc_SUB, voices[i].wave_it_SUB, voices[i].velocity))) / 128);

			wave1 = tmpOut;
			wave2 = (wave1 - ((threshold) * (((((wave1 * wave1) / (1 << FP_PRECISSION)) * wave1) / (1 << FP_PRECISSION)) / 3) / (1 << FP_PRECISSION)));
			tmpOut = wave2;      //(FM<<5t)* wave2/(1<<FP_PRECISSION);
			if (tmpOut >= (threshold))
				tmpOut = threshold;
			else if (tmpOut <= 0)
				tmpOut = 0;

			out += tmpOut;

		}

	}

	if (delay_val > 0)
		out = EFFECTS_delay(out);

	u32 cutoff = (((filter_envelope) * ((env & (0xFF)) << 7)) / 128) + (((127 - filter_envelope) * filter_cutoff) / 128);
	out = FILTER_StateVariableResonance(out, cutoff, filter_Q, filter_mode) / 32;

	return out & (0xFFFFFFFF);
}

s32 SYNTH_OSC_output(synth_waveform_t waves[2], u32 acc1, u32 pos1, u32 acc2, u32 pos2, u32 accSub, u32 posSub, u8 vel) {
	s32 out = 0;
	u32 wave1 = 0;

	u32 wave2 = 0;

	u32 sub = 0;
	switch (waves[0]) {
	case WAVEFORM_SAW:
		wave1 = OSC1_vol * (((12287 + SYNTH_Saw(acc1, pos1)) * vel) / 128) / 128;
		break;
	case WAVEFORM_PULSE:

		wave1 = OSC1_vol * (((12287 + SYNTH_Pulse(acc1, pos1)) * vel) / 128) / 128;
		break;
	case WAVEFORM_SINE:

		wave1 = (OSC1_vol * (((12287 + SYNTH_Sin(acc1, pos1)) * vel) / 128) / 128) * 2;
		break;
	case WAVEFORM_FM:

		wave1 = OSC1_vol * (((12287 + SYNTH_FM(acc1, pos1)) * vel) / 128) / 128;
		break;
	case WAVEFORM_PWM:

		wave1 = (OSC1_vol * (((12287 + SYNTH_PWM(acc1, pos1, PWM)) * vel) / 128) / 128) * 2;
		break;
	}
	switch (waves[1]) {
	case WAVEFORM_SAW:
		wave2 = OSC2_vol * (((12287 + SYNTH_Saw(acc2, pos2)) * vel) / 128) / 128;
		break;
	case WAVEFORM_PULSE:

		wave2 = OSC2_vol * (((12287 + SYNTH_Pulse(acc2, pos2)) * vel) / 128) / 128;
		break;
	case WAVEFORM_SINE:

		wave2 = (OSC2_vol * (((12287 + SYNTH_Sin(acc2, pos2)) * vel) / 128) / 128) * 2;
		break;
	case WAVEFORM_FM:

		wave2 = (OSC2_vol * (((12287 + SYNTH_FM(acc2, pos2)) * vel) / 128) / 128) * 2;
		break;
	case WAVEFORM_PWM:

		wave2 = (OSC2_vol * (((12287 + SYNTH_PWM(acc2, pos2, PWM)) * vel) / 128) / 128) * 2;
		break;
	}
	sub = SUB_vol * (((12287 + SYNTH_Pulse(accSub, posSub)) * vel) / 128) / 128;
	out = (wave1 + wave2 + sub);

	return out / 3;
}

s32 SYNTH_Pulse(u32 acc, u32 pos) {
	u32 stepLen = acc * 4;
	s32 out;
	if (pos < ((WAVETABLE_SIZE << FP_PRECISSION) >> 1)) {	//first half of wave
		if (pos < (stepLen)) {
			u32 idx = (((pos * (1 << FP_PRECISSION)) / stepLen) * WAVETABLE_HALF) / (1 << FP_PRECISSION);
			out = sStep[(idx + (FM_modulate_val1 * sintab[((pos / 2) >> FP_PRECISSION)])) % 2048];
		} else {
			out = sStep[(WAVETABLE_HALF + (FM_modulate_val1 * sintab[((pos / 2) >> FP_PRECISSION)])) % 2048];
		}

	} else {
		if (pos < ((WAVETABLE_SIZE << FP_PRECISSION) >> 1) + stepLen) {
			u32 idx = WAVETABLE_HALF + ((((((pos - (WAVETABLE_SIZE << FP_PRECISSION) >> 1) * (1 << FP_PRECISSION)) / stepLen) * WAVETABLE_HALF) / (1 << FP_PRECISSION)) * WAVETABLE_HALF) / (1 << FP_PRECISSION);
			out = sStep[(idx + (FM_modulate_val1 * sintab[((pos / 2) >> FP_PRECISSION)])) % 2048];
		} else {
			out = sStep[(0 + (FM_modulate_val1 * sintab[((pos / 2) >> FP_PRECISSION)])) % 2048];
		}
	}

	return out;
}

//phase lenthg is 2048  PWM(128*16)=1	|	PWM(64*16)=0.5

s32 SYNTH_PWM(u32 acc, u32 pos, u32 width) {
	u32 stepLen = acc * 4;
	s32 out;
	width = width > 5 ? width : 5;
	width <<= 3;	//range of wavetable size
	if (pos < ((width << FP_PRECISSION))) {	//first half of wave
		if (pos < (stepLen)) {
			u32 idx = (((pos * (1 << FP_PRECISSION)) / stepLen) * WAVETABLE_HALF) / (1 << FP_PRECISSION);
			out = sStep[idx];
		} else {
			out = sawTab[pos >> FP_PRECISSION] - sawTab[(s32) (pos - width) < 0 ? ((pos - width) + (WAVETABLE_SIZE << FP_PRECISSION)) >> FP_PRECISSION : (pos - width) >> FP_PRECISSION];
		}

	} else {
		if (pos < ((width << FP_PRECISSION)) + stepLen) {
			u32 idx =
			WAVETABLE_HALF + ((((((pos - (width << FP_PRECISSION)) * (1 << FP_PRECISSION)) / stepLen) * WAVETABLE_HALF) / (1 << FP_PRECISSION)) * WAVETABLE_HALF) / (1 << FP_PRECISSION);
			out = sStep[idx];
		} else {
			out = sawTab[pos >> FP_PRECISSION] - sawTab[(s32) (pos - width) < 0 ? ((pos - width) + (WAVETABLE_SIZE << FP_PRECISSION)) >> FP_PRECISSION : (pos - width) >> FP_PRECISSION];
		}
	}
	return out;
}

s32 SYNTH_Sin(u32 acc, u32 pos) {
	s32 out = sintab[pos >> FP_PRECISSION] << 6;
	return out;
}

s32 SYNTH_FM(u32 acc, u32 pos) {
	s32 out = sintab[((pos >> FP_PRECISSION) + (FM_modulate_val1 * sintab[(pos >> FP_PRECISSION) / 2])) % (2048)] << 5;
	return out;
}

s32 SYNTH_Saw(u32 acc, u32 pos) {

	u32 stepLen = acc * 8;

	s32 out = 0;
	if (pos < ((WAVETABLE_SIZE << FP_PRECISSION) >> 1)) {	//first half of wave
		if (pos < (stepLen)) {
			u32 idx = (((pos * (1 << FP_PRECISSION)) / stepLen) * WAVETABLE_HALF) / (1 << FP_PRECISSION);
			out = sStep[idx];
		} else {
			out = sawTab[pos >> FP_PRECISSION];
		}
	} else {
		out = sawTab[pos >> FP_PRECISSION];
	}

	return out;
}

#define	plus23 2863311530‬
#define	min23 1431655765
void SYNTH_OctaveSet(u8 osc, u8 value) {
}

u32 buf0;

void SYNTH_FilterFrequencySet(u8 value, u8 lfo) {

	filter_cutoff = (value) << 8;
#if lcd
	if (!lfo)
		LCD_FilterCutoff(filter_cutoff >> 8);
#endif
}
void SYNTH_FilterQSet(u8 value) {
	//filter_freq= ((float)(value+1)*70);
	filter_Q = (value) >> 1;
#if lcd
	LCD_FilterResonance(filter_Q >> 1);
#endif
	//SetFilter();
}

u32 EFFECTS_delay(u32 input) {

	u16 out = input % 0xffff;
	s32 idx = (delay_index - (delay_val * 377));

	if (idx < 0) {
		idx += SAMPLERATE;
	}
	u16 tmp = delay_buffer[idx];

	delay_buffer[delay_index] = out + ((tmp * ((u16) feedback_val << 9)) / (1 << FP_PRECISSION));

	//increment buffer
	delay_index++;
	//wrap back to start
	if (delay_index > SAMPLERATE - 1) {
		delay_index -= SAMPLERATE;
	}

	return out + tmp;
}

s32 lp1, hp1, bp1, f;
u16 svf_cutoff;
u8 svf_mode;

u32 FILTER_StateVariable(u32 input, u16 cutoff, u8 mode) {

	mode >>= 5;
	s32 bandpass, lowpass, highpass;
	s32 in;

	cutoff = (cutoff > 2047) ? cutoff : 2048;
	f = cutoff / 4;

	in = input & 0xFFFF;

	lp1 += (f * bp1) / 65535;
	hp1 = in - lp1 - bp1;
	bp1 += (f * hp1) / 65535;
	lowpass = lp1;
	highpass = hp1;
	bandpass = bp1;

	lp1 += (f * bp1) / 65535;
	hp1 = in - lp1 - bp1;
	bp1 += (f * hp1) / 65535;
	lowpass += lp1;
	highpass += hp1;
	bandpass += bp1;

	lp1 += (f * bp1) / 65535;
	hp1 = in - lp1 - bp1;
	bp1 += (f * hp1) / 65535;
	lowpass += lp1;
	highpass += hp1;
	bandpass += bp1;

	lowpass /= 3;
	highpass /= 4;
	bandpass /= 3;

	switch (mode) {
	case 0:
		return lowpass;
	case 1:
		return bandpass;
	case 2:
		return highpass;
	}

}

u32 FILTER_StateVariableResonance(u32 input, u16 cutoff, u32 Q, u8 mode) {

	//6 times filtering
	mode >>= 5;
	s32 bandpass, lowpass, highpass;
	s32 in;

	cutoff = (cutoff > 4095) ? cutoff : 4096;
	f = cutoff / 4;
	Q = (Q > 2) ? Q : 3;
	u16 q = (1 << FP_PRECISSION) / Q;

	in = input & 0xFFFF;

	lp1 += (f * bp1) / 65535;
	hp1 = in - lp1 - ((bp1 * q) / (1 << FP_PRECISSION));
	bp1 += (f * hp1) / 65535;
	lowpass = lp1;
	highpass = hp1;
	bandpass = bp1;

	lp1 += (f * bp1) / 65535;
	hp1 = in - lp1 - ((bp1 * q) / (1 << FP_PRECISSION));
	bp1 += (f * hp1) / 65535;
	lowpass += lp1;
	highpass += hp1;
	bandpass += bp1;

	lp1 += (f * bp1) / 65535;
	hp1 = in - lp1 - ((bp1 * q) / (1 << FP_PRECISSION));
	bp1 += (f * hp1) / 65535;
	lowpass += lp1;
	highpass += hp1;
	bandpass += bp1;

	lp1 += (f * bp1) / 65535;
	hp1 = in - lp1 - ((bp1 * q) / (1 << FP_PRECISSION));
	bp1 += (f * hp1) / 65535;
	lowpass += lp1;
	highpass += hp1;
	bandpass += bp1;

	lowpass /= 4;
	highpass /= 4;
	bandpass /= 4;

	switch (mode) {
	case 0:
		return lowpass;
	case 1:
		return bandpass;
	case 2:
		return highpass;
	}

}

u32 ENVELOPE(u8 voice, u32 time) {
	u32 on, off, out, relout;
	u8 start = 127;
	on = voices[voice].on_time;
	off = voices[voice].off_time;

	u16 attack = (u16) attack_time << 3; //shift right to get attack_time * 4096, max attack time around 5 seconds
	u16 decay = (u16) decay_time << 1;
	u16 rel = (u16) release << 3;
	if (voices[voice].e_stage == ENVELOPE_ATTACK) { //on note
		u16 lifetime = time - on;
		if (lifetime <= attack) {
			out = (u32) ((((u32) lifetime * (1 << FP_PRECISSION)) / attack) * (start)) / (1 << FP_PRECISSION);
		}
		if (lifetime > attack && lifetime <= (attack + decay)) {
			out = (((s32) ((lifetime - attack) * (1 << FP_PRECISSION)) / decay) * (s16) (sustain - start)) / (1 << FP_PRECISSION) + start;
		}
		if (lifetime > (attack + decay)) {
			out = sustain;
		}
	}
	if (voices[voice].e_stage == ENVELOPE_RELEASE) { //note off
		u16 off_lifetime = off - on;
		if (off_lifetime <= attack) {
			relout = (u32) ((((u32) off_lifetime * (1 << FP_PRECISSION)) / attack) * (start)) / (1 << FP_PRECISSION);
			out = ((((s32) ((time - off) * (1 << FP_PRECISSION)) / rel) * (s16) (0 - relout)) / (1 << FP_PRECISSION)) + relout;
		}
		if (off_lifetime > attack && off_lifetime <= (attack + decay)) {
			relout = (((s32) ((off_lifetime - attack) * (1 << FP_PRECISSION)) / decay) * (s16) (sustain - start)) / (1 << FP_PRECISSION) + start;

			out = ((((s32) ((time - off) * (1 << FP_PRECISSION)) / rel) * (s16) (0 - relout)) / (1 << FP_PRECISSION)) + relout;
		}
		if (off_lifetime > (attack + decay)) {
			relout = sustain;

			out = ((((s32) ((time - off) * (1 << FP_PRECISSION)) / rel) * (s16) (0 - relout)) / (1 << FP_PRECISSION)) + relout;
		}

		if ((time - off) > rel) {
			voices[voice].on_note = false;
			voices[voice].e_stage = ENVELOPE_OFF;
			voices[voice].note = 0xff;
		}
	}
	return out >> 1; //>>12;

}

