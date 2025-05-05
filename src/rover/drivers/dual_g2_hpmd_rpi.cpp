#include "dual_g2_hpmd_rpi.h"
// Initialize gpio

#define MAX_SPEED 480

// Motor speeds for this library are specified as numbers between -MAX_SPEED and
// MAX_SPEED, inclusive.
// This has a value of 480 for historical reasons/to maintain compatibility with
// older libraries for other Pololu boards (which used WiringPi to set up the
// hardware PWM directly).


Motor::Motor(int pi, int pwm, int dir, int en, int flt): pwm_pin(pwm), dir_pin(dir), en_pin(en), flt_pin(flt) 
{
	set_mode(pi, pwm, PI_OUTPUT);
	set_mode(pi, dir, PI_OUTPUT);
	set_mode(pi, en, PI_OUTPUT);
	set_mode(pi, flt, PI_INPUT);

	set_pull_up_down(flt_pin, pigpio.PUD_UP) # make sure FLT is pulled up
	gpio_write(pi, en_pin, PI_HIGH); // enable driver by default
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
	pi.hardware_PWM(pi, pwm_pin, 20000, speed * 6250 / 3); 

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


Motors::Motors() {
	pi = pigpio_start();
	if (pi != 0) throw std::runtime_error("Can't connect to pigpio");

	motor1 = Motor(pi, _pin_M1PWM, _pin_M1DIR, _pin_M1EN, _pin_M1FLT);
	motor2 = Motor(pi, _pin_M2PWM, _pin_M2DIR, _pin_M2EN, _pin_M2FLT);
}

void Motors::setSpeeds(int m1_speed, int m2_speed) {
	motor1.setSpeed(m1_speed);
	motor2.setSpeed(m2_speed)
}

void Motors::enable() {
	motor1.enable();
	motor2.enable();
}
void Motors::disable() {
	motor1.disable();
	motor2.disable();
}

std::array<int, 2> Motors::getFaults() {
	return motor1.getFault() || motor2.getFault();
}

void Motors::forceStop() {
	// reinitialize the pigpio interface in case we interrupted another command
	// (so this method works reliably when called from an exception handler)
	pigpio_stop(pi);
	pi = pigpio_start();
	if (pi != 0) throw std::runtime_error("Can't connect to pigpio");
	setSpeeds(0, 0);
}
