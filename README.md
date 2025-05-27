# Senior Project: Autonomous Rover
Work-in-progress team project. Rebuild and refactor of a former team's project.

The bot will search for objects around itself in an area it is designated into, then collect and sort objects it finds by color, reporting back telemetry information about their positions to a computer. Once it finds all objects, it will return back to its starting position.

## Software Details
Redesign uses ROS2 Jazzy as a development base to allow for easier concurrent processing. Currently running with Docker on a PI 3B+. This computer runs the main code loop inside of a ros2 node.

Diffdrive base uses `ros2 control` to send and receive serial to the Mega. Encoder position is used by the arduino for motor closed feedback control.

Camera vision is parsed on a separate Pi 5 using OpenCV, and telemetry of coordinates is sent to the Pi 3 upon request (See Next Goals).


## Hardware Details
* Pololu G2 Motor Driver for Arduino ([Link](https://www.pololu.com/product/2518))
 * Note: Arduino code uses Raspberry Pi variant plugged into Arduino Mega for simpler driver implementation. Will require a pin remapping if the official Pololu G2 motor driver for Arduino is used (Recommended).
* 2x 12v 12Ah LiFePO4 batteries 
* Pi 5 2GB (4 to 8 preferred)
* 12" Notched Drive Axles
* 2 encoder mounts
* 2 encoder axle adapters
* 3 tires
* Caster
* 2 Encoders
* 2 motors
* Chain Drive
* Security Camera (for vision)

## Results
* Our hardware stack was complete by the end of the semester, though our setups were disjointed.
* We overcomplicated our software stack initially, which made research and development more complex. These features should have been researched and added later in development.
* For example, ROS2 Control allows for safer control of our motors, but took several weeks to learn, and discover the easiest solution to our problem using it.
* Locating and setting up security cameras for our vision system took a lot of our time, but became a good study on POE and RTSP.
* As a result however, we didn't get as much done on the project as we would have liked withih our time constraint.


## Future Goals
* All code for camera and drive can be stored directly onto a single Pi 5, simplifying the ROS setup. We were unable to add this in the time available.
  * The motor driver we used does not support the Pi 5 without massive driver overhauls, but is now controlled by an Arduino Mega. The camera can now be a singular node that parses in the background, and streams coordinates.
  * Implementation should output global coordinates, pulling world coordinates from `robot_state_publisher`, adding them to the coordinates last calculated by the camera.
* The drivebase has yet to be tuned, and drives in reverse when the PID is running, however it forwards when using raw PWM.
  * To maintainers: **Warning**: Any motor tests have to be done outside, minimum of 10 to 20 feet from all obstacles. These motors are very torquey and fast. Development of shutdown code is vital. Look at `ros2_control` documentation for setting up `on_deactivate()` function, and how to call it.
* Dead Reckoning needs tuning.
* After robot is able to locate an object, create a pathing routine to locate objects, use dead reckoning returned from the diff_drive controller to establish a boundry zone for the robot's movement. If needed, use optical flow, 9dof sensor and/or GPS with sensor fusion techniques to get a cleaner dead reckoning result.
* Design wiring harness for POE adapter and USB C PD adapter.

## Additional Hardware
* POE Adapter (24V DC step-up to 44V DC max 0.3A over POE) ([example](https://a.co/d/9A3WQek))
* USB C PD adapter(24V DC input, must support 5V 5A output safely) ([example](https://a.co/d/cJLEPb2))
  * POE class 1 wouldn't supply enough current to drive both the camera and Pi 5. POE+ would work, but the camera might not be compatible.
  
## Team Members

### Spring 2025
* **Ulises Ortega-Morales** ([Github](https://github.com/That-Weirdo))
  * Environment Sensing
    * Researched interfacing with camera over RTSP and POE display.
    * Configured Interfacing (RTSP over Ethernet).
    * Implemented Object Tracking and Identification with OpenCV2.
    * Researched and implemented Data Transmission over UART.

* **Joseph Salisbury** ([Github](https://github.com/jpsalis))
  * Movement and Dead Reckoning
    * Researched ROS2 and ROS2 Control.
    * Developed and simulated URDF model of robot with xacro macros.
    * Wired and mounted motors and encoders with custom parts.
    * Developed a hardware driver for ROS2 control, porting an old library for Humble to Jazzy.

* **Brandon Orlando**
  * Object Collection and Sorting System
    * Researched object sorting and collection systems.
    * Prototyped 3DOF manipulator with stepper motors.
  * Bot Hardware
    * Collaborated on design and wiring of drive base and power system.
