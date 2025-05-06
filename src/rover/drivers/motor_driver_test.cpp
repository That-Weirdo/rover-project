#include <csignal>
#include <iostream>
#include <ranges>
#include <chrono>
#include <thread>
#include <exception>
#include <string>

#include <pigpiod_if2.h>

#include "dual_g2_hpmd_rpi.h"

#define PEAK_SPEED 450 
using namespace std;

int pi = pigpio_start(nullptr, nullptr);
static Motors motors(pi);

class IOError : public exception {
	private:
		string message;
	public:
		IOError(const char * msg)
			: message(msg)
		{
		}

		const char * what() const throw()
		{
			return message.c_str();
		}
};

void raiseIfFault() {
	if (motors.motor1.getFault()) {
		throw IOError("Motor 1 Fault");
	}
	if (motors.motor2.getFault()) {
		throw IOError("Motor 2 Fault");
	}
}


void handler(int signal) {
	std::cout << "Caught signal: " << signal << std::endl << "Shutting down motors." << std::endl;
	motors.forceStop();
	exit(signal);
}


int main(int argc, char *argv []) {
	using namespace std;
	// Ctrl + C Behavior Override
	struct sigaction sigIntAction{};
	sigIntAction.sa_handler = handler;
	sigemptyset(&sigIntAction.sa_mask);
	sigIntAction.sa_flags = 0;
	sigaction(SIGINT, &sigIntAction, nullptr);

// Set up sequences of motor speeds.

	try {
		motors.setSpeeds(0, 0);

		cout << "Motor 1 forward" << endl;
		for (int s = 0; s <= PEAK_SPEED; s++) {
			motors.motor1.setSpeed(s);
			raiseIfFault();
			this_thread::sleep_for(chrono::milliseconds(2));
		}

		this_thread::sleep_for(chrono::seconds(1));

		for (int s = PEAK_SPEED; s >= 0; s--) {
			motors.motor1.setSpeed(s);
			raiseIfFault();
			this_thread::sleep_for(chrono::milliseconds(2));
		}


		cout << "Motor 1 reverse" << endl;
		for (int s = 0; s >= -PEAK_SPEED; s--) {
			motors.motor1.setSpeed(s);
			raiseIfFault();
			this_thread::sleep_for(chrono::milliseconds(2));
		}

		this_thread::sleep_for(chrono::seconds(1));

		for (int s = -PEAK_SPEED; s <= 0; s++) {
			motors.motor1.setSpeed(s);
			raiseIfFault();
			this_thread::sleep_for(chrono::milliseconds(2));
		}

		// Disable the drivers for half a second.
		motors.disable();
		this_thread::sleep_for(chrono::milliseconds(500));
		motors.enable();
		this_thread::sleep_for(chrono::milliseconds(50));

		// Change direction.

		cout << "Motor 2 forward" << endl;
		for (int s = 0; s <= PEAK_SPEED; s++) {
			motors.motor2.setSpeed(s);
			raiseIfFault();
			this_thread::sleep_for(chrono::milliseconds(2));
		}

		this_thread::sleep_for(chrono::seconds(1));

		for (int s = PEAK_SPEED; s >= 0; s--) {
			motors.motor2.setSpeed(s);
			raiseIfFault();
			this_thread::sleep_for(chrono::milliseconds(2));
		}

		cout << "Motor 2 reverse" << endl;
		for (int s = 0; s >= -PEAK_SPEED; s--) {
			motors.motor2.setSpeed(s);
			raiseIfFault();
			this_thread::sleep_for(chrono::milliseconds(2));
		}

		this_thread::sleep_for(chrono::seconds(1));

		for (int s = -PEAK_SPEED; s <= 0; s++) {
			motors.motor2.setSpeed(s);
			raiseIfFault();
			this_thread::sleep_for(chrono::milliseconds(2));
		}
	}

	catch (string e) {
		// Stop the motors, even if there is an exception
		cout << "Caught Error: " << e << endl;
	}
	motors.forceStop();
}
