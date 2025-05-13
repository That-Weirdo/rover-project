#Docker image generator for RPI3B+ connected to pololu G2 motor controller, Encoders and other hardware.
FROM ros:jazzy-ros-base

# Starting directory for workspace
WORKDIR /root/rover_ws

# INSTALL DEPENDENCIES
RUN apt update && apt install -y wget make unzip python3-pip
RUN git clone https://github.com/joan2937/pigpio && \
	cd pigpio && \
	make install CFLAGS=-DEMBEDDED_IN_VM

# Temporarily copy the package file to allow rosdep to find its packages (This is so jenky)
COPY ./rover_ws/src/rover/package.xml /root/rover_ws/src/rover/package.xml
COPY ./rover_ws/src/rover/setup.py /root/rover_ws/src/rover/setup.py
COPY ./rover_ws/src/rover_hardware/package.xml /root/rover_ws/src/rover_hardware/package.xml
COPY ./rover_ws/src/rover_hardware/CMakeLists.txt /root/rover_ws/src/rover_hardware/CMakeLists.txt

# Install all dependencies from src/rover/package.xml automatically
RUN rosdep update && rosdep install -r --from-paths src/rover --ignore-src --rosdistro jazzy -y

RUN rm -r pigpio

RUN apt install -y ros-jazzy-ament-cmake

RUN cd src && git clone https://github.com/joshnewans/serial.git

RUN . /opt/ros/jazzy/setup.sh && colcon build --merge-install --packages-select serial

# Source and Build

# Add any static files to copy here
#
# COPY {source} {destination}
# Set up color terminal and source jazzy's setup.sh


#Pretty terminal colors because ADHD
ENV TERM=xterm-256color
RUN echo "PS1='\e[92m\u\e[0m@\e[94m\h\e[0m:\e[35m\w\e[0m$ '" >> /root/.bashrc
RUN echo "alias ls='ls --color=auto'" >> /root/.bashrc

# Copy source command to .bashrc so it's run every session
RUN echo source /opt/ros/jazzy/setup.bash >> /root/.bashrc

# By default source setup.sh and run the launch file. Overridden when entering a manual command like /bin/bash.
# Make sure to source the workspace with . install/setup.sh.
CMD ["/bin/bash", "-c", "source install/setup.bash", "&&", "ros2 launch rover rover.launch.py"]
