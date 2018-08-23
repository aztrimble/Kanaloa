# Simulation Sensor Data

This tutorial assumes that you already have Ubuntu 16.04 and ROS Kinetic installed on your machine.

## Prerequisites

1. Ubuntu 16.04 [instructions here](https://github.com/riplaboratory/Kanaloa/blob/master/Tutorials/SoftwareInstallation/Ubuntu16.04/readme.md)
2. Standard .bashrc inclusions [instructions here](https://github.com/riplaboratory/Kanaloa/blob/master/Tutorials/SoftwareInstallation/.bashrc_inclusions/readme.md)
3. ROS Kinetic [instructions here](https://github.com/riplaboratory/Kanaloa/blob/master/Tutorials/SoftwareInstallation/ROS/Kinetic/readme.md)
4. RobotX Simulation [instructions here](https://github.com/riplaboratory/Kanaloa/blob/master/Tutorials/SoftwareInstallation/RobotX%20Simulation/readme.md)

##Overview
These inputs and outputs of this simulation work via ROS topics, which can be Published to (Send data to) or Subscribed (Retrieve data from) to. This document discusses the topics present in this simulation, and the parameters used to manipulate the simulation. The first section talks about the Motor Input Data, which are inpput topics which can be published to in order to control the boat thrusters. The second section discusses the sensor data topics which can be subscribes to in order to retrieve data from the GPS and IMU. 

## The Motor Input Data
The following is the ROS information for giving commands to the 4 thrusters of the holonomic drive. A python example on how to code input can be found [here](https://github.com/riplaboratory/Kanaloa/blob/master/Tutorials/SoftwareInstallation/RobotX%20Simulation/vmrc_ws/src/vmrc/scripts/test_publisher_holonomic.py). Actual force output of motors can be edited in wamv_gazebo/urdf/dynamics/wamv_gazebo_dynamics_plugin.xacro

Thruster  | Topic name | Input range
------------- | ------------- | -------------
Left Front Thruster | /left_front_thrust_cmd  | -1.0 to 1.0
Right Front Thruster | /right_front_thrust_cmd  | -1.0 to 1.0
Left Rear Thruster | /left_rear_thrust_cmd  | -1.0 to 1.0
Right Rear Thruster | /right_rear_thrust_cmd  | -1.0 to 1.0

These inputs and outputs of this simulation work via ROS topics, which can be Published to (Send data to) or Subscribed (Retrieve data from) to. 
```console
$ rostopic pub --once /right_front_thrust_cmd std_msgs/Float32 "data: 1.0" 
```


## The Output Sensor Data
The following is the raw data that Published by the corresponding simulation sensors. The two major sensors discussed will be the GPS and IMU. Camera data is also present in the simulation. Most data types for individual topics are either float64 or string 

### 1) GPS
Topic Name  | Description | Data Output Ex.
------------- | ------------- | -------------
/gps/fix | Misc data of simulation gps parameters  |   <Data too large for text box>
/gps/fix/frame_id   | Name of the gazebo id for gps  | "gps_wamv_link"
/gps/fix/latitude | Latitude position of GPS module | 21.3099275057
/gps/fix/longitude | Longitude position of GPS module | -157.890056696
/gps/fix/altitude | Altitude of GPS module | 1.19869570655
/gps_/fix_velocity | Return Vector data of GPS | <Data too large for text box>
/gps_fix_velocity/vector | Returns xyz data of Vector | <Data too large for text box>
/gps_fix_velocity/vector/x |Returns X vector data | -0.00301705844586
/gps_fix_velocity/vector/y |Returns Y vector data | -0.0024224246569
/gps_fix_velocity/vector/z |Returns Z vector data | 0.0236897020918



### 2) IMU
Topic Name  | Description | Data Output Ex.
------------- | ------------- | -------------
/imu/data | All data of simulation IMU  |   <Data too large for text box>
/imu/data/orientation | Orientation vector data of IMU  |   <Data too large for text box>
/imu/data/orientation/x | X orientation of IMU  |   -0.00175234455745
/imu/data/orientation/y | Y orientation of IMU  |   -0.00279812650758
/imu/data/orientation/z | Z orientation of IMU  |   0.00122652490272
/imu/data/orientation/w | Z orientation of IMU  |   0.999993797687
/imu/data/angular_velocity | Angular Velocity vector |   <Data too large for text box>
/imu/data/angular_velocity/x | X angular Velocity |  0.00700244764078
/imu/data/angular_velocity/y | Y angular Velocity |  0.0105729202332
/imu/data/angular_velocity/z | Z angular Velocity |  -8.44440447166e-05
/imu/data/linear_acceleration | Linear acceleration vector |   <Data too large for text box>
/imu/data/linear_acceleration/x | X Linear acceleration |  0.053069672376
/imu/data/linear_acceleration/y | Y Linear acceleration |  -0.0328918310041
/imu/data/linear_acceleration/z | Z Linear acceleration |  9.74738495513

### 3) Camera
Cameras on the WAMV may incluse a front camera as well as stereo cameras. All raw image data can be viewed using the command:
```console
$ rqt_image_view
```
Topic Name  | Description 
------------- | ------------- 
/front_camera/image_raw | Raw Image Data from Front Camera
/front_camera/image_raw/compressed | Compressed Image Data from Front Camera
/stereo_right/image_raw | Raw Image Data from Right Camera
/stereo_left/image_raw | Raw Image Data from Right Camera
