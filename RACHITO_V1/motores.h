

class Motores
{
  public:
    Motores (void);
    void Motor_Init(int MI_AIN1,int MI_AIN2, int  MD_AIN1, int MD_AIN2);
    void SetSpeeds(int,int);      

        //MOTOR DERECHO
        int MOTORD_AIN1;  
        int MOTORD_AIN2;  
        
        //MOTOR IZQUIERDO
        int MOTORI_AIN1;
        int MOTORI_AIN2;

        int motord, motori;
        
  private:

        
};
