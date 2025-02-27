import os
from launch import LaunchDescription
from launch_ros.actions import Node

def generate_launch_description():
    encoder_node = Node(
        package='serial_driver',
        executable='serial_bridge',
        namespace='rover',
        name='enc_drv',

        parameters=[
            {"device_name":'/dev/ttyACM0'},
            {'baud_rate':115200},
            {'flow_control':"none"},
            {'parity':"none"},
            {'stop_bits':"1"},
        ]
    )
    #diff_drive = Node(
    #        package='ros2_control',
    #        executable='diff_drive_controller',
    #        namespace='rover',
    #        name='diff_drive',
    #        parameters=[
    #            {
    node_robot_state_publisher = Node(
            package='robot_state_publisher',
            executable='robot_state_publisher',
            name='robot_state_publisher',
            output='screen',
            parameters=[{
                'robot_description': ParameterValue(Command(['xacro ', str(path_to_urdf)],




    return LaunchDescription([
        encoder_node,
    ])
