#Docker image generator for RPI3B+ connected to pololu G2 motor controller, Encoders and other hardware.
FROM ros:jazzy-ros-core

# Download required packages (TODO: Maybe allow ros2 to do this automatically with rosdep instead?
RUN sudo rosdep init && rosdep update
RUN rosdep install --from-paths src -y --ignore-src

#run apt update && apt install -y \
#	ros-jazzy-ros2-control \
#	ros-jazzy-controller-manager \
#	ros-jazzy-joint-state-publisher \
#	pigpiod \

# Copy important paths
COPY ./rover_ws /root/rover_ws
#TODO: Add paths for hardware access

# Starting directory for workspace
WORKDIR /root/rover_ws

# Source setup and build
RUN . /opt/ros/jazzy/setup.sh && colcon build --merge-install 

ENTRYPOINT ["/bin/bash", "-c", ". /opt/ros/humble/setup.sh && . install/setup.sh && ros2 launch rover rover_launch.py"]
