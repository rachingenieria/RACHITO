
#define MAX_VALUE                 1024
#define NUM_MUESTRAS              500

//Sensores de Linea PD
#define NUM_SENSORS             8  // Numero de sensores que usa

void Leer_sensores (unsigned int* sensorValuesp);
void Reset_Calibracion(void);
int Calibrar_Color_Linea(unsigned int* sensorValuesp);
void Calibrar_Sensores(unsigned int* sensorValuesp);
int Leer_linea(unsigned int* sensorValuesp, int linea_anterior, int colorlinea, int setpoint);
