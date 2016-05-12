#include <stdio.h>
#include "utilities.h"  // delay_ms()
#include "io.hpp"       // board IO
#include "stdlib.h"
#include "gpio.hpp"
#include "lpc_pwm.hpp"

typedef enum{
    start,
    lightSensor,
    temp_fan,
    motionSensor,
    dead
}state_machine_t;

//GPIO myPin23(P1_23);
//GPIO myPin1(P1_19);
//GPIO myPin2(P1_20);
//GPIO myPin3(P1_22);
//myPin1.setAsOutput();
//myPin2.setAsOutput();
//myPin3.setAsOutput();
//myPin23.setAsOutput();



int main(void)
{
    state_machine_t currentState = start;
    while(1)
    {
        delay_ms(100);

        bool switch1_pressed = SW.getSwitch(1);
        bool switch2_pressed = SW.getSwitch(2);
        bool switch3_pressed = SW.getSwitch(3);
        bool switch4_pressed = SW.getSwitch(4);
        GPIO pin28(P1_28);
        GPIO pin23(P1_23);
        GPIO pin29(P1_29);
        pin28.setAsOutput();
        pin23.setAsOutput();
        pin29.setAsOutput();
        float x = AS.getX();
        float y = AS.getY();
        float light_value = 0;
        float percent = 0;
        pin28.setLow();
        pin29.setLow();
        pin23.setLow();

        switch(currentState){
            case start:
                if(switch1_pressed){
                    printf("Current State: START. Press button 2 to go to LED\n");
                    currentState = lightSensor;
                }
                break;

            case lightSensor:
                while(switch2_pressed)
                {
                    //printf("Current State: START. Press button 1 to go to TEMP_FAN.\n");
                    light_value = LS.getRawValue();
                    percent =  (light_value / 4096)*100;
                    bool condition = true;
                    while(condition){
                        if(percent < 33.3)
                        {
                            pin28.setLow();
                            pin23.setLow();
                            //turn on the GREEN LED
                            LD.setNumber(percent);
                            pin29.setHigh();
                            condition = false;
                            //currentState = temp_fan;
                        }
                        if(percent > 33.3 && percent < 66.6){
                            pin29.setLow();
                            pin23.setLow();
                            //turn on the RED LED
                            LD.setNumber(percent);
                            pin28.setHigh();
                            condition = false;
                            //currentState = temp_fan;
                        }
                        if(percent > 66.6 && percent < 100){
                            pin29.setLow();
                            pin28.setLow();
                            //turn on the BLUE LED
                            LD.setNumber(percent);
                            pin23.setHigh();
                            condition = false;
                            //currentState = temp_fan;
                        }
                        if(switch3_pressed){
                            break;
                        }
                    }
                    if(switch3_pressed){
                        currentState = temp_fan;
                        break;
                    }
                }
                break;
            case temp_fan:
                while(switch3_pressed){
                    printf("Current State: LIGHT_SENSOR. Press button 2 to go to MOTION_SENSOR.\n");
                    light_value = LS.getRawValue();
                    percent =  (light_value / 4096)*100;
                    bool condition = true;
                    /* Use 1Khz PWM.  Each PWM shares the 1st frequency you set */
                    PWM motor1(PWM::pwm1, 1000); //frequency?

                    while(condition){
                        if(percent < 33.3){
                            //TURN ON THE GREEN LED
                            pin28.setLow();
                            pin23.setLow();
                            LD.setNumber(percent);
                            pin29.setHigh();
                            motor1.set(10);
                        }
                        if(percent > 33.3 && percent < 66.6){
                            //TURN ON THE RED LED
                            /* Set to 50% motor speed */

                            pin29.setLow();
                            pin23.setLow();
                            LD.setNumber(percent);
                            pin28.setHigh();
                            motor1.set(30);
                        }
                        if(percent > 66.6 && percent < 100){
                            //TURN ON THE BLUE LED
                            pin29.setLow();
                            pin28.setLow();
                            //turn on the BLUE LED
                            LD.setNumber(percent);
                            pin23.setHigh();
                            motor1.set(100);
                        }
                        if(switch4_pressed){
                            break;
                        }
                    }
                    if(switch4_pressed){
                        currentState = motionSensor;
                        break;
                    }
                }
                break;

            case motionSensor:
                x = ((x + 1024)/2048)*100;
                y = ((x + 1024)/2048)*100;

                if(x < 49){
                    //turn on LED strip 1 (left side)
                    pin28.setLow();
                    pin23.setLow();
                    pin29.setHigh();
                }

                else if(x > 51){
                    //turn on LED strip 2 (right side)
                    pin29.setLow();
                    pin23.setLow();
                    pin28.setHigh();
                }

                else if(y < 49){
                    //turn on LED strip 3 (bottom)
                    pin29.setLow();
                    pin28.setLow();
                    pin23.setHigh();
                }
                if(switch4_pressed){
                    currentState = dead;
                }
                break;


            case dead:
                if(switch4_pressed){
                    exit(0);
                }
                break;

            default:

                printf("State machine error");
                break;
        }
    }
}




