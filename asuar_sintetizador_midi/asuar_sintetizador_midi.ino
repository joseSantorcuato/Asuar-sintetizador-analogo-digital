/*
*  SINTETIZADOR ASUAR, PRIMER SINTETIZADOR DIGITAL CONSTRUIDO Y PROGRAMADO EN CHILE CON LICENCIA GNU/GPL
*  Jose Luis Santorcuato Tapia
*  
*  Basado en la libreria "Mozzi" de Tim Barrass
*  Chile 2014
*/


#include <MIDI.h>
#include <MozziGuts.h>
#include <Oscil.h> // oscilador
#include <tables/triangle512_int8.h> // formas de onda, tri,swa,sin,cos,sqr
#include <tables/saw_analogue512_int8.h> 
#include <tables/cos512_int8.h> 
#include <tables/sin512_int8.h> 
#include <tables/square_analogue512_int8.h> 
#include <tables/whitenoise8192_int8.h>

#include <tables/cos512_int8.h> 

#include <EventDelay.h>
#include <mozzi_rand.h>
#include <LowPassFilter.h>

#include <Line.h> //para envolvente
#include <mozzi_midi.h>
#include <ADSR.h>
#include <Smooth.h>

#include <mozzi_fixmath.h>

const byte intensity = 255;

MIDI_CREATE_DEFAULT_INSTANCE();

#define CONTROL_RATE 64 // 

// Oscilador que se llama aForma
Oscil <512, AUDIO_RATE> aForma; 
Oscil<COS512_NUM_CELLS, AUDIO_RATE> aVibrato(COS512_DATA);

EventDelay kGainChangeDelay;
const unsigned int gainChangeMsec = 200;

EventDelay kSmoothOnOff;
const unsigned int smoothOnOffMsec = 2000;

float smoothness = 0.9975f;
Smooth <long> aSmoothGain(smoothness);
boolean smoothIsOn=true;
long target_gain = 0;


// generador de envolvente
ADSR <CONTROL_RATE,AUDIO_RATE> envelope;
 


int formas;

const int boton = 8;    
int estado = 0; 
int cuenta = 0;

const char poteUno = 0; // potes y entrada analogica
const char poteDos = 1; //
const char poteTres = 2; //
const char poteCuatro = 3; //
const char poteCinco = 4; //
const char poteSeis = 5; //

int valorPoteUno ;
int valorPoteDos ;
int valorPoteTres ;
int valorPoteCuatro ;
int valorPoteCinco ;
int valorPoteSeis ;

byte volume;
byte onda;
int a;
int d;
int s;
int r;

int at,de,sus,rea;

float lfo;
LowPassFilter lpf;


void HandleNoteOn(byte channel, byte note, byte velocity) { 
  if(velocity == 0){
    
    envelope.noteOff();
  }  
  else{
    aForma.setFreq(mtof(note)); // simple but less accurate frequency
    //aForma.setFreq_Q16n16(Q16n16_mtof(Q8n0_to_Q16n16(note))); // accurate frequency
    envelope.noteOn();
  }
}


void HandleNoteOff(byte channel, byte note, byte velocity) { 
  envelope.noteOff();
}


void setup() {
  // Initiate MIDI communications, listen to all channels (not needed with Teensy usbMIDI)
  MIDI.begin(MIDI_CHANNEL_OMNI);    

  // Connect the HandleNoteOn function to the library, so it is called upon reception of a NoteOn.
  MIDI.setHandleNoteOn(HandleNoteOn);  // Put only the name of the function
  MIDI.setHandleNoteOff(HandleNoteOff);  // Put only the name of the function

  //envelope.setADLevels(255,64);
 // envelope.setTimes(50,200,10000,200); // 10000 is so the note will sustain 10 seconds unless a noteOff comes

  //aForma.setFreq(440); // default frequency
  kGainChangeDelay.set(gainChangeMsec);
  kSmoothOnOff.set(smoothOnOffMsec);
  startMozzi(CONTROL_RATE); 

}

void updateControl(){
   if(kSmoothOnOff.ready()){
    if (smoothIsOn) {
      aSmoothGain.setSmoothness(0.f);
      smoothIsOn = false;
    }
    else{
      aSmoothGain.setSmoothness(smoothness);
      smoothIsOn = true;
    }
    kSmoothOnOff.start();
  }

  // random volume changes
  if(kGainChangeDelay.ready()){
    target_gain = (250);
    kGainChangeDelay.start();
  }
  valorPoteUno = mozziAnalogRead(poteUno); 
  valorPoteDos = mozziAnalogRead(poteDos); 
  valorPoteTres = mozziAnalogRead(poteTres); 
  valorPoteCuatro = mozziAnalogRead(poteCuatro);
  valorPoteCinco = mozziAnalogRead(poteCinco); 
  valorPoteSeis = mozziAnalogRead(poteSeis); 

  
   // volume = 200;//map(valorPoteUno, 0, 1023, 0, 255); 
    
      
    lfo = map(valorPoteUno, 0, 1023, 0, 20); 

    onda     =  map(valorPoteDos, 0,1023, 0,10); 
    at       =  map(valorPoteTres, 0, 1023, 0,1000); 
    de       =  map(valorPoteCuatro, 0, 1023, 0,1000); 
    sus      =   map(valorPoteCinco, 0, 1023, 0,1000); 
    rea      =   map(valorPoteSeis, 0, 1023, 0,5000);
    
         aVibrato.setFreq(lfo);
  
         MIDI.read();
  
          byte ataque = 255;;
          byte decaimiento= 255;
          envelope.setADLevels(ataque,decaimiento);
          int a = at;
          int d = de;
          int s = sus;
          int r = rea;
          envelope.setTimes(a,d,s,r); 
            lpf.setCutoffFreq(valorPoteCinco/5);


  
 
if(onda > 0 && onda < 2 ){
  
        aForma.setTable(SIN512_DATA);
}
if(onda > 2 && onda < 4){
  
       aForma.setTable(SAW_ANALOGUE512_DATA);
}
if(onda > 4 && onda < 6){
  
      aForma.setTable(TRIANGLE512_DATA);
}
if(onda > 6 && onda < 8){
  
      aForma.setTable(SQUARE_ANALOGUE512_DATA);
}

if(onda > 8 && onda < 10){
  
     aForma.setTable(WHITENOISE8192_DATA);
}

  envelope.update();

}

int updateAudio(){
    Q15n16 vibrato = (Q15n16) intensity * aVibrato.next();
    long asig = (long)
       aForma.phMod(vibrato)*aSmoothGain.next(target_gain)* envelope.next() ; 
       asig >>= 16;
     
  return (int) lpf.next(asig);

}


void loop() {
     audioHook(); 
} 






