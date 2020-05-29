#include <ResponsiveAnalogRead.h>
#include <MIDI.h>

int midiChannel = 8; //midi chanel

//Time between control value updates sent to device.
//Longer = fewer updates when knob is turned fast because position has changed by more steps between updates, so parameter on synth actually changes faster
//Shorter = more updates when knob is turned fast because position has changed by fewer steps between updates, so parameter on synth actually changes slower
int delayTime = 15; //milliseconds

// Set pins for mux address select
int addressSelect0 = 2;
int addressSelect1 = 3;
int addressSelect2 = 4;

// set pins to receive data from muxes
int potMux1 = 15;
int slideMux = 14;

// Value and ValueLag variables for each control
// A current value and an old value are kept in order to know when a control has changed
int vcfCutoffValue;
int vcfCutoffValueLag;
int EGAReleaseValue;
int EGAReleaseValueLag;
int vcfResValue;
int vcfResValueLag;
int EGASustainValue;
int EGASustainValueLag;
int vcfOWFMValue;
int vcfOWFMValueLag;
int EGADecayValue;
int EGADecayValueLag;
int vcfKeyFollowValue;
int vcfKeyFollowValueLag;
int EGAAttackValue;
int EGAAttackValueLag;
int vcfVeloValue;
int vcfVeloValueLag;
int ChorusValue;
int ChorusValueLag;
int wheelVCFamountValue;
int wheelVCFamountValueLag;
int vcoOctValue;
int vcoOctValueLag;
int HPFValue;
int HPFValueLag;
int LFOwaveformValue;
int LFOwaveformValueLag;

//Midi cc #'s
int vcoOct = 14; //0-31: 16', 32-63: 8', 64-95: 4', 96-127: 2'
int vcoWaveForm = 15; //0-31: Saw, 32-63: Tri, 64-95: Pulse, 96-127 Saw+Tri
int vcoPulseWidth = 16; //0-127
int pwms = 17; //0-127
int vcoEGDepth = 18; //0-127
int noise = 19; //0-63: Noise Off, 64-127: Noise On
int sampler = 20; //0-63: Sampler not enabled, 64-127: Sampler Enabled
int balance = 21; //0-127

int vcfCutoff = 74; //0-127
int vcfRes = 71; //0-127
int vcfOWFM = 22; //0-127
int vcfKeyFollow = 24; //0-127
int vcfVelo = 25; //0-127 0: -50, 64: 0, 127: +50
int wheelVCFamount = 46; //0-127
int HPF = 26; //0-127
int vcfEGSel = 27; //0-63: Filter uses EGA, 64-127: Filter uses EGO
int vcfEGDepth = 23; //0-127 0: -50, 64: 0, 127: +50

int EGAAttack = 73; //0-127 VCA always uses this EG
int EGADecay = 28; //0-127
int EGASustain = 29; //0-127
int EGARelease = 72; //0-127

int EGOAttack = 30; //0-127 VCO always uses this EG
int EGODecay = 31; //0-127
int EGOSustain = 32; //0-127
int EGORelease = 33; //0-127

int VCALevel = 34; //0-127
int VCAVelo = 35; //0-127 0: -50, 64: 0, 127: +50

int LFOdestination = 36; //0-31: Off, 32-63: VCO, 64-95: VCF, 96-127: VCA
int LFOwaveform = 37; //0-24: DnSaw,25-49:UpSaw,50-74:Tri,75-99:Square,100-127:Random
int LFOfreq = 38; //0-127
int LFOdepth = 39; //0-127
int LFOdelay = 40; //0-127

int Chorus = 41; //0-42: Chorus off, 43-85: Mode 1, 86-127: Mode 2
int voiceAssign = 42; //0-42: Poly, 43-85: Dual, 86-127: Unison
int solPort = 43; //0-127
int detune = 44; //0-127
int wheelPitchBnd = 45; //0-127 mapped to 0-12
int wheelLFOdepth = 47; //0-127
int midiSplit = 48; //0-42: Off, 43-85: Upp, 86-127: Low
int sust = 64; //0-127

MIDI_CREATE_INSTANCE(HardwareSerial, Serial1, MIDI);

