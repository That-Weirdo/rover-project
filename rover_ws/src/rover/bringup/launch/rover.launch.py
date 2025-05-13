# Reference pulled from https://github.com/ros-controls/ros2_control_demos/blob/master/example_3/bringup/launch/rrbot_system_multi_interface.launch.py

from launch import LaunchDescription
from launch.actions import DeclareLaunchArgument, RegisterEventHandler, IncludeLaunchDescription
from launch.conditions import IfCondition
from launch.event_handlers import OnProcessExit
from launch.substitutions import Command, FindExecutable, LaunchConfiguration, PathJoinSubstitution
from launch.launch_description_sources import PythonLaunchDescriptionSource

from launch_ros.actions import Node
from launch_ros.descriptions import ParameterValue
from launch_ros.substitutions import FindPackageShare

#TODO:  Run gazebo, so it can run URDF model
#TODO:  Get ros2_control running
#TODO: Event handler to handle and log crashes

def generate_launch_description():

    """ ARGUMENTS """
    # Declare Arguments
    declared_arguments = []

    """ PACKAGE SHARES """
    rover_pkg = FindPackageShare('rover')

    robot_description_content = Command(
            [
                PathJoinSubstitution([FindExecutable(name="xacro")]),
                " ",
                PathJoinSubstitution(
                    [rover_pkg, 'urdf', 'rover.xacro']
                ),
                " ",
                "use_gazebo:=",
                "true",
            ]
        )     

    robot_description = {"robot_description": ParameterValue(robot_description_content, value_type=str)}

    # Robot State Publisher
        # Publishes robot description to a topic for others to read
    robot_state_pub_node = Node(
            package='robot_state_publisher',
            executable='robot_state_publisher',
            name='robot_state_publisher',
            output='both',
            parameters=[
                robot_description,
            ]
    )

    # Setup intermediate controllers
    robot_controllers = PathJoinSubstitution([rover_pkg, 'config', 'robot_controllers.yaml'])


    control_node = Node(
        package="controller_manager",
        executable="ros2_control_node",
        parameters=[robot_controllers],
        output="both",
    )

    # Allows publishing to Joint State Broadcaster for RVIZ
    joint_state_broadcaster_spawner = Node(
        package="controller_manager",
        executable="spawner",
        arguments=["joint_state_broadcaster"],
    )
    

    robot_controller_spawner = Node(
            package="controller_manager",
            executable="spawner",
            arguments=[
                "diffbot_base_controller", 
                "--param-file", 
                robot_controllers, 
                "--controller-ros-args", 
                "-r /diffbot_base_controller/cmd_vel:=/cmd_vel"
            ],
    )

    nodes = [
        control_node,
        robot_state_pub_node,
        robot_controller_spawner,
        joint_state_broadcaster_spawner,
    ]

    # Define all launch arguments and nodes to run
    return LaunchDescription(declared_arguments + nodes)

if __name__ == '__main__':
    generate_launch_description()

