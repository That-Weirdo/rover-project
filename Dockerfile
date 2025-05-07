#Docker image generator for RPI3B+ connected to pololu G2 motor controller, Encoders and other hardware.
FROM ros:jazzy-ros-base

# Starting directory for workspace
WORKDIR /root/rover_ws

# Add a User so commands don't have to run as root
#RUN useradd ros
#USER ros

# INSTALL DEPENDENCIES
RUN apt update && apt install -y wget make unzip
RUN wget https://github.com/joan2937/pigpio/archive/master.zip && \
	unzip master.zip && \
	cd pigpio-master && \
	make && \
	make install

# Copy important paths
COPY ./rover_ws /root/rover_ws
	
RUN rosdep update && rosdep install -r --from-paths src --ignore-src --rosdistro jazzy -y

# TODO: Maybe add setup.sh to .bashrc so alternative commands don't need to run it every time
RUN echo source /opt/ros/jazzy/setup.sh >> ~/.bashrc

# BUILD
RUN . /opt/ros/jazzy/setup.sh && colcon build --merge-install --symlink-install

RUN apt install python3-pip -y 


# Add paths for hardware access
#COPY /dev/mem /dev/mem

# Always run this command on container creation
ENTRYPOINT . /opt/ros/jazzy/setup.sh && . install/setup.sh && 

#By default open user into shell
CMD ["ros2 launch rover rover.launch.py"]
