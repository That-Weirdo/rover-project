#ifndef DIFF_DRIVE_CONTROL_H
#define DIFF_DRIVE_CONTROL_H

#include <memory>
#include <string>
#include <vector>

#include "hardware_interface/handle.hpp"
#include "hardware_interface/hardware_info.hpp"
#include "hardware_interface/system_interface.hpp"
#include "hardware_interface/types/hardware_interface_return_values.hpp"
#include "rclcpp/clock.hpp"
#include "rclcpp/duration.hpp"
#include "rclcpp/macros.hpp"
#include "rclcpp/time.hpp"
#include "rclcpp_lifecycle/node_interfaces/lifecycle_node_interface.hpp"
#include "rclcpp_lifecycle/state.hpp"

#include <pigpiod_if2.h>

// Demo from https://github.com/masum919/ros2_control_custom_hardware_interface/blob/main/motor_controller/motor_controller/include/motor_controller/arduino_interface.hpp

namespace rover_hardware 
{

using CallbackReturn = rclcpp_lifecycle::node_interfaces::LifecycleNodeInterface::CallbackReturn;

class DiffDrive : public hardware_interface::ActuatorInterface
{
	public:
		DiffDrive();
		virtual ~DiffDrive();

		//void LifecycleNodeInterface();
		//void on_configure();
		//void on_cleanup();
		//void on_shutdown();
		//void on_error();

		hardware_interface::CallbackReturn on_init(
				const hardware_interface::HardwareInfo &hardware_info) override;

		hardware_interface::CallbackReturn on_configure(
				const rclcpp_lifecycle::State & previous_state) override;

		CallbackReturn on_activate(
				const rclcpp_lifecycle::State & previous_state) override;

		CallbackReturn on_deactivate(
				const rclcpp_lifecycle::State & previous_state) override;

		hardware_interface::return_type read(
				const rclcpp::Time &time, const rclcpp::Duration& period) override;

		hardware_interface::return_type write(
				const rclcpp::Time &time, const rclcpp::Duration & period) override;

	private:
		int pi = 0; //Holds pigpio hardware information. 0 until configured.
};
}

#endif
