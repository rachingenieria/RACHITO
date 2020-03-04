#ifndef RACHVEL_h
#define RACHVEL_h
#include "Arduino.h"

//--------------------------------------------------------------------------------------//
#define FIRMWARE_VERSION     15

// LISTADO DE PINES Y CONECCIONES
#define LED1           7
#define LED2           6
#define SW1            5
#define ON_RF          13

//ESTRUCTURA PARA SALVAR DATOS
 
class rachvel
{
  public:
    rachvel (void);
    void setupconfig(int vavg,int kpg, int  kdg, int pmw_t);

        int    ver;
        int    vavg;
        int    kpg;
        int    kdg;
        int    pmw_t; //LINEA "0"= Blanco ,"1" = NEGRO
        
        int    colorlinea;
        int    position_line;
        int    S[8];
        int start;
        int remoto_enable;
        int sw_enable;
        int mx, my;
        int discriminate[8];
        
  private:
        
        //variables de control
};
 
#endif
