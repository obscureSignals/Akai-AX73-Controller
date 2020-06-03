#include <ResponsiveAnalogRead.h>
#include <MIDI.h>

int midiChannel = 8;

//Time between control value updates sent to device.
//Longer = fewer updates when knob is turned fast because position has changed by more steps between updates, so parameter on synth actually changes faster
//Shorter = more updates when knob is turned fast because position has changed by fewer steps between updates, so parameter on synth actually changes slower
int DELAY_TIME = 25; //millisceonds
unsigned long delayStart = 0; // the time the delay started

int noiseCount = 0; //for counting how long the noise control has remained unchanged

int addressSelect0 = 14;
int addressSelect1 = 15;
int addressSelect2 = 16;

//Inputs from mux chips
int slideMux = 17;
int switchMux = 18;
int potMux1 = 19;
int potMux2 = 20;
int potMux3 = 21;

//Initialize "Value" variable for each parameter - These will be current 7-bit value for each parameter
int vcoOctValue;
int vcoWaveFormValue;
int vcoPulseWidthValue;
int pwmsValue;
int vcoEGDepthValue;
//int noiseValue;
//int samplerValue;
int balanceValue;

int vcfCutoffValue;
int vcfResValue;
int vcfOWFMValue;
int vcfKeyFollowValue;
int vcfVeloValue;
int wheelVCFamountValue;
int HPFValue;
int vcfEGSelValue;
int vcfEGDepthValue;

int EGAAttackValue;
int EGADecayValue;
int EGASustainValue;
int EGAReleaseValue;

int EGOAttackValue;
int EGODecayValue;
int EGOSustainValue;
int EGOReleaseValue;

int VCALevelValue;
int VCAVeloValue;

int LFOdestinationValue;
int LFOwaveformValue;
int LFOfreqValue;
int LFOdepthValue;
int LFOdelayValue;

int ChorusValue;
int voiceAssignValue;
int solPortValue;
int detuneValue;
int wheelPitchBndValue;
int wheelLFOdepthValue;
int midiSplitValue;
int sustValue;


//Initialize "ValueLag" variable for each paramete
//These will be previous 7-bit value for each parameter used to determine if value has changed
int vcoOctValueLag;
int vcoWaveFormValueLag;
int vcoPulseWidthValueLag;
int pwmsValueLag;
int vcoEGDepthValueLag;
//int noiseValueLag;
//int samplerValueLag;
int balanceValueLag;

int vcfCutoffValueLag;
int vcfResValueLag;
int vcfOWFMValueLag;
int vcfKeyFollowValueLag;
int vcfVeloValueLag;
int wheelVCFamountValueLag;
int HPFValueLag;
int vcfEGSelValueLag;
int vcfEGDepthValueLag;

int EGAAttackValueLag;
int EGADecayValueLag;
int EGASustainValueLag;
int EGAReleaseValueLag;

int EGOAttackValueLag;
int EGODecayValueLag;
int EGOSustainValueLag;
int EGOReleaseValueLag;

int VCALevelValueLag;
int VCAVeloValueLag;

int LFOdestinationValueLag;
int LFOwaveformValueLag;
int LFOfreqValueLag;
int LFOdepthValueLag;
int LFOdelayValueLag;

int ChorusValueLag;
int voiceAssignValueLag;
int solPortValueLag;
int detuneValueLag;
int wheelPitchBndValueLag;
int wheelLFOdepthValueLag;
int midiSplitValueLag;
int sustValueLag;

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

//Row 1
ResponsiveAnalogRead respvcoOct(potMux3, true); //Mux Address = 0
ResponsiveAnalogRead respvcoWaveForm(potMux3, true); //Mux Address = 1
ResponsiveAnalogRead respvcoPulseWidth(potMux1, true); //Mux Address = 0
ResponsiveAnalogRead resppwms(potMux1, true); //Mux Address = 1
ResponsiveAnalogRead respvcoEGDepth(potMux1, true); //Mux Address = 2
ResponsiveAnalogRead respbalance(potMux1, true); //Mux Address = 3
ResponsiveAnalogRead respdetune(potMux1, true); //Mux Address = 4
ResponsiveAnalogRead respsolPort(potMux1, true); //Mux Address = 5

//Row 2
ResponsiveAnalogRead respLFOdestination(potMux3, true); //Mux Address = 2
ResponsiveAnalogRead respLFOwaveform(potMux3, true); //Mux Address = 3
ResponsiveAnalogRead respLFOfreq(potMux1, true); //Mux Address = 6
ResponsiveAnalogRead respLFOdepth(potMux1, true); //Mux Address = 7
ResponsiveAnalogRead respLFOdelay(potMux2, true); //Mux Address = 0
ResponsiveAnalogRead respwheelLFOdepth(potMux2, true); //Mux Address = 1
ResponsiveAnalogRead respVCALevel(potMux2, true); //Mux Address = 2
ResponsiveAnalogRead respVCAVelo(potMux2, true); //Mux Address = 3

