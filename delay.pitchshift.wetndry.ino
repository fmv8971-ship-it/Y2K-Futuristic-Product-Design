#include <Audio.h>
#include <Wire.h>
#include <SPI.h>
#include <SD.h>
#include <SerialFlash.h>

#define GRAIN_BUFFER (AUDIO_BLOCK_SAMPLES * 8)
#define CHORUS_BUFFER (AUDIO_BLOCK_SAMPLES * 8)

// GUItool: begin automatically generated code
AudioInputI2S i2s1;                 //xy=1011.2500152587891,935.0000143051147
AudioEffectGranular granular1;      //xy=1035.000015258789,1140.0000171661377
AudioEffectChorus chorus1;          //xy=1207.500015258789,1090.000015258789
AudioMixer4 mixer2;                 //xy=1350.2500190734863,734.7500114440918
AudioEffectDelay delay1;            //xy=1364.2500190734863,1024.250015258789
AudioMixer4 mixer1;                 //xy=1494.5000228881836,1009.2500152587891
AudioEffectBitcrusher bitcrusher1;  //xy=1602.500015258789,1195.0000143051147
AudioMixer4 mixer3;                 //xy=1675.0000228881836,943.7500152587891
AudioOutputI2S headphones;          //xy=1781.7500267028809,1071.5000162124634
AudioConnection patchCord1(i2s1, 0, mixer2, 0);
AudioConnection patchCord2(i2s1, 0, mixer3, 1);
AudioConnection patchCord3(i2s1, 1, granular1, 0);
AudioConnection patchCord4(granular1, chorus1);
AudioConnection patchCord5(chorus1, bitcrusher1);
AudioConnection patchCord6(mixer2, delay1);
AudioConnection patchCord7(delay1, 0, mixer1, 0);
AudioConnection patchCord8(delay1, 1, mixer1, 1);
AudioConnection patchCord9(delay1, 2, mixer1, 2);
AudioConnection patchCord10(mixer1, 0, mixer2, 1);
AudioConnection patchCord11(mixer1, 0, mixer3, 0);
AudioConnection patchCord12(bitcrusher1, 0, mixer3, 2);
AudioConnection patchCord13(mixer3, 0, headphones, 0);
AudioConnection patchCord14(mixer3, 0, headphones, 1);
AudioControlSGTL5000 sgtl5000_1;  //xy=1347.7500228881836,1281.7500190734863
// GUItool: end automatically generated code

const int LineIn = AUDIO_INPUT_LINEIN;

int16_t grainBuffer[GRAIN_BUFFER];
int16_t chorusBuffer[CHORUS_BUFFER];

int chorusPotPin = A13;
int bitcrusherPotPin = A14;

int switchPin = 24;
//supposed to be a switchPIn
int pitchSwitchPin = 25;

int bitcrushingAmount = 0;
int voiceAmount = 0;

int delayPot = A17;
int blendPot = A16;
int fbPot = A15;
float blendVal = 0;

void setup() {
  Serial.begin(9600);

  AudioMemory(1000);
  sgtl5000_1.enable();
  sgtl5000_1.volume(0.5);
  sgtl5000_1.inputSelect(LineIn);
  sgtl5000_1.inputLevel(2.0);
  granular1.begin(grainBuffer, GRAIN_BUFFER);
  chorus1.begin(chorusBuffer, CHORUS_BUFFER, 4);

  pinMode(delayPot, INPUT);
  pinMode(fbPot, INPUT);
  pinMode(blendPot, INPUT);
  pinMode(switchPin, INPUT);
  pinMode(pitchSwitchPin, INPUT);
}

float multi = 1.4252;
float delayAmount = 0;
float fb = 0;

void loop() {
  fb = float(analogRead(fbPot)) / 6000.0f;
  if (fb < 0.04f) fb = 0;
  delayAmount = analogRead(delayPot) * multi;

  blendVal = map(float(analogRead(blendPot)), 0, 1023, 0, 1);
  Serial.println(blendVal);
  delay1.delay(0, delayAmount * multi);
  delay1.delay(1, 0);
  delay1.delay(2, 0);
  delay1.disable(3);
  delay1.disable(4);
  delay1.disable(5);
  delay1.disable(6);
  delay1.disable(7);
  mixer2.gain(1, fb);
  mixer3.gain(0, blendVal);
  mixer3.gain(1, 1 - blendVal);

  delay(50);

  //pitch shift code
  octaveSwitch();
  mixer3.gain(1, !digitalRead(24)); // Momentary button reading pin 24 to switch between mixer channels
  mixer3.gain(2, digitalRead(24));
  bitcrushingAmount = map(analogRead(bitcrusherPotPin), 0, 1023, 1, 12); // Mapping bitcrusher onto pot
  voiceAmount = map(analogRead(chorusPotPin), 0, 1023, 1, 4); // Mapping chorus onto pot
  bitcrusher1.bits(bitcrushingAmount);
  chorus1.voices(voiceAmount * 2);
  delay(10);
  Serial.println(bitcrushingAmount);
}

void octaveSwitch() {
  if (digitalRead(pitchSwitchPin) == HIGH) {
    granular1.setSpeed(4.0);  // Two octaves up
    granular1.beginPitchShift(20);
  } else {
    granular1.setSpeed(2.0);  // One octave up
    granular1.beginPitchShift(20);
  }
}