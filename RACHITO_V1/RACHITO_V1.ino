#include <stdio.h>
#include <string.h>

#include "eeprom.h"
#include "motores.h"
#include "linea.h"
#include "api.h"
#include "rachvel.h"

#define FIRMWARE_VERSION     15 

//--------------------------------------------------------------------------------------//

rachvel Rachvel;
Motores motor;
slinea Slinea;

//------------------------------------------------------------------------------------//
//Asignacion de Pines de conexion

//Sensores
unsigned char sensorline_pins[NUM_SENSORS] = {A0,A1,A2,A3,A4,A5,A6,A7}; // SENSORES DEL 0 AL 8 QTR8

//MOTOR - Cualquier GPIO funciona
#define MI_AIN1    9
#define MI_AIN2    3
#define MD_AIN1    11   
#define MD_AIN2    10

// LISTADO DE PINES Y CONECCIONES
#define LED1           7
#define LED2           6
#define SW1            5
#define ON_RF          13

//------------------------------------------------------------------------------------//
//PARAMETROS del Control del Velocista
//AQUI SE MODIFICAN LOS PARAMETROS DE COMPETENCIA
//VALORES DE CONTROL POR DEFECTO
int   VELCIDAD_MAXIMA        = 36;       //Velocidad Maxima (entre 0 y 100)
int   CTE_PROPORCIONAL       = 8;      //Constante de Control Proporcional (ente 1 y 20)
int   CTE_DERIVATIVA         = 32;      //Constante de Control Diferencia (ente 1 y 20)
int   V_TURBINA              = 30;      //Constante de Control Diferencia (ente 1 y 20)                                                                                                                                                                                                                  
int   PISTACOLOR             = 0;


//------------------------------------------------------------------------------------//
//Variables para Control Adicional
#define BUFFER_ERROR 300 //Tamaño de recta a detectar

int error[BUFFER_ERROR];
float power_difference, power_difference_ext;
float power_difference_ant;


int detect_recta_ant, detect_recta;
char stat_sw = 0; 

int recta_tamano = 0;
int recta_tamano_ultimo = 0;
//------------------------------------------------------------------------------------//
 
