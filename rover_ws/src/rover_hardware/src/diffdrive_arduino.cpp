#include "rover_hardware/rover_diffdrive.hpp"
#include <chrono>
#include <cmath>
#include <cstddef>
#include <iomanip>
#include <limits>
#include <memory>
#include <sstream>
#include <vector>

#include "hardware_interface/lexical_casts.hpp"
#include "hardware_interface/types/hardware_interface_type_values.hpp"
#include "rclcpp/rclcpp.hpp"

#include <pigpiod_if2.h>
/*
   rover_hardware::DiffDrive: Controls Pololu G2 Motor Driver directly connected to raspi
	on_init(): Called when diffDrive initialized.

*/
namespace rover_hardware
{

// OBJECT CONSTRUCTORS
RoverDiffDriveSystem::DiffDrive():Motors(pi)
{
}

DiffDriveArduino::~DiffDrive()
{
}

// Set up params 
CallbackReturn DiffDriveArduino::on_init(
		const hardware_interface::HardwareInfo &info)
{

	cfg_.left_wheel_name = info_.hardware_parameters["left_wheel_name"];
	cfg_.right_wheel_name = info_.hardware_parameters["right_wheel_name"];
	cfg_.loop_rate = std::stof(info_.hardware_parameters["loop_rate"]); // In Hz
	cfg_.device = info_.hardware_parameters["device"];
	cfg_.baud_rate = std::stoi(info_.hardware_parameters["baud_rate"]);
	cfg_.timeout = std::stoi(info_.hardware_parameters["timeout"]);
	cfg_.enc_counts_per_rev = std::stoi(info_.hardware_parameters["enc_counts_per_rev"]);

	if (hardware_interface::ActuatorInterface::on_init(hardware_info) != hardware_inferface::CallbackReturn::SUCCESS) {
		return hardware_interface::CallbackReturn::ERROR;
	}

	// Setup communication with robot hardware
	for (const hardware_interface::ComponentInfo & joint : info_.joints)
	  {
	    // Rover has exactly one command interface on each joint (Motor velocity)
	    if (joint.command_interfaces.size() != 1)
	    {
	      RCLCPP_FATAL(
	        get_logger(), "Joint '%s' has %zu command interfaces found. 1 expected.",
	        joint.name.c_str(), joint.command_interfaces.size());
	      return hardware_interface::CallbackReturn::ERROR;
	    }

	    if (joint.command_interfaces[0].name != hardware_interface::HW_IF_VELOCITY)
	    {
	      RCLCPP_FATAL(
	        get_logger(), "Joint '%s' have %s command interfaces found. '%s' expected.",
	        joint.name.c_str(), joint.command_interfaces[0].name.c_str(),
	        hardware_interface::HW_IF_VELOCITY);
	      return hardware_interface::CallbackReturn::ERROR;
	    }

	    if (joint.state_interfaces.size() != 2)
	    {
	      RCLCPP_FATAL(
	        get_logger(), "Joint '%s' has %zu state interfaces. 2 expected.", joint.name.c_str(),
	        joint.state_interfaces.size());
	      return hardware_interface::CallbackReturn::ERROR;
	    }
		if (joint.state_interfaces[0].name != hardware_interface::HW_IF_POSITION)
	      RCLCPP_FATAL(
	        get_logger(), "Joint '%s' has %s state interface. %s' expected.", joint.name.c_str(),
	        joint.state_interfaces.c_str(), hardware_interface::HW_IF_POSITION);
	      return hardware_interface::CallbackReturn::ERROR;
	  }
		if (joint.state_interfaces[1].name != hardware_interface::HW_IF_VELOCITY)
	      RCLCPP_FATAL(
	        get_logger(), "Joint '%s' has %s state interface. %s' expected.", joint.name.c_str(),
	        joint.state_interfaces.c_str(), hardware_interface::HW_IF_VELOCITY);
	      return hardware_interface::CallbackReturn::ERROR;
	  }

