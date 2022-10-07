#include <arduino.h>
#include "motores.h"

extern Motores motor;

Motores::Motores(void)
{
  
}

void Motores::Motor_Init(int MI_AIN1,int MI_AIN2, int  MD_AIN1, int MD_AIN2)
{

  motor.MOTORD_AIN1 = MD_AIN1;
  motor.MOTORD_AIN2 = MD_AIN2;
  motor.MOTORI_AIN1 = MI_AIN1;
  motor.MOTORI_AIN2 = MI_AIN2;
  
  pinMode(MD_AIN1, OUTPUT);
  pinMode(MD_AIN2, OUTPUT);
  pinMode(MI_AIN1, OUTPUT);
  pinMode(MI_AIN2, OUTPUT);

  TCCR2A =   B00000011;  // Fast PWM MODE - OCA DISCONETED
  TCCR2B =  (TCCR2A & B11110111) ;  
  
  TIMSK2 = (TIMSK2 & B11111000) | 0x07;
}

//RANGO -1000 to 1000
void Motores::SetSpeeds(int mi,int md)
{
 motord = md/4;
 motori = mi/4 ;

 int motor_derecho = md/4;
 int motor_izquierdo = mi/4;
 
 if(motor_derecho > 0)
  {
    
     if(motor_derecho>255)
         {
            motor_derecho = 255;
         }
     motor_derecho = 255 - motor_derecho;    
     OCR2B = motor_derecho;
  }
  else if(motor_derecho < 0)
    {
      
      if(motor_derecho<-255) //LIMITE GIRO ATRAS
         {
            motor_derecho = -255;
         }
         
     motor_derecho = 255 + motor_derecho;
     OCR2B = motor_derecho;
  }
 
 
 
  if(motor_izquierdo > 0)
  { 
    if(motor_izquierdo>255)
    {
      motor_izquierdo = 255;
    }
    motor_izquierdo = 255 - motor_izquierdo;
    OCR2A = motor_izquierdo;
  }
  else if (motor_izquierdo < 0)
    {
       if(motor_izquierdo<-255)
         {
            motor_izquierdo = -255;
         }
      motor_izquierdo = 255 + motor_izquierdo;     
      OCR2A = motor_izquierdo;
  }
  
}

ISR(TIMER2_OVF_vect){
    digitalWrite(motor.MOTORI_AIN1, HIGH);
    digitalWrite(motor.MOTORI_AIN2, HIGH);
    digitalWrite(motor.MOTORD_AIN1, HIGH);
    digitalWrite(motor.MOTORD_AIN2, HIGH);
}

ISR(TIMER2_COMPA_vect){
  
 if(motor.motori > 0)
  { 
     digitalWrite(motor.MOTORI_AIN2, LOW);
  }
  else if(motor.motori < 0) 
  {
     digitalWrite(motor.MOTORI_AIN1, LOW);
  }
}
 
ISR(TIMER2_COMPB_vect){
   if(motor.motord > 0)
  { 
     digitalWrite(motor.MOTORD_AIN1, LOW);
  }
  else if(motor.motord < 0) 
  {
     digitalWrite(motor.MOTORD_AIN2, LOW);
  }
}
