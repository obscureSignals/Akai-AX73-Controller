#include <ResponsiveAnalogRead.h>
#include <MIDIMod.h>

//Incoming midi variables
byte inType;
int inPitchBend;
byte lastByte;
byte sysExArray[5205];

int midiChannel = 8; //Channel 8 is factory setting on AX73

//Time between control value updates sent to device.
//Longer = fewer updates sent to synth when knob is turned fast because position has changed by more steps between updates,
//so parameter on synth actually changes faster because synth is processing fewer requests
//Shorter = more updates when knob is turned fast because position has changed by fewer steps between updates, 
//so parameter on synth actually changes slower, because synth is processing more updates
int DELAY_TIME = 25; //millisceonds
unsigned long delayStart = 0; // the time the delay started

int noiseCount = 0; //for counting how long the noise control has remained unchanged

int addressSelect0 = 14; //pin for mux address 0
int addressSelect1 = 15; //pin for mux address 1
int addressSelect2 = 16; //pin for mux address 2

//inputs pins for signals from mux chips
int slideMux = 17;
int switchMux = 18;
int potMux1 = 19;
int potMux2 = 20;
int potMux3 = 21;

// Initialize "Value" variable for each parameter - These will be the current, 7-bit values for each parameter.
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


// Initialize "ValueLag" variable for each paramete
// These will be previous 7-bit value for each parameter used to determine if control position has changed
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

// Midi cc #'s
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
//wheelPitchBnd values
//0-9 = 0
//10-19 = 1
//20-29 = 2
//30-39 = 3
//40-49 = 4
//50-59 = 5
//60-68 = 6
//69-78 = 7
//79-88= 8
//89-98 = 9
//99-108 = 10
//109-118 = 11
//119-127 = 12

int wheelLFOdepth = 47; //0-127
int midiSplit = 48; //0-42: Off, 43-85: Upp, 86-127: Low
int sust = 64; //0-127

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

//Sliders
ResponsiveAnalogRead respEGAAttack(slideMux, true); //Mux Address = 0
ResponsiveAnalogRead respEGADecay(slideMux, true); //Mux Address = 1
ResponsiveAnalogRead respEGASustain(slideMux, true); //Mux Address = 2
ResponsiveAnalogRead respEGARelease(slideMux, true); //Mux Address = 3
ResponsiveAnalogRead respEGOAttack(slideMux, true); //Mux Address = 4
ResponsiveAnalogRead respEGODecay(slideMux, true); //Mux Address = 5
ResponsiveAnalogRead respEGOSustain(slideMux, true); //Mux Address = 6
ResponsiveAnalogRead respEGORelease(slideMux, true); //Mux Address = 7

//Gobal/Switches
ResponsiveAnalogRead respwheelPitchBnd(switchMux, true); //Mux Address = 0
ResponsiveAnalogRead respChorus(switchMux, true); //Mux Address = 1
ResponsiveAnalogRead respvoiceAssign(switchMux, true); //Mux Address = 2
ResponsiveAnalogRead respvcfEGSel(switchMux, true); //Mux Address = 3


MIDI_CREATE_INSTANCE(HardwareSerial, Serial1, MIDI);

// When note on message is received from USB, send same message out to synth.
void OnNoteOn(byte channel, byte note, byte velocity) { 
  MIDI.sendNoteOn(note, velocity, channel);
}

// When note off message is received from USB, send same message out to synth.
void OnNoteOff(byte channel, byte note, byte velocity) {
  MIDI.sendNoteOff(note, velocity, channel);
}

// When pitch change message is received from USB, send same message out to synth.
void OnPitchChange(byte channel, int pitch) {
  MIDI.sendPitchBend(pitch, channel);
}

// When SysEx message is received from USB, send same message out to synth.
void OnSystemExclusive(const byte *data, uint16_t length, bool last) {
  MIDI.sendSysEx (length, data, true); // Do not add in stop and start bytes reggardless of what USB midi message reports with 'last'.
}

// When control change message is received from USB, send same message out to synth.
void OnControlChange(byte channel, byte control, byte value) { 
  MIDI.sendControlChange(control, value, channel);
}

