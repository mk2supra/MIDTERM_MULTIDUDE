#include <Arduino.h>
#include <P1AM.h>
//#include <MotorEncoder.h>

enum MachineStates {
  kiln,
  grabber,
  rotater,
  convey
  
}; 

MachineStates curState = kiln;

//module variables
int modInput=1;
int modOutput=2;
//int modAnalog=3; //no analog for multidude

//INPUTS
int refswitchturntab=1;
int refswitchturnconveybelt=2;
int conveylightbarrierend=3;
int refsawblade=4;
int refsuctiontable=5;
int refoveninside=6;
int refovenoutside=7;
int refvacswitchkiln=8;
int lbkiln=9;

//OUTPUTS
int turntableCW=1;
int turntableCCW=2;
int conveyFORD=3;
int sawmotor=4;
int retractoven=5;
int expendoven=6;
int vac2oven=7;
int vac2turntable=8;
int lampkiln=9;
int compressor=10;
int vacvalve=11;
int valvelower=12;
int valvedoor=13;
int valvefeeder=14;

//variables add in later


void setup() {
  //startup P1AM Modules
  delay(1000); //just incase delay
  while(!P1.init()){
  delay(100);
  };
  Serial.begin(9600);
  delay(1000); //just in case delay
 }


bool DiscPlaced(){ //full basket at inner light barrier ready to be stored
  return !P1.readDiscrete(modInput, lbkiln);
}

bool OvenPullin(){
  return !P1.readDiscrete(modInput, refoveninside);
}

bool OvenPullout(){
  return !P1.readDiscrete(modInput, refovenoutside);
}

bool VacSwitchOven(){
  return !P1.readDiscrete(modInput, refvacswitchkiln);
}

bool VacSwitchTable(){
  return !P1.readDiscrete(modInput, refsuctiontable);
}

bool SawRef(){
  return !P1.readDiscrete(modInput, refsawblade);
}

bool turntableRef(){
  return !P1.readDiscrete(modInput, refswitchturntab);
}

bool FordConvey(){
  return !P1.readDiscrete(modInput, refswitchturnconveybelt);
}

bool LBFORD(){
  return !P1.readDiscrete(modInput, conveylightbarrierend);
}

//output functions
void ToggleCompressor(bool s){
  P1.writeDiscrete(s, modOutput, compressor);
}

void Belt(bool s){
  P1.writeDiscrete(s, modOutput, conveyFORD);
}

void Saw(bool s){
  P1.writeDiscrete(s, modOutput, sawmotor);
}

void valveTable(bool s){
  P1.writeDiscrete(s, modOutput, valvefeeder);
}

void kilnLight(bool s){
  P1.writeDiscrete(s, modOutput, lampkiln);
}

void OvenIn(bool s){
  P1.writeDiscrete(s, modOutput, retractoven);
}

void OvenOut(bool s){
  P1.writeDiscrete(s, modOutput, expendoven);
}

void OvenDoor(bool s){
  P1.writeDiscrete(s, modOutput, valvedoor);
}

void vacOven(bool s){
  P1.writeDiscrete(s, modOutput, vac2oven);
}

void vacturntable(bool s){
  P1.writeDiscrete(s, modOutput, vac2turntable);
}

void vacgrip(bool s){
  P1.writeDiscrete(s, modOutput, vacvalve);
}

void dropgripper(bool s){
  P1.writeDiscrete(s, modOutput, valvelower);
}

void turnCW(bool s){
  P1.writeDiscrete(s, modOutput, turntableCW);
}

void turnCCW(bool s){
  P1.writeDiscrete(s, modOutput, turntableCCW);
}
//should be every funciton I need I hope...

void loop() {

  switch (curState) {
    case kiln:
      // read outer light barrier, comp on, oven door true
      if (DiscPlaced()){
        ToggleCompressor(true); OvenDoor(true);
      
      delay(500);
      //next pull piece into kiln and shut door and bake it
      OvenOut(true);
      while(OvenPullout()){}
      OvenOut(false);
 
      OvenIn(true);
      while(OvenPullin()){}
      OvenIn(false);
      OvenDoor(false);
 //turn on light
      kilnLight(true);
      delay(3000);
      kilnLight(false);
 
      OvenDoor(true);
      OvenOut(true);
      while(OvenPullout()){}
      OvenOut(false);
      OvenDoor(false);  
      curState=grabber;
      } 
      break;
      case grabber:
      vacOven(true);
      while(VacSwitchOven()){}
      vacOven(false);
 
      dropgripper(true);
      delay(500);
      vacgrip(true);
      delay(500);
      dropgripper(false);
      delay(500);
 
      vacturntable(true);
      while(VacSwitchTable()){}
      vacturntable(false);
 
      curState = rotater;
      break;  
      case rotater:
      turnCCW(true);
      while(turntableRef()){}
      turnCCW(false);
 
      dropgripper(true);
      delay(500);
      vacgrip(false);
      delay(500);
      dropgripper(false);
      delay(500);
 
      turnCW(true);
      while(SawRef()){}
      turnCW(false);
 
      Saw(true);
      delay(5000);
      Saw(false);
 
      turnCW(true);
      while(FordConvey()){}
      turnCW(false);
 
      valveTable(true);
      delay(500);
      valveTable(false);
      curState = convey;
      break;
      case convey:
      Belt(true);
      if (LBFORD()){
      delay(1000);
      Belt(false);
      ToggleCompressor(false);

      //to make it run faster
      turnCCW(true);
      while(turntableRef()){}
      turnCCW(false);
      //so now we can shave a little time off the total operation.


      curState = kiln;
      }
      break;
      default:
      break;
  };
}
