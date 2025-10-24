#ifndef MOTOR_ENCODER_H
#define MOTOR_ENCODER_H
#endif


#include <Arduino.h>
#include <P1AM.h>

class MotorEncoder{
    private:
        int modInput;
        int modOutput;
        int pinCw;
        int pinCcw;
        int pinEncoder;
        int pulseCount;
        bool prevState;
        int dir;
        int pinLimitSwitch;


    public:
        MotorEncoder(int mInput, int mOutput, int pCw, int pCcw, int pE, int sw): modInput(mInput), modOutput(mOutput), pinCw(pCw), pinCcw(pCcw), pinEncoder(pE), pulseCount(0), prevState(false), dir(1), pinLimitSwitch(sw) {;}

        void Home(){
            //MoveCcw();
            while (!P1.readDiscrete(modInput,pinLimitSwitch)){
                MoveCcw();//move it here casue it keeps spinning even if home'd
                Serial.print("Waiting for button press on limit switch");
                delay(100);
                //do nothing
                }
                Stop();
                ZeroPulse();
        }

        void begin(){
            //instance=this;

        }

        void MoveCw (){
            P1.writeDiscrete(true,modOutput,pinCw);
            P1.writeDiscrete(false,modOutput,pinCcw);//force the opposite direction off so the motor moves how we want
            dir=1;
        }

         void MoveCcw (){
            P1.writeDiscrete(true,modOutput,pinCcw);
            P1.writeDiscrete(false,modOutput,pinCw);//force the opposite direction off so the motor moves how we want
            dir=-1;
        }

        void Stop (){
            P1.writeDiscrete(false,modOutput,pinCcw);
            P1.writeDiscrete(false,modOutput,pinCw);
        }

        void UpdatePulse(){
            bool currentState=P1.readDiscrete(modInput,pinEncoder);
            if (currentState && !prevState){
                pulseCount+=dir;
            }
                prevState=currentState;
        }

        void ZeroPulse(){
            pulseCount=0;
        }   

        int GetPulseCount(){
            return pulseCount;
        }

        bool MoveTo(int targetPosition){
            UpdatePulse();
            if (pulseCount<targetPosition){
                MoveCw();
                //return false;
            }
            else if (pulseCount>targetPosition){
                MoveCcw();
                //return false;
            }
            else {
                Stop();
                return true;
            }
            return false;  
        }
    }
;
