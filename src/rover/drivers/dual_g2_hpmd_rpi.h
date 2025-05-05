#ifndef DUAL_G2_HPMD_RPI_H
#define DUAL_G2_HPMD_RPI_H

#include <pigpio_if2.h>
#include <iostream>
#include <array>
#include <time>
#include <stdexcept>

#define MAX_SPEED 480

#define pin_M1FLT 5
#define pin_M2FLT 6
#define pin_M1PWM 12
#define pin_M2PWM 13
#define pin_M1EN 22
#define pin_M2EN 23
#define pin_M1DIR 24
#define pin_M2DIR 25

class Motor 
{
private: 
    int pwm_pin, dir_pin, en_pin, flt_pin;

    int pi; // Return code from pigpio_start

public: 
	Motor(int pi, int pwm, int dir, int en, int flt);

	void setSpeed(int speed);

	void enable();

	void disable();

	int getFault();
}

class Motors
{
	private:
		int pi; // Return code from pigpio_start
	public:
		Motor motor1;
		Motor motor2;

		Motors();

		void setSpeeds(m1_speed, m2_speed);

		void enable();

		void disable();

		std::array<int, 2> getFaults();

		void forceStop():

}
#endif
