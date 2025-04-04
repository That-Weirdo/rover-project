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
    declared_arguments.append(
        DeclareLaunchArgument(
            "use_mock_hardware",
            default_value="false",
            description="Start robot with mock hardware mirroring command to its states.",
        )
    )

    declared_arguments.append(
        DeclareLaunchArgument(
            "gui",
            default_value="true",
            description="Start RViz2 automatically with this launch file.",
        )
    )
    declared_arguments.append(
        DeclareLaunchArgument(
            "use_sim_time",
            default_value="true",
            choices=['true', 'false']
        )
    )

    # Initialize Arguments
    use_mock_hardware = LaunchConfiguration("use_mock_hardware")
    gui = LaunchConfiguration("gui")
    use_sim_time = LaunchConfiguration("use_sim_time")


    """ PACKAGE SHARES """
    rover_pkg = FindPackageShare('rover')
    ros_gz_sim = FindPackageShare('ros_gz_sim')
    

    """ GAZEBO """


    #gazebo_params_file = PathJoinSubstitution([rover_pkg, 'config', 'gazebo_params.yaml'])

    
    # Launch Gazebo with empty world
    gazebo = IncludeLaunchDescription(
        PythonLaunchDescriptionSource([
            PathJoinSubstitution([
                ros_gz_sim,
                'launch', 
                'gz_sim.launch.py'
            ])
        ]),
        launch_arguments={
            #'config_file': gazebo_params_file,
            'gz_args': ' -r -v 3 empty.sdf'
        }.items()
    )

    # Spawn model in world based on /robot_description
    gz_spawn_entity = Node(
        package="ros_gz_sim",
        executable="create",
        output="screen",
        arguments=[
            "-topic",
            "/robot_description",
            "-name",
            "rover_diff_drive", # Originally rrbot_system_position
            "-allow_renaming",
            "true",
        ],
    )

    # Gazebo bridge
    gazebo_bridge = Node(
        package="ros_gz_bridge",
        executable="parameter_bridge",
        arguments=["/clock@rosgraph_msgs/msg/Clock[gz.msgs.Clock"],
        output="screen",
    )

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
                {"use_sim_time": use_sim_time}
            ]
    )

    # Setup intermediate controllers
    robot_controllers = PathJoinSubstitution([rover_pkg, 'config', 'robot_controllers.yaml'])

    # RVIZ
    rviz_config_file = PathJoinSubstitution([rover_pkg, 'config', 'config.rviz'])
    
    rviz_node = Node(
            package='rviz2',
            executable='rviz2',
            name="rviz2",
            arguments=['-d', rviz_config_file],
            condition=IfCondition(gui),
    )

        
    #control_node = Node(
    #    package="controller_manager",
    #    executable="ros2_control_node",
    #    parameters=[robot_controllers],
    #    output="both",
    #)

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
        gazebo,
        gazebo_bridge,
        gz_spawn_entity,
        robot_state_pub_node,
        robot_controller_spawner,
        joint_state_broadcaster_spawner,
        rviz_node
    ]

    # Define all launch arguments and nodes to run
    return LaunchDescription(declared_arguments + nodes)

if __name__ == '__main__':
    generate_launch_description()