void setup() {
  Serial.begin(9600); //debugging
  Serial1.begin(31250); //MIDI serial
   
  MIDI_CREATE_INSTANCE(HardwareSerial, Serial1, MIDI);

  MIDI.begin(MIDI_CHANNEL_OMNI);
  
  pinMode(addressSelect0, OUTPUT);
  pinMode(addressSelect1, OUTPUT);
  pinMode(addressSelect2, OUTPUT);

  usbMIDI.setHandleNoteOff(OnNoteOff);
  usbMIDI.setHandleNoteOn(OnNoteOn);
  usbMIDI.setHandlePitchChange(OnPitchChange);
  usbMIDI.setHandleSystemExclusive(OnSystemExclusive);
  usbMIDI.setHandleControlChange(OnControlChange);   

  delayStart = millis();

  MIDI.sendTuneRequest(); //tune on startup

}

void loop() {

  usbMIDI.read();


  MIDI.turnThruOff(); // Turn off midi through. Stops notes from AX73 keyboard looping back and playing twice.

  // Echo incoming MIDI messages out over USB.
  if (Serial1.available() > 0){
    lastByte = Serial1.peek();
    if (lastByte == 240){ // Watch for MIDI SysEx Byte
      Serial.println ("SysEx Arrived");
      //This section is neccesary because MIDI.read does not recognize the SysEx messages generated by the AX73 as MIDI messages.
      int i = 0;
      while (i < 5205){ // Build SysEx message array.
        if (Serial1.available() > 0){
          sysExArray[i] = Serial1.read();
          i++;
        }
      }
      for (int i = 0; i < 5205; i++){ // THe sysex dump will not work properly without this section. I have no idea why, for now.
        Serial.print (i);
        Serial.print (" ");
        Serial.println (sysExArray [i]); 
      }
      usbMIDI.sendSysEx (5205, sysExArray, true); // Send SysEx message.
    }  
    if (MIDI.read()) {
    Serial.println ("MIDI Read!");  
    inType = MIDI.getType();
      switch (inType) {
      case midi::NoteOn: 
        usbMIDI.sendNoteOn (MIDI.getData1(), MIDI.getData2(), MIDI.getChannel());
        break;
      case midi::NoteOff:
        usbMIDI.sendNoteOff (MIDI.getData1(), MIDI.getData2(), MIDI.getChannel()); 
        break;
      case midi::ControlChange:
        usbMIDI.sendControlChange (MIDI.getData1(), MIDI.getData2(), MIDI.getChannel()); 
        break;
      case midi::PitchBend: 
        // The AX73 only sends 7 bit pitch bend data - the first byte of the MIDI message is empty. 
        // To get the full resolution out of a controller that is sending 14 bit pitchbend data, this code would need to be modified. 
        inPitchBend = (MIDI.getData2() - 64) << 7; // Conversion from unsigned 7 bit pitch bend value to signed 14 bit value
        usbMIDI.sendPitchBend (inPitchBend, MIDI.getChannel());
        break;
      default:
        break;
      }
    }
  }
  if ((millis() - delayStart) >= DELAY_TIME) { // only check control positions and send midi cc every DELAY_TIME
    delayStart += DELAY_TIME; // this prevents drift in the delays

    threeBitWrite(0, 0, 0); // select 74HCT4051 channel 0 (of 0 to 7)
    
    sendMIDIData(vcoPulseWidth, &respvcoPulseWidth, &vcoPulseWidthValue, &vcoPulseWidthValueLag, 1, 0); //potMux1
    sendMIDIData(LFOdelay, &respLFOdelay, &LFOdelayValue, &LFOdelayValueLag, 1, 0); //potMux2
    sendMIDIData(vcoOct, &respvcoOct, &vcoOctValue, &vcoOctValueLag, 4, 0); //potMux3
    sendMIDIData(EGAAttack, &respEGAAttack, &EGAAttackValue, &EGAAttackValueLag, 1, 0); //slideMux
    sendMIDIData(wheelPitchBnd, &respwheelPitchBnd, &wheelPitchBndValue, &wheelPitchBndValueLag, 13, 0); //switchMux
         
    threeBitWrite(0, 0, 1); // select 74HCT4051 channel 1 (of 0 to 7)

    sendMIDIData(pwms, &resppwms, &pwmsValue, &pwmsValueLag, 1, 0); //potMux1
    sendMIDIData(wheelLFOdepth, &respwheelLFOdepth, &wheelLFOdepthValue, &wheelLFOdepthValueLag, 1, 0); //potMux2
    sendMIDIData(vcoWaveForm, &respvcoWaveForm, &vcoWaveFormValue, &vcoWaveFormValueLag, 4, 0); //potMux3
    sendMIDIData(EGADecay, &respEGADecay, &EGADecayValue, &EGADecayValueLag, 1, 0); //slideMux
    sendMIDIData(Chorus, &respChorus, &ChorusValue, &ChorusValueLag, 3, 0); //switchMux

    threeBitWrite(0, 1, 0); // select 74HCT4051 channel 2 (of 0 to 7)

    sendMIDIData(vcoEGDepth, &respvcoEGDepth, &vcoEGDepthValue, &vcoEGDepthValueLag, 1, 0); //potMux1
    sendMIDIData(VCALevel, &respVCALevel, &VCALevelValue, &VCALevelValueLag, 1, 0); //potMux2
    sendMIDIData(LFOdestination, &respLFOdestination, &LFOdestinationValue, &LFOdestinationValueLag, 4, 0); //potMux3
    sendMIDIData(EGASustain, &respEGASustain, &EGASustainValue, &EGASustainValueLag, 1, 0); //slideMux
    sendMIDIData(voiceAssign, &respvoiceAssign, &voiceAssignValue, &voiceAssignValueLag, 3, 0); //switchMux
    
    threeBitWrite(0, 1, 1); // select 74HCT4051 channel 3 (of 0 to 7)

    // noise control has special requirements
    // The AX73 was originally set up to interface with Akai's S-612, S-900 and S-950 samplers.
    // In order to accomplish this, there is an "a-b" ballance control. Source "a" is the VCOs and source "b" is a sampler and/or noise.
    // Since most users do not own the sampler, I have configured the "noise" control to send two additonal messages to the synth: "turn noise on" and "turn sampler off".
    // This way, whenever the noise control is adjusted, you are sure to mix in noise and not a sampler that is not connected.
    respbalance.update(); // update responsive parameter from mux output
    balanceValue = respbalance.getValue() >> 3; // bitshift responsive parameter from 10 bits to 7 bits and assign to paramValue variable
    if (balanceValue != balanceValueLag) { // check value against lag value
      balanceValueLag = balanceValue; // set new lag value
      MIDI.sendControlChange(balance, (127 - balanceValue), midiChannel); // send inverted midi cc data so on control synth is counterclockwise and noise is clockwise
      if (noiseCount > 200) { // if, prior to this event, the noise control had not been changed for more than 200 cycles
        MIDI.sendControlChange(noise, 127, midiChannel); // turn noise on
        MIDI.sendControlChange(sampler, 0, midiChannel); // turn smapler off
        noiseCount = 0;
      }
    }
    else {
      noiseCount++;
    }

    sendMIDIData(VCAVelo, &respVCAVelo, &VCAVeloValue, &VCAVeloValueLag, 1, 0); //potMux2
    sendMIDIData(LFOwaveform, &respLFOwaveform, &LFOwaveformValue, &LFOwaveformValueLag, 5, 0); //potMux3
    sendMIDIData(EGARelease, &respEGARelease, &EGAReleaseValue, &EGAReleaseValueLag, 1, 0); //slideMux
    sendMIDIData(vcfEGSel, &respvcfEGSel, &vcfEGSelValue, &vcfEGSelValueLag, 2, 1); //switchMux

    threeBitWrite(1, 0, 0); // select 74HCT4051 channel 4 (of 0 to 7)

    sendMIDIData(detune, &respdetune, &detuneValue, &detuneValueLag, 1, 0); //potMux1
    sendMIDIData(vcfOWFM, &respvcfOWFM, &vcfOWFMValue, &vcfOWFMValueLag, 1, 0); //potMux2
    sendMIDIData(HPF, &respHPF, &HPFValue, &HPFValueLag, 1, 0); //potMux3
    sendMIDIData(EGOAttack, &respEGOAttack, &EGOAttackValue, &EGOAttackValueLag, 1, 0); //slideMux

    threeBitWrite(1, 0, 1); // select 74HCT4051 channel 5 (of 0 to 7)

    sendMIDIData(solPort, &respsolPort, &solPortValue, &solPortValueLag, 1, 0); //potMux1
    sendMIDIData(vcfKeyFollow, &respvcfKeyFollow, &vcfKeyFollowValue, &vcfKeyFollowValueLag, 1, 0); //potMux2
    sendMIDIData(vcfCutoff, &respvcfCutoff, &vcfCutoffValue, &vcfCutoffValueLag, 1, 0); //potMux3
    sendMIDIData(EGODecay, &respEGODecay, &EGODecayValue, &EGODecayValueLag, 1, 0); //slideMux

    threeBitWrite(1, 1, 0); // select 74HCT4051 channel 6 (of 0 to 7)

    sendMIDIData(LFOfreq, &respLFOfreq, &LFOfreqValue, &LFOfreqValueLag, 1, 0); //potMux1
    sendMIDIData(vcfVelo, &respvcfVelo, &vcfVeloValue, &vcfVeloValueLag, 1, 0); //potMux2
    sendMIDIData(vcfEGDepth, &respvcfEGDepth, &vcfEGDepthValue, &vcfEGDepthValueLag, 1, 0); //potMux3
    sendMIDIData(EGOSustain, &respEGOSustain, &EGOSustainValue, &EGOSustainValueLag, 1, 0); //slideMux

    
    threeBitWrite(1, 1, 1); // select 74HCT4051 channel 7 (of 0 to 7)

    sendMIDIData(LFOdepth, &respLFOdepth, &LFOdepthValue, &LFOdepthValueLag, 1, 0); //potMux1
    sendMIDIData(wheelVCFamount, &respwheelVCFamount, &wheelVCFamountValue, &wheelVCFamountValueLag, 1, 0); //potMux2
    sendMIDIData(vcfRes, &respvcfRes, &vcfResValue, &vcfResValueLag, 1, 0); //potMux3
    sendMIDIData(EGORelease, &respEGORelease, &EGOReleaseValue, &EGOReleaseValueLag, 1, 0); //slideMux
  }
}

