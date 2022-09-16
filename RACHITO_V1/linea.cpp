#include <arduino.h>

#include "linea.h"

extern slinea Slinea;

slinea::slinea(void)
{
  
}

void slinea::Asignacion_Pines(unsigned char* sensors_pins, int sensor_num)
{
   for(int x=0; x<NUM_SENSORS && x<sensor_num; x++)
   {
      pins[x]=sensors_pins[x];
   }
}

void slinea::Reset_Calibracion(void)
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


int slinea::Calibrar_Color_Linea(void)
{  
   int color_fondo = 4;

   Calibrar_Sensores ();

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
     Leer_sensores ();
     delay(500);
     Leer_sensores ();
     
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


void slinea::Calibrar_Sensores(void)
{   

    Leer_sensores ();
 
    for(int x=0; x<NUM_SENSORS; x++)
     {
         if(num_muestras == NUM_MUESTRAS/2)
         {
             discriminate[x] = (sensorValues_max[x] + sensorValues_min[x])/2; //DISCRIMINANTE TEMPORAL
         }

         if(num_muestras > NUM_MUESTRAS/2)
         {
           if(sensorValuesp[x] < discriminate[x])
             {
                    blanco[x] += sensorValuesp[x];
                    num_blanco[x] ++;
             }
             if(sensorValuesp[x]> discriminate[x])
             {
                    negro[x] += sensorValuesp[x];
                    num_negro[x] ++;
             }
          }

          if(num_muestras == NUM_MUESTRAS -1 )
          {
              blanco[x] =  blanco[x]/num_blanco[x]; //MEDIA BLANCOS
              negro[x]  =  negro[x]/num_negro[x];   //MEDIA NEGROS

              discriminate[x] = (blanco[x] + negro[x])/2; //DISCRIMINANTE FINAL
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

 
int slinea::Leer_linea(int linea_anterior, int colorlinea , int setpoint)
{   
   int linea, suma, activos;
   int rango_comparacion;
   
    Leer_sensores ();
    
    suma = 0;
    linea = 0;
    activos = 0;
    sensores_b = 0;
    
    
    
    for(int x=0; x<NUM_SENSORS; x++)
     {
        rango_comparacion = discriminate[x];
        //rango_comparacion = (sensorValues_max[x] + sensorValues_min[x])/2; //DISCRIMINANTE TEMPORAL
        
       if(colorlinea)
       {
             if(sensorValuesp[x] < rango_comparacion) // NEGRA LINEA NEGRA - FONDO BLANCO
             {
                suma += pesos[x + setpoint];
                sensores_b += 1 << x;
                activos ++;
                S[x] = 1;
             }
             else
             {
               S[x] = 0;
             }
       }
       else
       {
             if(sensorValuesp[x] > rango_comparacion) // ES MENOR ES POR QUE ES BLANCO LINEA-- FONDO NEGRO
             {
                suma += pesos[x + setpoint];
                sensores_b += 1 << x;
                activos ++;
                S[x] = 1;
             }       
             else
             {
                 S[x] = 0;
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


void slinea::Leer_sensores (void)
{
  int sensor_time = 0;
  int val;
  
      for(int x=0; x<NUM_SENSORS; x++)
       {
              sensorValuesp[x] = analogRead(pins[x]);
       }
}