//Row 3
ResponsiveAnalogRead respvcfCutoff(potMux3, true); //Mux Address = 5
ResponsiveAnalogRead respvcfRes(potMux3, true); //Mux Address = 7
ResponsiveAnalogRead respvcfOWFM(potMux2, true); //Mux Address = 4
ResponsiveAnalogRead respvcfKeyFollow(potMux2, true); //Mux Address = 5
ResponsiveAnalogRead respvcfVelo(potMux2, true); //Mux Address = 6
ResponsiveAnalogRead respwheelVCFamount(potMux2, true); //Mux Address = 7
ResponsiveAnalogRead respHPF(potMux3, true); //Mux Address = 4
ResponsiveAnalogRead respvcfEGDepth(potMux3, true); //Mux Address = 6

ResponsiveAnalogRead respEGARelease(slideMux, true);
ResponsiveAnalogRead respEGASustain(slideMux, true);
ResponsiveAnalogRead respEGADecay(slideMux, true);
ResponsiveAnalogRead respEGAAttack(slideMux, true);
ResponsiveAnalogRead respChorus(slideMux, true);

void OnNoteOn(byte channel, byte note, byte velocity) {
  MIDI.sendNoteOn(note, velocity, channel);
}

void OnNoteOff(byte channel, byte note, byte velocity) {
  MIDI.sendNoteOff(note, velocity, channel);
}

void OnPitchChange(byte channel, int pitch) {
  MIDI.sendPitchBend(pitch, channel);
}

void setup() {
  Serial.begin(9600); //debugging

  MIDI.begin();

  pinMode(addressSelect0, OUTPUT);
  pinMode(addressSelect1, OUTPUT);
  pinMode(addressSelect2, OUTPUT);

  usbMIDI.setHandleNoteOff(OnNoteOff);
  usbMIDI.setHandleNoteOn(OnNoteOn);
  usbMIDI.setHandlePitchChange(OnPitchChange);

  delayStart = millis();

}

