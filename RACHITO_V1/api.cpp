#include <arduino.h>
#include "rachvel.h"
#include "eeprom.h"
#include "api.h"

extern rachvel Rachvel;

int API_CONFIG(char* uart_buffer_rx)
{
int respuesta = 0;
          if( (uart_buffer_rx[0] == 'V' || uart_buffer_rx[0] == 'D' || uart_buffer_rx[0] == 'P' || uart_buffer_rx[0] == 'I'|| uart_buffer_rx[0] == 'T' || uart_buffer_rx[0] == 'S' || uart_buffer_rx[0] == 'X') 
              && (uart_buffer_rx[1] >= '0'  && uart_buffer_rx[1] <= '9')
              && (uart_buffer_rx[2] >= '0'  && uart_buffer_rx[2] <= '9'))
          {
                  if(uart_buffer_rx[0] == 'V')
                  {
                     Rachvel.vavg = (uart_buffer_rx[1] - '0' )*10 + (uart_buffer_rx[2]- '0' );
                     respuesta = 1;
                  }
                  else if(uart_buffer_rx[0] == 'P')
                  {
                     Rachvel.kpg = (float)((uart_buffer_rx[1] - '0' )*10 + (uart_buffer_rx[2]- '0' ));
                     respuesta = 1;
                  }
                  else if(uart_buffer_rx[0] == 'D')
                  {
                     Rachvel.kdg = (float)((uart_buffer_rx[1] - '0' )*10 + (uart_buffer_rx[2]- '0' ));
                     respuesta = 1;
                  }
                  else if(uart_buffer_rx[0] == 'T')
                  {
                     Rachvel.pmw_t = ( uart_buffer_rx[1] - '0' )*10 + (uart_buffer_rx[2] - '0' );
                     respuesta = 1;
                  }
                  else if(uart_buffer_rx[0] == 'X')
                  {
                     Rachvel.mx = 20 *  ((uart_buffer_rx[2] - '0' )-  5);
                     Rachvel.my = 20 *  ((uart_buffer_rx[1] - '0' ) - 5);
                  }

                  else if(uart_buffer_rx[0] == 'S')
                  {
                     if((uart_buffer_rx[1] == '1'  && uart_buffer_rx[2] == '5'))
                     {
                         Eeprom_save();
                         respuesta = 1;
                     }

                     if((uart_buffer_rx[1] == '2'  && uart_buffer_rx[2] == '0'))
                     {
                         Rachvel.start = 0XFF;
                         respuesta = 1;
                     }

                     if((uart_buffer_rx[1] == '2'  && uart_buffer_rx[2] == '5'))
                     {
                         Rachvel.start = 0;
                         respuesta = 1;
                     }

                     if((uart_buffer_rx[1] == '3'  && uart_buffer_rx[2] == '0'))
                     {
                         //Rachvel.remoto_enable = 1;
                     }

                     if((uart_buffer_rx[1] == '3'  && uart_buffer_rx[2] == '5'))
                     {
                         //Rachvel.remoto_enable = 0;
                     }

                     if((uart_buffer_rx[1] == '4'  && uart_buffer_rx[2] == '0'))
                     {
                         //Rachvel.sw_enable = 1;
                     }

                     if((uart_buffer_rx[1] == '4'  && uart_buffer_rx[2] == '5'))
                     {
                         //Rachvel.sw_enable = 0;
                     }

                     //PISO
                      if((uart_buffer_rx[1] == '5'  && uart_buffer_rx[2] == '0'))
                     {
                         Rachvel.colorlinea = 0;
                     }

                     if((uart_buffer_rx[1] == '5'  && uart_buffer_rx[2] == '5'))
                     {
                         Rachvel.colorlinea = 1;
                     }
                  }
                  
          }
          else
          {
            uart_buffer_rx[0] = 0;
            uart_buffer_rx[1] = 0;
            uart_buffer_rx[2] = 0;
          }

return respuesta;
}


int API_BUFFER(char uart_buffer_rx)
{
int respuesta = 0;
          if( uart_buffer_rx== 'V' || uart_buffer_rx == 'D' || uart_buffer_rx == 'P' || uart_buffer_rx == 'I'|| uart_buffer_rx == 'T' || uart_buffer_rx == 'S' || uart_buffer_rx == 'X') 
          {
            respuesta = 1;
          }
  return respuesta;
}


int count;
char uart_buffer_tx[32];
char uart_buffer_rx[32];

void Serial_command(void)
{
while (Serial.available()) {
      char datau = Serial.read();
      if( API_BUFFER(datau))
       {
         count = 0;
       }
      uart_buffer_rx[count] = datau;
      count ++;
      if( count == 3)
      {
        if(API_CONFIG(uart_buffer_rx))
        {
           Serial_send_variables();
        }
      }
      if(count > 3)
        count = 0;
    }
}

void Serial_send_variables(void)
{
  Serial.print("#");
  Serial.print(",");
  Serial.print(Rachvel.vavg);
  Serial.print(",");
  Serial.print(Rachvel.kpg);
  Serial.print(",");
  Serial.print(Rachvel.kdg);
  Serial.print(",");
  Serial.print(Rachvel.pmw_t);
  Serial.print(",");
  Serial.print(Rachvel.colorlinea);
  Serial.print(",");
  Serial.print(Rachvel.position_line);
  for(int s=0; s<8; s++)
  {
     Serial.print(",");
     Serial.print(Rachvel.S[s]);
  }
  Serial.print(",");

  delay(2);
  
}
