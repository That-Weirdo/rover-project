/***************************************************************
   Motor driver function definitions - by James Nugen
   *************************************************************/

#ifdef L298_MOTOR_DRIVER
  #define RIGHT_MOTOR_BACKWARD 5
  #define LEFT_MOTOR_BACKWARD  6
  #define RIGHT_MOTOR_FORWARD  9
  #define LEFT_MOTOR_FORWARD   10
  #define RIGHT_MOTOR_ENABLE 12
  #define LEFT_MOTOR_ENABLE 13

#elif defined POLOLU_DUAL_G2_RPI
  #define LEFT_MOTOR_DIR 5
  #define RIGHT_MOTOR_DIR 6
  #define LEFT_MOTOR_ENABLE 8
  #define RIGHT_MOTOR_ENABLE 9
  #define LEFT_MOTOR_PWM 2
  #define RIGHT_MOTOR_PWM 3
  #define LEFT_MOTOR_FLT 14 
  #define RIGHT_MOTOR_FLT 15
#endif

void initMotorController();
void setMotorSpeed(int i, int spd);
void setMotorSpeeds(int leftSpeed, int rightSpeed);
