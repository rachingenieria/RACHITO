#ifndef RACHVEL_h
#define RACHVEL_h
#include "Arduino.h"

//--------------------------------------------------------------------------------------//



//ESTRUCTURA PARA SALVAR DATOS
class rachvel
{
  public:
    rachvel (void);
    void setupconfig(int vavg,int kpg, int  kdg, int pmw_t,int firware_ver);

        int ver;
        int vavg;
        int kpg;
        int kdg;
        int pmw_t; //LINEA "0"= Blanco ,"1" = NEGRO
        
        int colorlinea;
        int position_line;
        int start;
        int remoto_enable;
        int sw_enable;
        int mx, my;
        
  private:
        
        //variables de control
};
 
#endif