// write address to output pins connected to Mux
void threeBitWrite(byte bit1, byte bit2, byte bit3) {
  digitalWrite(addressSelect2, bit1);
  digitalWrite(addressSelect1, bit2);
  digitalWrite(addressSelect0, bit3);

  // allow 50 us for signals to stablize
  delayMicroseconds(50);
}


// Check to see if control positon has changed. If yes, send updated midi cc value.
// divs is the number of divisions if control is to be treated as a switch. For a four-position switch, divs = 4. For a continuous control, divs = 0
// invert is to invert the control (127 = 0 aand 0 = 127). invert = 1 for invert. invert = 0 for non-inverted.
void sendMIDIData(int param, ResponsiveAnalogRead *respParam, int *paramValue, int *paramValueLag, float divs, int invert) {
  respParam->update(); // update responsive parameter from mux output
  float raw = respParam->getValue(); // pull in raw control value from adc
  raw = raw/1023; // convert raw value to fraction of full scale
  // This is a correction for loading of the controls. Controls are ~100K and and are loaded by ~100k, so loading is significant.
  float correct = -1*((-1*raw+1-sqrt(5*sq(raw)-2*raw+1))/(2*raw));    
  correct = correct*1023; //convert back into 10 bit format
  int corrected = correct; //convert to int so we can bitshift
  *paramValue = corrected >> 3; // bitshift corrected value from 10 bits to 7 bits and assign to paramValue variable
  if (divs > 1) { // check if control is switch
    float incsize = (127 / (divs-1)); // determine size of switch increments
    for (int i = 0; i < (divs); i++) { // see which increment the switch is set to
      if ((*paramValue >= ((i * incsize) - (incsize / 2))) && (*paramValue < ((i + 1) * incsize)-(incsize / 2))) {
        *paramValue = ((i * (127/divs))+(127/(2*divs))); // set paramValue to value in the middle of the increment
      }
    }
  }
  if (invert == 1) { // if invert is on
    *paramValue = (127 - *paramValue); // invert paramValue
  }
  if (*paramValue != *paramValueLag) { // check value against lag value
    *paramValueLag = *paramValue; // set new lag value
    MIDI.sendControlChange(param, *paramValue, midiChannel); // send midi cc data
  }
}
