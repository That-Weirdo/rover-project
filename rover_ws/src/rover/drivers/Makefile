compile:
	g++ -Wall -pthread -o motor_driver_test dual_g2_hpmd_rpi.cpp dual_g2_hpmd_rpi.h motor_driver_test.cpp -lpigpiod_if2 -lrt

run:
	sudo pigpiod
	./motor_driver_test