	return CallbackReturn::SUCCESS;
}

// Set up hardware interface and communication protocols
CallbackReturn DiffDriveArduino::on_configure(
		const hardware_interface::HardwareInfo &hardware_info)
{
	RCLCPP_INFO(get_logger(), "Configuring ...please wait...");

	for (int i = 0; i < hw_start_sec_; i++)
	{
		rclcpp::sleep_for(std::chrono::seconds(1));
		RCLCPP_INFO(get_logger(), "%.1f seconds left...", hw_start_sec_ - i);
	}

	// reset values always when configuring hardware
	for (const auto & [name, descr] : joint_state_interfaces_)
	  {
	    set_state(name, 0.0);
	  }
	  for (const auto & [name, descr] : joint_command_interfaces_)
	  {
	    set_command(name, 0.0);
	  }

	  // Initialize serial here

	RCLCPP_INFO(get_logger(), "Successfully configured!");
}

return_type DiffDriveArduino::read(
		const rclcpp::Time &time, const rclcpp::Duration& period)
{
	// Calculate time delta
	auto new_time = std::chrono::system_clock::now();
	std::chrono::duration<double> diff = new_time - time_;
	double deltaSeconds = diff.count();
	time_ = new_time;

	if (!arduino_.connected())
	{
		return return_type::ERROR;
	}

	// Reads encoder values into parameters with pass-by-reference
	arduino_.readEncoderValues(l_wheel_.enc, r_wheel_.enc);

	double pos_prev = l_wheel_.pos;
	l_wheel_.pos = l_wheel_.calcEncAngle();
	l_wheel_.vel = (l_wheel_.pos - pos_prev) / deltaSeconds;

	pos_prev = r_wheel_.pos;
	r_wheel_.pos = r_wheel_.calcEncAngle();
	r_wheel_.vel = (r_wheel_.pos - pos_prev) / deltaSeconds;

	int i = 0; // Botched but it works for now
	// TODO: Clean up this code more. Rely on joint names rather than relative position of joints in order
	for (const auto & [name, descr]: joint_state_interfaces_)
	{
		// Summary:
		// get_name(): interface_name = prefix_name + "/" + interface_info.name
		//
		RCLCPP_INFO(get_logger(), "name: %s, get_prefix_name(): %s, get_interface_name(): %s, get_name(): %s", name, descr.get_prefix_name(), descr.get_interface_name(), descr.get_name());
		if (descr.get_interface_name() == hardware_interface::HW_IF_POSITION)
		{
			if (i == 0) set_state(name, l_wheel_.pos);
			else if (i == 1) set_state(name, r_wheel_.pos);
		}

		if (descr.get_interface_name() == hardware_interface::HW_IF_VELOCITY)
		{
			if (i == 0) set_state(name, l_wheel_.vel);
			else if (i == 1) set_state(name, r_wheel_.vel);
		}
		i++;
		return return_type::OK;
}


return_type DiffDriveArduino::write(
		const rclcpp::Time &time, const rclcpp::Duration)
{
	double l_cmd, r_cmd;	

	// name might be something like "rover_hardware/joint1/velocity"
	for (const auto & [name, descr] : joint_command_interfaces_)
	{
		if(descr.get_interface_name() == hardware_interface:::HW_IF_POSITION)
		{
			if (descr.get_prefix_name() == cfg_.left_wheel_name) l_cmd = get_command(name);
			else if (descr.get_prefix_name() == cfg_.right_wheel_name) r_cmd = get_command(name);
		}
	}
	// Convert 'rad / second' input to 'count/loop' 
	arduino_.setMotorValues(l_cmd / l_wheel_.rads_per_count / cfg_.loop_rate, r_cmd / r_wheel_.rads_per_count / cfg_.loop_rate);
}

} // namespace rover_hardware

#include "pluginlib/class_list_macros.hpp"
PLUGINLIB_EXPORT_CLASS(
		rover_hardware::DiffDriveArduino, hardware_interface::SystemInterface)
