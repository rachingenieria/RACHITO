#ifndef LINEA_h

#define MAX_VALUE                 1024
#define NUM_MUESTRAS              500

//Sensores de Linea PD
#define NUM_SENSORS             8  // Numero de sensores que usa

class slinea
{
  public:
  
    slinea (void);
    void Asignacion_Pines(unsigned char* sensors_pins, int sensor_num);
    void Leer_sensores (void);
    void Reset_Calibracion(void);
    int Calibrar_Color_Linea(void);
    void Calibrar_Sensores(void);
    int Leer_linea(int linea_anterior, int colorlinea, int setpoint);

    
    int colorlinea;
    int position_line;
    int S[8];
    int discriminate[NUM_SENSORS];  
    int sensorValues[NUM_SENSORS];    
        
  private:
        //variables de control
        int sensor_num;
        unsigned char pins[NUM_SENSORS];
        int pesos[NUM_SENSORS*2+2] = {-60,-60,-60,-60,-60,-35,-25,-15,-5,5,15,25,35,60,60,60,60,60}; // SENSORES DEL 0 AL 8 QTR8
        
        int sensorValuesp[NUM_SENSORS];
        unsigned int sensorValues_max[NUM_SENSORS];
        unsigned int sensorValues_min[NUM_SENSORS];

        int minimo_general = 1024;
        int maximo_general = 0;
        int num_muestras = 0;
        int sensores_b;
        int negro[NUM_SENSORS], blanco[NUM_SENSORS];
        int num_negro[NUM_SENSORS], num_blanco[NUM_SENSORS];
};

#endif
