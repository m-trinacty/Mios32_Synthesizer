
#ifndef _SYNTH_H
#define _SYNTH_H

#include <stdbool.h>

#include <mios32.h>

/////////////////////////////////////////////////////////////////////////////
// Global definitions
/////////////////////////////////////////////////////////////////////////////

// number of provided waveforms
#define SYNTH_NUM_WAVEFORMS  4


/////////////////////////////////////////////////////////////////////////////
// Global Types
/////////////////////////////////////////////////////////////////////////////

#define FP_PRECISSION 16
#define POLY  5

// available waveforms
typedef enum {
  WAVEFORM_SAW,
  WAVEFORM_PULSE,
  WAVEFORM_SINE,
  WAVEFORM_FM,
  WAVEFORM_PWM
} synth_waveform_t;

typedef enum {
  ENVELOPE_OFF,
  ENVELOPE_ATTACK,
  ENVELOPE_DECAY,
  ENVELOPE_SUSTAIN,
  ENVELOPE_RELEASE
} envelope_stage;

struct voice {
	u8 note;   //note

	u8 state;   //note

	u8 velocity;   //velocity
	u8 chn;     //L/R channel position

	u8 octave;
	u32 on_time;
	u32 off_time;
	bool on_note;
	envelope_stage e_stage;

	u32 wave_acc_OSC1;
	u32 wave_it_OSC1;

	u32 wave_acc_OSC2;
	u32 wave_it_OSC2;

	u32 wave_acc_SUB;
	u32 wave_it_SUB;


} voices[POLY];

//global variables


static synth_waveform_t waveform_select[2];

static u8 OSC1_vol;

static u8 OSC2_vol;

static u8 SUB_vol;

static u8 volume = 1;
static float octave = 0;

static float octave1 = 0;

static float octave2 = 0;


static int detune_idx1 = 63;
static int detune_idx2 = 63;
//Envelope Generator
//filter
static float filter_gain = 0;


static u8 filter_envelope=0;
static u8 filter_mode;
static u16 filter_cutoff;
static u16 filter_Q = 0;


static u32 threshold;
static u8 seq_pause = 0;



static u32 attack_time = 0;
static u32 sustain = 0;
static u8 release;
static u8 decay_time;


static u8 vol;

static u8 pitchbend;


static u16 delay_buffer[48000];
static u32 delay_index;
static u8 delay_val=0;
static u8 feedback_val=0;
static u8 FM_modulate_val1=0;

static u8 PWM=64;
static u8 LFO_target=0;
static u8 LFO_on=0;
static u8 LFO_rate=0;

static u8 LFO_top=0;

/////////////////////////////////////////////////////////////////////////////
// Prototypes
/////////////////////////////////////////////////////////////////////////////

extern s32 SYNTH_Init(u32 mode);

extern s32 SYNTH_WaveformSet(u8 chn, synth_waveform_t waveform);
extern synth_waveform_t SYNTH_WaveformGet(u8 chn);

extern float SYNTH_FrequencyGet(u8 chn);
extern s32 SYNTH_FreqSet_OSC(u8 voice, float frq, int osc);

extern s32 SYNTH_VelocitySet(u8 chn, u8 velocity);
extern s32 SYNTH_VelocityGet(u8 chn);
extern void SYNTH_HandleMidi(mios32_midi_port_t port,mios32_midi_package_t midi_package);
extern void SYNTH_DetuneOSCSet(int val,int osc,int lfo);

extern void SYNTH_VolOSCSet(int val,int osc, int lfo);

extern void ENVELOPE_AttackSet(int val);

extern void ENVELOPE_DecaySet(int val);

extern void ENVELOPE_SustainSet(int val);

extern void ENVELOPE_ReleaseSet(int val);

extern void SYNTH_LFORunSet(int val);
extern void SYNTH_LFORateSet(int val);
extern void SYNTH_LFOTarget(int val);
extern void SYNTH_LFOModeSet(int val);
extern void SYNTH_LFOTopSet(int val);
extern void SYNTH_PWMSet(int val);
extern void SYNTH_FMSet(int val);
extern void SYNTH_DELAYLengthSet(int val);
extern void SYNTH_DELAYFeedbackSet(int val);

extern void SYNTH_FilterFrequencySet(u8 value,u8 lfo);

extern void SYNTH_FilterQSet(u8 value);

extern void SYNTH_FilterEnvelopeSet(int val);
extern void SYNTH_AINMenu(u32 pin, u32 pin_value);

extern void SYNTH_MasterVolumeSet(int val);



extern s32 ARP_Init(u32 mode);

extern s32 ARP_Reset(void);

extern s32 LFO_Handler(void);

/////////////////////////////////////////////////////////////////////////////
// Export global variables
/////////////////////////////////////////////////////////////////////////////


#endif /* _SYNTH_H */