void setup()
{
  int val;
  
  pinMode(LED2, OUTPUT);
  pinMode(LED1, OUTPUT);
  
  pinMode(SW1,INPUT);
  digitalWrite(SW1, HIGH); //PULL UP 
  
  pinMode(ON_RF,INPUT); 
  digitalWrite(ON_RF, LOW); //PULL DOWN
   
  Serial.begin(115200);

  Slinea.Asignacion_Pines(sensorline_pins,8);

  motor.Motor_Init(MI_AIN1,MI_AIN2,MD_AIN1,MD_AIN2);
  motor.SetSpeeds(0,0);

  Eeprom_read();
  if(Rachvel.ver != FIRMWARE_VERSION)// DATOS CORRECTOS y CARGADOS
  {
    Rachvel.setupconfig(VELCIDAD_MAXIMA,CTE_PROPORCIONAL,CTE_DERIVATIVA,V_TURBINA,FIRMWARE_VERSION); //valres por DEFECTO
    Eeprom_save();
  }
  
  int cursorz = 0;
  int menuactivo = 0;
  
 // MENU DE CONFIGURACION
 Serial_send_variables();


  digitalWrite(LED2, HIGH);
  digitalWrite(LED1, HIGH);
  delay(500);
  digitalWrite(LED2, LOW);
  digitalWrite(LED1, LOW);
  
  motor.SetSpeeds(0, 0); 
  val = 1;
  int mx = 0;
  int my = 0;
  Rachvel.mx = 0;
  Rachvel.my = 0;
  
  do{
      //RECIBIR DATOS POR BLUETOOTH
      Serial_command();
      val = digitalRead(SW1);  
      
         mx =  -(Rachvel.vavg * Rachvel.mx)/10;
         my =   (Rachvel.vavg * Rachvel.my)/10;
      
      motor.SetSpeeds(mx + my, mx - my);
      
  }while (val);
  

  motor.SetSpeeds(0, 0); 

  digitalWrite(LED2, LOW);
  digitalWrite(LED1, LOW);
  delay(200);
  digitalWrite(LED2, HIGH);     
  digitalWrite(LED1, HIGH);
  delay(200);
  digitalWrite(LED2, LOW);     
  digitalWrite(LED1, LOW);
  delay(200);                  
  digitalWrite(LED2, HIGH);     
  digitalWrite(LED1, HIGH);   
  delay(200);
  digitalWrite(LED2, LOW);     
  digitalWrite(LED1, LOW); 
  

//-------------Instrucciones para Empezar a hacer la Calibracion de Sensores--------------------------------------//
  
  Slinea.Reset_Calibracion(); //ROBOT EN MEDIO DE LA LINEAS
  Rachvel.colorlinea = Slinea.Calibrar_Color_Linea();
  digitalWrite(LED2, LOW);
  digitalWrite(LED1, HIGH);
  
  //GIRA MIENTRA CALIBRA
  motor.SetSpeeds(-250, 250);
  int tiempo_cal = NUM_MUESTRAS + 1;
  while(tiempo_cal--)
  {
      Slinea.Calibrar_Sensores();
      delay(5);
  }
  digitalWrite(LED2, HIGH);
  digitalWrite(LED1, LOW);
  motor.SetSpeeds(0, 0);

  val = digitalRead(SW1);  
  Rachvel.position_line = 60;
  
  while (val == HIGH )
          {
             val = digitalRead(SW1);  
             Rachvel.position_line = Slinea.Leer_linea(Rachvel.position_line ,Rachvel.colorlinea, 5 ); // leemos posicion de la linea en la variable position
          
             
               if (Rachvel.position_line < -20)
               {
                   digitalWrite(LED2, LOW);
                   digitalWrite(LED1, HIGH); 
                }
                else  if (Rachvel.position_line > 20 )
                {
                   digitalWrite(LED2, HIGH);
                   digitalWrite(LED1, LOW);    
                }
          
                error[5]=error[4];
                error[4]=error[3];
                error[3]=error[2];
                error[2]=error[1];
                error[1]=error[0];
                error[0]=Rachvel.position_line;
          
                power_difference = (error[0] * Rachvel.kpg) + ((error[0] - error[4]) * Rachvel.kdg);
                motor.SetSpeeds( - power_difference,  power_difference);
              
                delay(1);
}
  
   //---------------------------FIN DE PRUEBA DE CALIBRACION----------------------------------------------------//
   //stop Motors
  motor.SetSpeeds(0,0);
  
  digitalWrite(LED2, LOW);
  digitalWrite(LED1, LOW);
  delay(200);
  digitalWrite(LED2, HIGH);     
  digitalWrite(LED1, HIGH);
  delay(200);
  digitalWrite(LED2, LOW);     
  digitalWrite(LED1, LOW);
  delay(200);                  
  digitalWrite(LED2, HIGH);     
  digitalWrite(LED1, HIGH);   
  delay(200);
  digitalWrite(LED2, LOW);     
  digitalWrite(LED1, LOW);
  
 //---------------------------Verificacion de Sensores----------------------------------------------------//
  val = digitalRead(SW1);  
  while (val == HIGH )
  {    
     Serial_send_variables();
     Rachvel.position_line = Slinea.Leer_linea(Rachvel.position_line ,Rachvel.colorlinea, 5 ); 
     delay(100);
     val = digitalRead(SW1); 
  }

 //---------------------------LISTO PARA COMPETIR----------------------------------------------------// 
 //---------------------------SELECIONAR METODO DE INICO----------------------------------------------------//
  
  digitalWrite(LED2, LOW);
  digitalWrite(LED1, LOW);
  delay(200);
  digitalWrite(LED2, HIGH);     
  digitalWrite(LED1, HIGH);
  delay(200);
  digitalWrite(LED2, LOW);     
  digitalWrite(LED1, LOW);
  delay(200);                  
  digitalWrite(LED2, HIGH);     
  digitalWrite(LED1, HIGH);   
  delay(200);
  digitalWrite(LED2, LOW);     
  digitalWrite(LED1, LOW);
  delay(200);                  
  digitalWrite(LED2, HIGH);     
  digitalWrite(LED1, HIGH); 
 

// INICIO CON MODULO REMOTO
  
   Rachvel.start = 0; //STOP BLUETOOTH
   int rf_control = digitalRead(ON_RF);

  digitalWrite(LED2, HIGH); //ENCIENDE LEDS
  digitalWrite(LED1, HIGH); 
  val = 0;  
  
   while(rf_control == 0 && Rachvel.start == 0 && stat_sw == 0)
   {
      rf_control = digitalRead(ON_RF);
      Serial_command();
      delay(2);
      val = digitalRead(SW1); 
      if(val == LOW)
      {
          digitalWrite(LED2, HIGH);
          digitalWrite(LED1, HIGH);
          delay(1000);
          digitalWrite(LED2, HIGH);
          digitalWrite(LED1, LOW);
          delay(1000);
          digitalWrite(LED2, LOW);
          digitalWrite(LED1, HIGH);
          delay(1000);
          digitalWrite(LED2, HIGH);
          digitalWrite(LED1, HIGH);
          delay(1000);
          digitalWrite(LED2, LOW);
          digitalWrite(LED1, LOW);
          delay(999);
          stat_sw =  1;
      }
   }


  if(Rachvel.start == 0xFF) // MODULO CONECTADO PERO ARRANCA POR SW
  {
      stat_sw =  1; //NO PARA POR MODULO
  }
  
  if(rf_control) // MODULO NO CONECTADO, ARRANCA POR SW
  {
    stat_sw =  0; //NO PARA POR MODULO
  }

   Rachvel.start = 0xFF; //START BLUETOOTH

   digitalWrite(LED2, LOW);
   digitalWrite(LED1, LOW); 

   detect_recta_ant = 1;
   detect_recta = 1;
  
}
 
