#! /bin/bash

# Run container with volumes ported over
if [[ $1 = "-t" ]]; then
	echo "Opening terminal..."
	docker run -it --privileged -u root \
		-v /sys:/sys \
		-v /dev/mem:/dev/mem \
		-v $(pwd)/rover_ws/src/rover:/root/rover_ws/src/rover \
		-v $(pwd)/rover_ws/src/rover_hardware:/root/rover_ws/src/rover_hardware \
		rover:jazzy-ros \
		/bin/bash -c\
			"source /root/rover_ws/install/setup.sh; colcon build --merge-install --symlink-install; /bin/bash"
			#"colcon build --merge-install --symlink-install && source install/setup.sh && /bin/bash"

else
	echo "Starting Script..."
	docker run -it --privileged -u root \
		-v /sys:/sys \
		-v /dev/mem:/dev/mem \
		-v $(pwd)/rover_ws/src/rover:/root/rover_ws/src/rover \
		-v $(pwd)/rover_ws/src/rover_hardware:/root/rover_ws/src/rover_hardware \
		rover:jazzy-ros \
		/bin/bash -c\
			"source /opt/ros/jazzy/setup.bash && source install/setup.bash && ros2 launch rover rover.launch.py"
			#"source /opt/ros/jazzy/setup.bash && colcon build --merge-install --symlink-install && source install/setup.bash && ros2 launch rover rover.launch.py"
fi