// Responsive Analog Read variables for each control
ResponsiveAnalogRead respvcfCutoff(potMux1, true);
ResponsiveAnalogRead respEGARelease(slideMux, true);
ResponsiveAnalogRead respvcfRes(potMux1, true);
ResponsiveAnalogRead respEGASustain(slideMux, true);
ResponsiveAnalogRead respvcfOWFM(potMux1, true);
ResponsiveAnalogRead respEGADecay(slideMux, true);
ResponsiveAnalogRead respvcfKeyFollow(potMux1, true);
ResponsiveAnalogRead respEGAAttack(slideMux, true);
ResponsiveAnalogRead respvcfVelo(potMux1, true);
ResponsiveAnalogRead respChorus(slideMux, true);
ResponsiveAnalogRead respwheelVCFamount(potMux1, true);
ResponsiveAnalogRead respvcoOct(slideMux, true);
ResponsiveAnalogRead respHPF(potMux1, true);
ResponsiveAnalogRead respLFOwaveform(slideMux, true);


void setup() {
  Serial.begin(9600); //for debugging

  MIDI.begin();

  pinMode(addressSelect0, OUTPUT);
  pinMode(addressSelect1, OUTPUT);
  pinMode(addressSelect2, OUTPUT);

}

void loop() {
  // select 74HC4051 channel 0 (of 0 to 7)
  threeBitWrite(0,0,0);

  sendMIDIData(vcfCutoff, &respvcfCutoff, &vcfCutoffValue, &vcfCutoffValueLag, 1);
  sendMIDIData(EGARelease, &respEGARelease, &EGAReleaseValue, &EGAReleaseValueLag, 1);

  // select 74HC4051 channel 1 (of 0 to 7)
  threeBitWrite(0,0,1);

  sendMIDIData(vcfRes, &respvcfRes, &vcfResValue, &vcfResValueLag, 1);
  sendMIDIData(EGASustain, &respEGASustain, &EGASustainValue, &EGASustainValueLag, 1);

  // select 74HC4051 channel 2 (of 0 to 7)
  threeBitWrite(0,1,0);

  sendMIDIData(vcfOWFM, &respvcfOWFM, &vcfOWFMValue, &vcfOWFMValueLag, 1);
  sendMIDIData(EGADecay, &respEGADecay, &EGADecayValue, &EGADecayValueLag, 1);

  // select 74HC4051 channel 3 (of 0 to 7)
  threeBitWrite(0,1,1);

  sendMIDIData(vcfKeyFollow, &respvcfKeyFollow, &vcfKeyFollowValue, &vcfKeyFollowValueLag, 1);
  sendMIDIData(EGAAttack, &respEGAAttack, &EGAAttackValue, &EGAAttackValueLag, 1);

  // select 74HC4051 channel 4 (of 0 to 7)
  threeBitWrite(1,0,0);

  sendMIDIData(vcfVelo, &respvcfVelo, &vcfVeloValue, &vcfVeloValueLag, 1);
  sendMIDIData(Chorus, &respChorus, &ChorusValue, &ChorusValueLag, 3);

  // select 74HC4051 channel 5 (of 0 to 7)
  threeBitWrite(1,0,1);

  sendMIDIData(wheelVCFamount, &respwheelVCFamount, &wheelVCFamountValue, &wheelVCFamountValueLag, 1);
  sendMIDIData(vcoOct, &respvcoOct, &vcoOctValue, &vcoOctValueLag, 4);

  // select 74HC4051 channel 6 (of 0 to 7)
  threeBitWrite(1,1,0);

  //sendMIDIData(HPF, &respHPF, &HPFValue, &HPFValueLag, 1);
  sendMIDIData(LFOwaveform, &respLFOwaveform, &LFOwaveformValue, &LFOwaveformValueLag, 5);

  delay(delayTime);
}

// set address for muxes
void threeBitWrite(byte bit1, byte bit2, byte bit3) {
  digitalWrite(addressSelect2, bit1);
  digitalWrite(addressSelect1, bit2);
  digitalWrite(addressSelect0, bit3);

  // allow 50 us for signals to stablize
  delayMicroseconds(50);
}

// check if control has changed - if yes, send midi cc message to synth
void sendMIDIData(int param, ResponsiveAnalogRead *respParam, int *paramValue, int *paramValueLag, int divs) {
  respParam->update(); // update responsive parameter from mux output
  *paramValue = respParam->getValue()>>3; // bitshift responsive parameter from 10 bits to 7 bits and assign to paramValue variable
  if (divs > 1) { // check if control is switch 
    int incsize = (128/divs); // determine size of switch incrments
    for (int i=0; i<(divs); i++){ 
      if ((*paramValue >= (i * incsize)) && (*paramValue <= ((i+1) * incsize))) { // see which increment the switch is set to
        *paramValue = ((i * incsize) + (incsize/2)); // set paramValue to value in the middle of the increment 
      }
    }
  }
  if (*paramValue != *paramValueLag) { // check value against lag value
    *paramValueLag = *paramValue; // set new lag value
    MIDI.sendControlChange(param, *paramValue, midiChannel); // send midi cc data
  }
}