void loop()
{
 
//APAGADO POR MODULO REMOTO
   int rf_control = digitalRead(ON_RF);
   if (rf_control == 0 && stat_sw == 0)
   {
       motor.SetSpeeds(0, 0);
       while(1)
       {
          digitalWrite(LED2, HIGH);
          digitalWrite(LED1, HIGH);
          delay(500);
          digitalWrite(LED2, LOW);
          digitalWrite(LED1, LOW);
          delay(500);
        
        }
   } // STOP ROBOT

   if (Rachvel.start == 0 && stat_sw == 1)
   {
       motor.SetSpeeds(0, 0);
   }

 if( Rachvel.start )
  {
    Rachvel.position_line = Slinea.Leer_linea(Rachvel.position_line ,Rachvel.colorlinea, 5 ); // leemos posicion de la linea en la variable position

       /*
       //led para curva
       if (Rachvel.position_line < -20)
       {
           digitalWrite(LED2, LOW);
           digitalWrite(LED1, HIGH); 
        }
        else  if (Rachvel.position_line > 20 )
        {
           digitalWrite(LED2, HIGH);
           digitalWrite(LED1, LOW);    
        }
      */

   
   //Valores anteriores
    for(int ind = BUFFER_ERROR-1; ind > 0 ; ind --)
    {
      error[ind] = error[ind-1];
    }
     error[0] = Rachvel.position_line;
    

    int vavg= Rachvel.vavg;
    vavg = vavg*10; 
    power_difference = (error[0] * Rachvel.kpg) + ((error[0] - error[6]) * Rachvel.kdg);

    //Calculo de Recta  
    int suma_recta = 0;
    int sensor_curva = 0;
    
    detect_recta_ant = detect_recta;
   detect_recta = 0;
   
     for(int i=0 ; i<BUFFER_ERROR; i++)
      {
        suma_recta = error[i];
        if( error[i] > 20 || error[i] < -20)
        {
          sensor_curva = 1;
        }
      }

    suma_recta = suma_recta/BUFFER_ERROR;

    if(sensor_curva == 0)
    {
      if(abs(suma_recta) < 8)
      {
        detect_recta = 1;
      }
    }
    


      if(detect_recta == 0 && detect_recta_ant == 1) //CAmbio de Recta a curva
      {
           digitalWrite(LED2, HIGH);
           digitalWrite(LED1, HIGH); 
           motor.SetSpeeds(-50,-50); //Freno
           delay(15); //tiempo proporcional a la longitud de la recta
           //motor.SetSpeeds(vavg,vavg); //Freno
           //delay(100); //tiempo proporcional a la longitud de la recta
           digitalWrite(LED2, LOW);
           digitalWrite(LED1, LOW); 
      }
      
 
     //motor.SetSpeeds(vavg  - power_difference, vavg +  power_difference);
     if(power_difference > 0)
     {
       motor.SetSpeeds(vavg  - power_difference,  vavg +  (power_difference*0.2));
        //motor.SetSpeeds(vavg  - power_difference, vavg);
     }
     else if(power_difference < 0)
     {
        motor.SetSpeeds(vavg  - (power_difference*0.2), vavg +  power_difference);
        //motor.SetSpeeds(vavg  - power_difference, vavg);
     }
     else
     {
       motor.SetSpeeds(vavg,vavg);
     }
  }
  else
  {
     motor.SetSpeeds(0, 0);
  }
  
  //SERIAL STOP
   Serial_command();

   delay(1);
}
