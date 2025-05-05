from dual_g2_hpmd_rpi import motors, MAX_SPEED
#include <dual_g2_hpmd_rpi.h>
#include <csignal>
#include <iostream>
#include <ranges>

#include <chrono>
#include <thread>

class IOError : public std::runtime_error {
	public:
		IOError(const char * message) : std::runtime_error(message){ }
		IOError(const std::string& message) : std::runtime_error(message){ }
};
class MotorError : public std::runtime_error {
	public:
		IOError(const char * message) : std::runtime_error(message){ }
		IOError(const std::string& message) : std::runtime_error(message){ }
};

void raiseIfFault() {
	if (motors.motor1.getFault()) {
		throw MotorError("Motor 1 Fault");
	}
	if (motors.motor2.getFault()) {
		throw DriverFault("Motor 2 Fault");
	}
}


void handler(int signal) {
	std::cout << "Caught signal: " << signal << std::endl << "Shutting down motors." << std::endl;
	motors.forcestop();
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
		for (int s = 0; s <= MAX_SPEED; s++) {
			motors.motor1.setSpeed(s);
			raiseIfFault();
			this_thread::sleep_for(chrono::milliseconds(2));
		}

		this_thread::sleep_for(chrono:seconds(1));

		for (int s = MAX_SPEED; s >= 0; s--) {
			motors.motor1.setSpeed(s);
			raiseIfFault();
			this_thread::sleep_for(chrono::milliseconds(2));
		}


		cout << "Motor 1 reverse" << endl;
		for (int s = 0; s >= -MAX_SPEED; s--) {
			motors.motor1.setSpeed(s);
			raiseIfFault();
			this_thread::sleep_for(chrono::milliseconds(2));
		}

		this_thread::sleep_for(chrono:seconds(1));

		for (int s = -MAX_SPEED; s <= 0; s++) {
			motors.motor1.setSpeed(s);
			raiseIfFault();
			this_thread::sleep_for(chrono::milliseconds(2));
		}

		// Disable the drivers for half a second.
		motors.disable();
		this_thread::sleep_for(chrono::milliseconds(500));
		motors.enable();


		cout << "Motor 2 forward" << endl;
		for (int s = 0; s <= MAX_SPEED; s++) {
			motors.motor2.setSpeed(s);
			raiseIfFault();
			this_thread::sleep_for(chrono::milliseconds(2));
		}

		this_thread::sleep_for(chrono:seconds(1));

		for (int s = MAX_SPEED; s >= 0; s--) {
			motors.motor2.setSpeed(s);
			raiseIfFault();
			this_thread::sleep_for(chrono::milliseconds(2));
		}


		cout << "Motor 2 reverse" << endl;
		for (int s = 0; s >= -MAX_SPEED; s--) {
			motors.motor2.setSpeed(s);
			raiseIfFault();
			this_thread::sleep_for(chrono::milliseconds(2));
		}

		this_thread::sleep_for(chrono:seconds(1));

		for (int s = -MAX_SPEED; s <= 0; s++) {
			motors.motor2.setSpeed(s);
			raiseIfFault();
			this_thread::sleep_for(chrono::milliseconds(2));
		}


	}

	catch (const DriverError& e) {
		// Stop the motors, even if there is an exception
	}
	motors.forceStop();

}
