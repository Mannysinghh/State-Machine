#include <stdio.h>
#include "utilities.h"  // delay_ms()
#include "io.hpp"       // board IO
#include "stdlib.h"
#include "gpio.hpp"
#include "lpc_pwm.hpp"

typedef enum{
    start,
    lightSensor,
    lightMotor,
    motionSensor,
    dead
}state_machine_t;
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
        float x = 0;
        float y = 0;
        float light_value = 0;
        float percent = 0;
        pin28.setLow();
        pin29.setLow();
        pin23.setLow();

        switch(currentState){
            case start:
                while(switch1_pressed){
                    PWM motor1(PWM::pwm1, 1000);
                    bool condition = true;
                    //printf("Current State: START. Press button 2 to go to LED\n");
                    bool switch1_pressed = SW.getSwitch(1);
                    bool switch2_pressed = SW.getSwitch(2);
                    while(condition){
                        motor1.set(0);
                        pin28.setHigh();
                        delay_ms(100);
                        pin28.setLow();
                        delay_ms(100);
                        pin29.setHigh();
                        delay_ms(100);
                        pin29.setLow();
                        delay_ms(100);
                        pin23.setHigh();
                        delay_ms(100);
                        pin23.setLow();
                        delay_ms(100);
                        condition = false;
                        if(switch1_pressed){
                            break;
                        }
                    }
                    if(switch2_pressed){
                        currentState = lightSensor;
                        break;
                    }
                }
                break;

            case lightSensor:
                while(switch2_pressed)
                {
                    bool switch1_pressed = SW.getSwitch(1);
                    bool switch2_pressed = SW.getSwitch(2);
                    bool switch3_pressed = SW.getSwitch(3);
                    bool switch4_pressed = SW.getSwitch(4);
                    //printf("Current State: START. Press button 1 to go to TEMP_FAN.\n");
                    light_value = LS.getRawValue();
                    percent =  (light_value / 4096)*100;
                    bool condition = true;
                    PWM motor1(PWM::pwm1, 1000);
                    while(condition){
                        if(percent < 33.3)
                        {
                            pin28.setLow();
                            pin23.setLow();
                            //turn on the GREEN LED
                            LD.setNumber(percent);
                            pin29.setHigh();
                            motor1.set(0);
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
                            motor1.set(0);
                            //currentState = temp_fan;
                        }
                        if(percent > 66.6 && percent < 100){
                            pin29.setLow();
                            pin28.setLow();
                            //turn on the BLUE LED
                            LD.setNumber(percent);
                            pin23.setHigh();
                            condition = false;
                            motor1.set(0);
                            //currentState = temp_fan;
                        }
                        if(switch3_pressed){
                            break;
                        }
                    }
                    if(switch3_pressed){
                        currentState = lightMotor;
                        break;
                    }
                }
                break;
            case lightMotor:
                while(switch3_pressed){
                    bool switch1_pressed = SW.getSwitch(1);
                    bool switch2_pressed = SW.getSwitch(2);
                    bool switch3_pressed = SW.getSwitch(3);
                    bool switch4_pressed = SW.getSwitch(4);
                    //printf("Current State: LightSensor. Press button 1 to go to TEMP_FAN.\n");
                    light_value = LS.getRawValue();
                    percent =  (light_value / 4096)*100;
                    bool condition = true;
                    PWM motor1(PWM::pwm1, 1000);
                    while(condition){
                        if(percent < 33.3)
                        {
                            pin28.setLow();
                            pin23.setLow();
                            //turn on the GREEN LED
                            LD.setNumber(percent);
                            pin29.setHigh();
                            motor1.set(10);
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
                            motor1.set(30);
                            //currentState = temp_fan;
                        }
                        if(percent > 66.6 && percent < 100){
                            pin29.setLow();
                            pin28.setLow();
                            //turn on the BLUE LED
                            LD.setNumber(percent);
                            pin23.setHigh();
                            condition = false;
                            motor1.set(100);
                            //currentState = temp_fan;
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
                while(switch4_pressed){
                    x = AS.getX();
                    y = AS.getY();
                    percent = ((x + 1024)/2048)*100;
                    //y = ((x + 1024)/2048)*100;
                    bool condition = true;

                    PWM motor1(PWM::pwm1, 1000);
                    motor1.set(0);
                    while(condition){
                        if(percent < 33.3){
                            //turn on LED strip 1 (left side)
                            pin28.setLow();
                            pin23.setLow();
                            pin29.setHigh();
                            motor1.set(0);
                            condition = false;
                        }
                        if(percent > 33.3 && percent < 66.6){
                            //turn on LED strip 2 (right side)
                            pin29.setLow();
                            pin23.setLow();
                            pin28.setHigh();
                            motor1.set(30);
                            condition = false;
                        }
                        if(percent < 100 && percent > 66.6){
                            pin29.setLow();
                            pin28.setLow();
                            pin23.setHigh();
                            motor1.set(100);
                            condition = false;
                        }
                    }
                    break;
                }

            case dead:
                if(switch1_pressed){
                    exit(0);
                }
                break;


            default:

                printf("State machine error");
                break;
        }
    }
}


