#include "linea.h"
#include <arduino.h>
#include "rachvel.h"

extern rachvel Rachvel;
int minimo_general = 1024;
int maximo_general = 0;
   
unsigned char pins[NUM_SENSORS] = {A0,A1,A2,A3,A4,A5,A6,A7}; // SENSORES DEL 0 AL 8 QTR8
int pesos[NUM_SENSORS*2+2] = {-60,-60,-60,-60,-60,-35,-25,-15,-5,5,15,25,35,60,60,60,60,60}; // SENSORES DEL 0 AL 8 QTR8

unsigned int sensorValues_max[NUM_SENSORS];
unsigned int sensorValues_min[NUM_SENSORS];
//unsigned int sensorValues_muestras[NUM_SENSORS][NUM_MUESTRAS];
int num_muestras = 0;
int sensores_b;
int negro[8], blanco[8];
int num_negro[8], num_blanco[8];

    
void Reset_Calibracion(void)
{
   for(int x=0; x<NUM_SENSORS; x++)
   {
     sensorValues_max[x] = 0; 
     sensorValues_min[x] = MAX_VALUE;

     negro[x] = 0;
     blanco[x]= 0;
     num_negro[x]= 0;
     num_blanco[x]= 0;
     num_muestras = 0;
   }
}


int Calibrar_Color_Linea(unsigned int* sensorValuesp)
{  
   int color_fondo = 4;

   Calibrar_Sensores (sensorValuesp);

    for(int x=0; x<NUM_SENSORS; x++)
    {
      if(minimo_general > sensorValues_min[x])
          minimo_general = sensorValues_min[x];

      if(maximo_general < sensorValues_max[x])
          maximo_general = sensorValues_max[x];
    }
   
   int rango_comparacion = (maximo_general + minimo_general)/2; //RANGO TEMPORAL
   
 while(color_fondo == 4)
 {
     Leer_sensores (sensorValuesp);
     delay(500);
     Leer_sensores (sensorValuesp);
     
    //fondo NEGRO - LINEA BLANCA
    if(sensorValuesp[0] > rango_comparacion && sensorValuesp[NUM_SENSORS-1] > rango_comparacion  )
    {
      color_fondo = 1;
    }
     //fondo BLANCO - LINEA NEGRA 
    if(sensorValuesp[0] < rango_comparacion && sensorValuesp[NUM_SENSORS-1] < rango_comparacion  )
    {
      color_fondo = 0;
    }
 }
     
return color_fondo;
}


void Calibrar_Sensores(unsigned int* sensorValuesp)
{   

    Leer_sensores (sensorValuesp);
 
    for(int x=0; x<NUM_SENSORS; x++)
     {
         if(num_muestras == NUM_MUESTRAS/2)
         {
             Rachvel.discriminate[x] = (sensorValues_max[x] + sensorValues_min[x])/2; //DISCRIMINANTE TEMPORAL
         }

         if(num_muestras > NUM_MUESTRAS/2)
         {
           if(sensorValuesp[x] < Rachvel.discriminate[x])
             {
                    blanco[x] += sensorValuesp[x];
                    num_blanco[x] ++;
             }
             if(sensorValuesp[x]> Rachvel.discriminate[x])
             {
                    negro[x] += sensorValuesp[x];
                    num_negro[x] ++;
             }
          }

          if(num_muestras == NUM_MUESTRAS -1 )
          {
              blanco[x] =  blanco[x]/num_blanco[x]; //MEDIA BLANCOS
              negro[x]  =  negro[x]/num_negro[x];   //MEDIA NEGROS

              Rachvel.discriminate[x] = (blanco[x] + negro[x])/2; //DISCRIMINANTE FINAL
          }
       
       if(sensorValuesp[x] > sensorValues_max[x])
       {
          sensorValues_max[x] = sensorValuesp[x];
       }
       
       if(sensorValuesp[x] < sensorValues_min[x])
       {
          sensorValues_min[x] = sensorValuesp[x];
       }
       
     }

    num_muestras ++;
}

 
int Leer_linea(unsigned int* sensorValuesp, int linea_anterior, int colorlinea , int setpoint)
{   
   int linea, suma, activos;
   int rango_comparacion;
   
    Leer_sensores (sensorValuesp);
    
    suma = 0;
    linea = 0;
    activos = 0;
    sensores_b = 0;
    
    
    
    for(int x=0; x<NUM_SENSORS; x++)
     {
        rango_comparacion = Rachvel.discriminate[x];
        //rango_comparacion = (sensorValues_max[x] + sensorValues_min[x])/2; //DISCRIMINANTE TEMPORAL
        
       if(colorlinea)
       {
             if(sensorValuesp[x] < rango_comparacion) // NEGRA LINEA NEGRA - FONDO BLANCO
             {
                suma += pesos[x + setpoint];
                sensores_b += 1 << x;
                activos ++;
                Rachvel.S[x] = 1;
             }
             else
             {
               Rachvel.S[x] = 0;
             }
       }
       else
       {
             if(sensorValuesp[x] > rango_comparacion) // ES MENOR ES POR QUE ES BLANCO LINEA-- FONDO NEGRO
             {
                suma += pesos[x + setpoint];
                sensores_b += 1 << x;
                activos ++;
                Rachvel.S[x] = 1;
             }       
             else
             {
                 Rachvel.S[x] = 0;
             }
       }
     }
     
     if(activos > 0)
     {
         linea = suma/activos;
     }
     else
     {
         
          if(linea_anterior > 10)
          {linea = pesos[setpoint+8];}
          else if(linea_anterior < -10)
          {linea = pesos[setpoint -1];}
          else
          {linea = 0;}
     
     }
     return linea;
     
}


void Leer_sensores (unsigned int* sensorValuesp)
{
  int sensor_time = 0;
  int val;
  
      for(int x=0; x<NUM_SENSORS; x++)
       {
              sensorValuesp[x] = analogRead(pins[x]);
       }
}