void loop() {

  usbMIDI.read();

  if ((millis() - delayStart) >= DELAY_TIME) { // only check control positions and send midi cc every DELAY_TIME
    delayStart += DELAY_TIME; // this prevents drift in the delays

    // select 74HC4051 channel 0 (of 0 to 7)
    threeBitWrite(0,0,0);

    sendMIDIData(vcoPulseWidth, &respvcoPulseWidth, &vcoPulseWidthValue, &vcoPulseWidthValueLag, 1); //potMux1
    sendMIDIData(LFOdelay, &respLFOdelay, &LFOdelayValue, &LFOdelayValueLag, 1); //potMux2
    sendMIDIData(vcoOct, &respvcoOct, &vcoOctValue, &vcoOctValueLag, 4); //potMux3

    // select 74HC4051 channel 1 (of 0 to 7)
    threeBitWrite(0,0,1);

    sendMIDIData(pwms, &resppwms, &pwmsValue, &pwmsValueLag, 1); //potMux1
    sendMIDIData(wheelLFOdepth, &respwheelLFOdepth, &wheelLFOdepthValue, &wheelLFOdepthValueLag, 1); //potMux2
    sendMIDIData(vcoWaveForm, &respvcoWaveForm, &vcoWaveFormValue, &vcoWaveFormValueLag, 4); //potMux3

    // select 74HC4051 channel 2 (of 0 to 7)
    threeBitWrite(0,1,0);

    sendMIDIData(vcoEGDepth, &respvcoEGDepth, &vcoEGDepthValue, &vcoEGDepthValueLag, 1); //potMux1
    sendMIDIData(VCALevel, &respVCALevel, &VCALevelValue, &VCALevelValueLag, 1); //potMux2
    sendMIDIData(LFOdestination, &respLFOdestination, &LFOdestinationValue, &LFOdestinationValueLag, 4); //potMux3

    // select 74HC4051 channel 3 (of 0 to 7)
    threeBitWrite(0,1,1);

    // noise control has special requirements
    respbalance.update(); // update responsive parameter from mux output
    balanceValue = respbalance.getValue()>>3; // bitshift responcive parameter from 10 bits to 7 bits and assign to paramValue variable
    if (balanceValue != balanceValueLag) { // check value against lag value
      balanceValueLag = balanceValue; // set new lag value
      MIDI.sendControlChange(balance, balanceValue, midiChannel); // send midi cc data
      if (noiseCount > 200){ // if, prior to this event, the noise control had not been changed for more than 200 cycles
        MIDI.sendControlChange(noise, 127, midiChannel); // turn noise on
        MIDI.sendControlChange(sampler, 0, midiChannel); // turn smapler off
        noiseCount = 0;
        }
     }
     else {
       noiseCount++;
     }

    sendMIDIData(VCAVelo, &respVCAVelo, &VCAVeloValue, &VCAVeloValueLag, 1); //potMux2
    sendMIDIData(LFOwaveform, &respLFOwaveform, &LFOwaveformValue, &LFOwaveformValueLag, 5); //potMux3

    // select 74HC4051 channel 4 (of 0 to 7)
    threeBitWrite(1,0,0);

    sendMIDIData(detune, &respdetune, &detuneValue, &detuneValueLag, 1); //potMux1
    sendMIDIData(vcfOWFM, &respvcfOWFM, &vcfOWFMValue, &vcfOWFMValueLag, 1); //potMux2
    sendMIDIData(HPF, &respHPF, &HPFValue, &HPFValueLag, 1); //potMux3

     // select 74HC4051 channel 5 (of 0 to 7)
    threeBitWrite(1,0,1);

    sendMIDIData(solPort, &respsolPort, &solPortValue, &solPortValueLag, 1); //potMux1
    sendMIDIData(vcfKeyFollow, &respvcfKeyFollow, &vcfKeyFollowValue, &vcfKeyFollowValueLag, 1); //potMux2
    sendMIDIData(vcfCutoff, &respvcfCutoff, &vcfCutoffValue, &vcfCutoffValueLag, 1); //potMux3

    // select 74HC4051 channel 6 (of 0 to 7)
    threeBitWrite(1,1,0);

    sendMIDIData(LFOfreq, &respLFOfreq, &LFOfreqValue, &LFOfreqValueLag, 1); //potMux1
    sendMIDIData(vcfVelo, &respvcfVelo, &vcfVeloValue, &vcfVeloValueLag, 1); //potMux2
    sendMIDIData(vcfEGDepth, &respvcfEGDepth, &vcfEGDepthValue, &vcfEGDepthValueLag, 1); //potMux3

    // select 74HC4051 channel 7 (of 0 to 7)
    threeBitWrite(1,1,1);

    sendMIDIData(LFOdepth, &respLFOdepth, &LFOdepthValue, &LFOdepthValueLag, 1); //potMux1
    sendMIDIData(wheelVCFamount, &respwheelVCFamount, &wheelVCFamountValue, &wheelVCFamountValueLag, 1); //potMux2
    sendMIDIData(vcfRes, &respvcfRes, &vcfResValue, &vcfResValueLag, 1); //potMux3

    //sendMIDIData(EGARelease, &respEGARelease, &EGAReleaseValue, &EGAReleaseValueLag, 1);
    //sendMIDIData(EGASustain, &respEGASustain, &EGASustainValue, &EGASustainValueLag, 1);
    //sendMIDIData(Chorus, &respChorus, &ChorusValue, &ChorusValueLag, 3);
    //sendMIDIData(EGAAttack, &respEGAAttack, &EGAAttackValue, &EGAAttackValueLag, 1);
    //sendMIDIData(EGADecay, &respEGADecay, &EGADecayValue, &EGADecayValueLag, 1);
  }
}
void threeBitWrite(byte bit1, byte bit2, byte bit3) {
  digitalWrite(addressSelect2, bit1);
  digitalWrite(addressSelect1, bit2);
  digitalWrite(addressSelect0, bit3);

  // allow 50 us for signals to stablize
  delayMicroseconds(50);
}

void sendMIDIData(int param, ResponsiveAnalogRead *respParam, int *paramValue, int *paramValueLag, int divs) {
  respParam->update(); // update responsive parameter from mux output
  *paramValue = respParam->getValue()>>3; // bitshift responcive parameter from 10 bits to 7 bits and assign to paramValue variable
  if (divs > 1) { // check if control is switch
    int incsize = (128/divs); // determine size of switch incrments
    for (int i=0; i<(divs); i++){ // see which increment the switch is set to
      if ((*paramValue >= (i * incsize)) && (*paramValue <= ((i+1) * incsize))) {
        *paramValue = ((i * incsize) + (incsize/2)); // set paramValue to value in the middle of the increment
      }
    }
  }
  if (*paramValue != *paramValueLag) { // check value against lag value
    *paramValueLag = *paramValue; // set new lag value
    MIDI.sendControlChange(param, *paramValue, midiChannel); // send midi cc data
  }
}
