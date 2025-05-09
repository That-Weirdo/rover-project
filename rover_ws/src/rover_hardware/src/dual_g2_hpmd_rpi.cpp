#include "dual_g2_hpmd_rpi.h"
// Initialize gpio

#define MAX_SPEED 480

// Motor speeds for this library are specified as numbers between -MAX_SPEED and
// MAX_SPEED, inclusive.
// This has a value of 480 for historical reasons/to maintain compatibility with
// older libraries for other Pololu boards (which used WiringPi to set up the
// hardware PWM directly).


Motor::Motor(const int& pi, int pwm, int dir, int en, int flt): pi(pi), pwm_pin(pwm), dir_pin(dir), en_pin(en), flt_pin(flt) 
{
	set_mode(pi, pwm, PI_OUTPUT);
	set_mode(pi, dir, PI_OUTPUT);
	set_mode(pi, en, PI_OUTPUT);
	set_mode(pi, flt, PI_INPUT);

	set_pull_up_down(pi, flt_pin, PI_PUD_UP); // make sure FLT is pulled up
	gpio_write(pi, en_pin, PI_LOW); // disable driver by default
}

void Motor::setSpeed(int speed) {
	int dir_value; // Forward by default

	if (speed < 0) {
		speed = -speed;
		dir_value = PI_HIGH;
	} 
	else dir_value = PI_LOW;

	if (speed > MAX_SPEED) speed = MAX_SPEED;

	gpio_write(pi, dir_pin, dir_value);

	// 20 kHz PWM, duty cycle in range 0-1000000 as expected by pigpio;
	hardware_PWM(pi, pwm_pin, 20000, speed * 6250 / 3); 

}

void Motor::enable() {
	gpio_write(pi, en_pin, PI_HIGH);
}

void Motor::disable() {
	gpio_write(pi, en_pin, PI_LOW);
}

int Motor::getFault() {
	return !gpio_read(pi, flt_pin);
}


Motors::Motors(int& pi): 
	pi(pi),
	motor1(pi, pin_M1PWM, pin_M1DIR, pin_M1EN, pin_M1FLT), 
	motor2(pi, pin_M2PWM, pin_M2DIR, pin_M2EN, pin_M2FLT) {

	if (pi_connected) { 
		throw std::runtime_error("Can't connect to pigpio");
	}
}

void Motors::setSpeeds(int m1_speed, int m2_speed) {
	motor1.setSpeed(m1_speed);
	motor2.setSpeed(m2_speed);
}

void Motors::enable() {
	motor1.enable();
	motor2.enable();
}
void Motors::disable() {
	motor1.disable();
	motor2.disable();
}

bool Motors::getFaults() {
	return motor1.getFault() || motor2.getFault();
}

bool Motors::pi_connected() {
	return pi != 0;

void Motors::forceStop() {
	// reinitialize the pigpio interface in case we interrupted another command
	// (so this method works reliably when called from an exception handler)
	pigpio_stop(pi);
	pi = pigpio_start(nullptr, nullptr);
	if (pi != 0) throw std::runtime_error("Can't connect to pigpio");
	setSpeeds(0, 0);
}
