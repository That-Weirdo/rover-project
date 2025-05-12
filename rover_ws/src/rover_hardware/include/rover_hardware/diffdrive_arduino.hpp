#ifndef ROVER_DIFF_DRIVE_CONTROL_H
#define ROVER_DIFF_DRIVE_CONTROL_H

#include <memory>
#include <string>
#include <vector>

#include "hardware_interface/system_interface.hpp"
#include "hardware_interface/types/hardware_interface_return_values.hpp"

#include "hardware_interface/handle.hpp"
#include "hardware_interface/hardware_info.hpp"
#include "rclcpp/clock.hpp"
#include "rclcpp/duration.hpp"
#include "rclcpp/macros.hpp"
#include "rclcpp/time.hpp"
#include "rclcpp_lifecycle/node_interfaces/lifecycle_node_interface.hpp"
#include "rclcpp_lifecycle/state.hpp"

#include <pigpiod_if2.h>
#include "rover_hardware/dual_g2_hpmd_rpi.h"

// Sources:
// 	Demo from https://github.com/masum919/ros2_control_custom_hardware_interface/blob/main/motor_controller/motor_controller/include/motor_controller/arduino_interface.hpp
// 	Ros2 Control Example 2: https://github.com/ros-controls/ros2_control_demos/blob/master/example_2/hardware/include/ros2_control_demo_example_2/diffbot_system.hpp
// 	Articulated Robotics Tutorial: https://youtu.be/J02jEKawE5U : !!OUTDATED!!
// https://control.ros.org/master/doc/ros2_control/hardware_interface/doc/writing_new_hardware_component.html

namespace rover_hardware 
{

using CallbackReturn = rclcpp_lifecycle::node_interfaces::LifecycleNodeInterface::CallbackReturn;

class RoverDiffDriveSystem : public hardware_interface::SystemInterface
{
	public:
		RoverDiffDriveSystem();
		virtual ~RoverDiffDriveSystem();

		//void LifecycleNodeInterface();
		//void on_configure();
		//void on_cleanup();
		//void on_shutdown();
		//void on_error();

		CallbackReturn on_init(
				const hardware_interface::HardwareInfo &hardware_info) override;

		CallbackReturn on_configure(
				const rclcpp_lifecycle::State & previous_state) override;

		CallbackReturn on_activate(
				const rclcpp_lifecycle::State & previous_state) override;

		CallbackReturn on_deactivate(
				const rclcpp_lifecycle::State & previous_state) override;

		return_type read(
				const rclcpp::Time &time, const rclcpp::Duration& period) override;

		return_type write(
				const rclcpp::Time &time, const rclcpp::Duration & period) override;

	private:
		// Local struct used to store state information for robot
		Config cfg_;
		ArduinoComms arduino_;

		Wheel l_wheel_;
		Wheel r_wheel_;

		rclcpp::Logger logger_;

		std::chrono::time_point<std::chrono::system_clock> time_;
};
}

#endif
