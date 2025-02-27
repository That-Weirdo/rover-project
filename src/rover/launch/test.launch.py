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



    return LaunchDescription([
        encoder_node,
    ])
