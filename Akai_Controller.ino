#include <ResponsiveAnalogRead.h>
#include <MIDI.h>

int midiChannel = 8;

//Time between control value updates sent to device. 
//Longer = fewer updates when knob is turned fast because position has changed by more steps between updates, so parameter on synth actually changes faster
//Shorter = more updates when knob is turned fast because position has changed by fewer steps between updates, so parameter on synth actually changes slower
int delayTime = 20; //millisceonds

int addressSelect0 = 2;
int addressSelect1 = 3;
int addressSelect2 = 4;

int potMux1 = 15;
int slideMux = 14;

int vcfCutoffValue;
int vcfCutoffValueLag;
int EGAReleaseValue;
int EGAReleaseValueLag;
int vcfResValue;
int vcfResValueLag;
int EGASustainValue;
int EGASustainValueLag;
int vcfVeloValue;
int vcfVeloValueLag;
int ChorusValue;
int ChorusValueLag;


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
int vcfEGDepth = 23; //0-127 0: -50, 64: 0, 127: +50
int vcfKeyFollow = 24; //0-127
int vcfVelo = 25; //0-127 0: -50, 64: 0, 127: +50
int HPF = 26; //0-127
int vcfEGSel = 27; //0-63: Filter uses EGA, 64-127: Filter uses EGO

int EGAAttack = 73; //0-127 VCA always uses this EG
int EGADecay = 28; //0-127 
int EGASustain = 29; //0-127
int EGARealease = 72; //0-127 

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
int wheelVCFamount = 46; //0-127
int wheelLFOdepth = 47; //0-127
int midiSplit = 48; //0-42: Off, 43-85: Upp, 86-127: Low
int sust = 64; //0-127 

MIDI_CREATE_INSTANCE(HardwareSerial, Serial1, MIDI);

ResponsiveAnalogRead respvcfCutoff(potMux1, true);
ResponsiveAnalogRead respEGARealease(slideMux, true);
ResponsiveAnalogRead respvcfRes(potMux1, true);
ResponsiveAnalogRead respEGASustain(slideMux, true);
ResponsiveAnalogRead respvcfVelo(potMux1, true);
ResponsiveAnalogRead respChorus(slideMux, true);

void setup() {
  // array component_values = [1,2,3,4,5,6];
  
  Serial.begin(9600);

  MIDI.begin();

  pinMode(addressSelect0, OUTPUT);
  pinMode(addressSelect1, OUTPUT);
  pinMode(addressSelect2, OUTPUT);

}

void loop() {
  // for (int i=0; i<=7; i++) {
  // select 74HC4051 channel 0 (of 0 to 7)
  digitalWrite(addressSelect2, LOW);
  digitalWrite(addressSelect1, LOW);
  digitalWrite(addressSelect0, LOW);

  // allow 50 us for signals to stablize
  delayMicroseconds(50);

  // read mux outputs at pins 14, 15
  respvcfCutoff.update();
  respEGARealease.update();
  
  // write responsive mux outputs to mux output variables
  vcfCutoffValue = respvcfCutoff.getValue()>>3; // bitshift mux output variables from 10 bits to 7 bits
  if (vcfCutoffValue != vcfCutoffValueLag){
    vcfCutoffValueLag = vcfCutoffValue;
    MIDI.sendControlChange(vcfCutoff, vcfCutoffValue, midiChannel);
    delay (delayTime);
  }
      
  EGAReleaseValue = respEGARealease.getValue()>>3; // bitshift mux output variables from 10 bits to 7 bits
  if (EGAReleaseValue != EGAReleaseValueLag){
    EGAReleaseValueLag = EGAReleaseValue;
    MIDI.sendControlChange(EGARealease, EGAReleaseValue, midiChannel);
    delay (delayTime);
  }      
  

  // select 74HC4051 channel 1 (of 0 to 7)
  digitalWrite(addressSelect2, LOW);
  digitalWrite(addressSelect1, LOW);
  digitalWrite(addressSelect0, HIGH);

  // allow 50 us for signals to stablize
  delayMicroseconds(50);

  // read mux outputs at pins 14, 15
  respvcfRes.update();
  respEGASustain.update();
  
  // write responsive mux outputs to mux output variables
  vcfResValue = respvcfRes.getValue()>>3; // bitshift mux output variables from 10 bits to 7 bits
  if (vcfResValue != vcfResValueLag){
    vcfResValueLag = vcfResValue;
    MIDI.sendControlChange(vcfRes, vcfResValue, midiChannel);
    delay (delayTime);
  }  
  
  EGASustainValue = respEGASustain.getValue()>>3; // bitshift mux output variables from 10 bits to 7 bits
  if (EGASustainValue != EGASustainValueLag){
    EGASustainValueLag = EGASustainValue;
    MIDI.sendControlChange(EGASustain, EGASustainValue, midiChannel);
    delay (delayTime);
  }
    
   // select 74HC4051 channel 4 (of 0 to 7)
  digitalWrite(addressSelect2, HIGH);
  digitalWrite(addressSelect1, LOW);
  digitalWrite(addressSelect0, LOW);

  // allow 50 us for signals to stablize
  delayMicroseconds(50);

  // read mux outputs at pins 14, 15
  respvcfVelo.update();
  respChorus.update();
  
  // write responsive mux outputs to mux output variables
  vcfVeloValue = respvcfVelo.getValue()>>3; // bitshift mux output variables from 10 bits to 7 bits
  if (vcfVeloValue != vcfVeloValueLag){
    vcfVeloValueLag = vcfVeloValue;
    MIDI.sendControlChange(vcfVelo, vcfVeloValue, midiChannel);
    delay (delayTime);
  }  
  
  ChorusValue = respChorus.getValue()>>3; // bitshift mux output variables from 10 bits to 7 bits
  if (ChorusValue < 43) {
    ChorusValue = 0;
  }
  else if ((ChorusValue > 42) && (ChorusValue < 86)) {
    ChorusValue = 63;
  }
  else if (ChorusValue > 85) {
    ChorusValue = 127;
  }
  if (ChorusValue != ChorusValueLag){
    ChorusValueLag = ChorusValue;
    Serial.println (ChorusValue<<5);
    MIDI.sendControlChange(Chorus, ChorusValue, midiChannel);
    delay (delayTime);
  }
    
  /*respPotMux1.update();
  potMux1Value = respPotMux1.getValue()>>3;
  MIDI.sendControlChange(74, potMux1Value, midiChannel);
  delay (100);
  */

}
