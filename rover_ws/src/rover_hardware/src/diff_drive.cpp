
#include "rover_hardware/diff_drive_pololu.hpp"
#include <hardware_interface/types/hardware_interface_type_values.hpp>
#include <pluginlib/class_list_macros.hpp>
#include <fcntl.h>
#include <errno.h>
#include <termios.h>
#include <unistd.h>
#include <iostream>
#include <chrono>
#include "rclcpp/rclcpp.hpp"
#include <string>
#include <std_msgs/msg/int32_multi_array.hpp>  // Include the proper message type
#include <cmath>
#include <std_msgs/msg/float32.hpp>

#include <pigpiod_if2.h>
#include "rover_hardware/dual_g2_hpmd_rpi.h"

namespace rover_hardware
{

// OBJECT CONSTRUCTORS
DiffDrive::DiffDrive():Motors(pi)
{
}

DiffDrive::~DiffDrive()
{

}

// Set up params 
CallbackReturn DiffDrive::on_init(const hardware_interface::HardwareInfo &hardware_info)
{
	CallbackReturn result = hardware_interface::ActuatorInterface::on_init(hardware_info);
	if (result != CallbackReturn::SUCCESS)
	{
		return result;
	}

	// Otherwise run normal initialization stuff
	try 
	{
		motors = Motors(pi);
		if (pi == 0) 
		{ 
			RCLCPP_ERROR(rclcpp::get_logger("diffDriveController"), "pigpiod failed to start");
			close(SerialPort);
			return CallbackReturn::ERROR;
		}
	}
	catch(std::exception &e)
	{
		RCLPP_ERROR(rclcpp::get_logger("diffDriveController"), "Caught error on init");
		return CallbackReturn::ERROR;
	}

	velocity_commands_.reserve(info_.joints.size());
	velocity_states_.reserve(info_.joints.size());
	prev_velocity_commands_.reserve(info_.joints.size());
	position_states_.reserve(info_.joints.size());

	return CallbackReturn::SUCCESS;
}

// Set up hardware interface and communication protocols
CallbackReturn DiffDrive::on_configure(const hardware_interface::HardwareInfo &hardware_info)
{
	pi = pigpio_start(NULL, NULL);
}


// Enable Actuators(motors), bot can now move. 
DiffDrive::CallbackReturn on_activate(const rclcpp_lifecycle::State &previous_state)
{
	RCLCPP_INFO(rclcpp::get_logger("diffDriveController"), "Starting robot hardware ...");

	motors.setSpeeds(0, 0);
	motors.enable(); // Arm motors

	RCLCPP_INFO(rclcpp::get_logger("diffDriveController"), "Hardware started, ready to take commands");
	return CallbackReturn::SUCCESS;
}

// Deactivate actuators, bot is now unable to move.
DiffDrive::CallbackReturn on_deactivate(const rclcpp_lifecycle::State &previous_state) 
{
	motors.setSpeeds(0, 0); // Just for sanity purposes. Disabling the motors is a GPIO pin assignment but it doesn't hurt.
	motors.disable();
	return hardware_interface::CallbackReturn::SUCCESS;
}

/*
std::vector<hardware_interface::StateInterface> DiffDrive::export_state_interfaces()
{
	std::vector<hardware_interface::StateInterface> state_interfaces;

	for (size_t i = 0; i < info_.joints.size(); i++)
	{
		state_interfaces.emplace_back(hardware_interface::StateInterface(
			info_.joints[i].name, hardware_interface::HW_IF_VELOCITY, &velocity_states_[i]));
	}

	for (size_t i = 0; i < info_.joints.size(); i++)
	{
		state_interfaces.emplace_back(hardware_interface::StateInterface(
			info_.joints[i].name, hardware_interface::HW_IF_POSITION, &position_states_[i]));
	}

	return state_interfaces;
}

std::vector<hardware_interface::CommandInterface> DiffDrive::export_command_interfaces() {
	std::vector<hardware_interface::CommandInterface> command_interfaces;

  
	  for (size_t i = 0; i < info_.joints.size(); i++)
	  {
	    command_interfaces.emplace_back(hardware_interface::CommandInterface(
		info_.joints[i].name, hardware_interface::HW_IF_VELOCITY, &velocity_commands_[i]));
	  }

	  return command_interfaces;
}
*/

hardware_interface::return_type DiffDrive::read(const rclcpp::Time &time, const rclcpp::Duration& period)
{
}
hardware_interface::return_type DiffDrive::write(const rclcpp::Time &time, const rclcpp::Duration)
{
}

std::vector<double> velocity_commands_;
std::vector<double> prev_velocity_commands_;
std::vector<double> velocity_states_;
std::vector<double> position_states_;
}
