# Installing RobotX Simulation (Virtual Maritime RobotX Challenge)

This tutorial assumes that you already have Ubuntu 16.04 and ROS Kinetic installed on your machine. Team Kanaloa takes no credit for this project and should only be used as a reference by it's members. Original Publication credit goes to Brian Bingham, Carlos Aguero, and the RobotX community. There repository can be found here:  https://bitbucket.org/osrf/vmrc

## Prerequisites

1. Ubuntu 16.04 [instructions here](https://github.com/riplaboratory/Kanaloa/blob/master/Tutorials/SoftwareInstallation/Ubuntu16.04/readme.md)
2. Standard .bashrc inclusions [instructions here](https://github.com/riplaboratory/Kanaloa/blob/master/Tutorials/SoftwareInstallation/.bashrc_inclusions/readme.md)
3. ROS Kinetic [instructions here](https://github.com/riplaboratory/Kanaloa/blob/master/Tutorials/SoftwareInstallation/ROS/Kinetic/readme.md)

## Installing the simulation
This tutorial will walk you through the setup required to make a computer ready to run the VMRC simulations. In order to run VMRC your computer will need a discrete graphics card and will need to satisfy the minimum System Requirements. All updates and official tutorials can be found here: https://bitbucket.org/osrf/vmrc/wiki/tutorials/

These instructions contain information for building the VMRC environment in Gazebo.

### 1. Update apt
Because the simulation uses some relatively new (as of summer 2018) features in ROS and Gazebo, it is highly recommended that you upgrade the packages installed on your system:
```
sudo apt update

sudo apt full-upgrade
```
### 2. Setup and install dependencies
```
sudo sh -c 'echo "deb http://packages.ros.org/ros/ubuntu $(lsb_release -sc) main" > /etc/apt/sources.list.d/ros-latest.list'
sudo apt-key adv --keyserver hkp://ha.pool.sks-keyservers.net:80 --recv-key 421C365BD9FF1F717815A3895523BAEEB01FA116
sudo sh -c 'echo "deb http://packages.osrfoundation.org/gazebo/ubuntu-stable `lsb_release -cs` main" > /etc/apt/sources.list.d/gazebo-stable.list'
wget http://packages.osrfoundation.org/gazebo.key -O - | sudo apt-key add -
sudo apt update
sudo apt upgrade
```
```
sudo apt install cmake mercurial gazebo7 git libeigen3-dev libgazebo7-dev pkg-config python ros-kinetic-gazebo-plugins ros-kinetic-gazebo-ros ros-kinetic-hector-gazebo-plugins ros-kinetic-joy ros-kinetic-joy-teleop ros-kinetic-robot-localization ros-kinetic-ros-base ros-kinetic-teleop-tools ros-kinetic-teleop-twist-keyboard ros-kinetic-velodyne-simulator ros-kinetic-xacro ruby wget
```
### 3. Create a catkin workspace for VMRC. 
If you are familiar with ROS catkin workspaces, this is a similar concept.  These instructions have you create a new catkin workspace called `vmrc_ws`; however, you can also utilize an existing workspace.  

```
mkdir -p ~/vmrc_ws/src
cd ~/vmrc_ws/src
```
Clone the VMRC repository:
```
hg clone https://bitbucket.org/osrf/vmrc
```
Source the ROS setup.bash file:
```
source /opt/ros/kinetic/setup.bash
```
Build the catkin workspace
```
cd ~/vmrc_ws
```
You may use `catkin_make` OR `catkin build` to build.  We recommend `catkin build`.
```
catkin build
```
### 3.1. Optional: add source source to your .bashrc
This will allow you to use this workspace without the need for sourcing the `devel/setup.bash` file every time you open a new terminal window.

Open your `.bashrc` file.
```
cd
sudo gedit .bashrc
```
Add your catkin workspace directory as a source.  For the example in this tutorial, you would add the line:
```
source ~/vmrc_ws/devel/setup.bash
```
### 4. Test the software
Naviate to your catkin workspace
```
cd ~/vmrc_ws
```
Source (if you did not add this line to your `.bashrc` file.
```
source devel/setup.bash
```
Launch the simulation (this will take some time the first time you run).
```
roslaunch robotx_gazebo sandisland.launch
```
### 4.1. Common `protobuf` (protocol buffer runtime library) error
If your console throws an error similar to:
```
[libprotobuf FATAL google/protobuf/stubs/common.cc:61] This program requires version 3.5.0 of the Protocol Buffer runtime library, but the installed version is 2.6.1.  Please update your library.  If you compiled the program yourself, make sure that your headers are from the same version of Protocol Buffers as your link-time library.  (Version verification failed in "google/protobuf/any.pb.cc".) terminate called after throwing an instance of 'google::protobuf::FatalException'

```
According to [this AskUbuntu link](https://askubuntu.com/questions/800007/upgrade-protobuf-2-6-1-to-3-0-0-b3/1033028), the version of `protobuf` installed on Ubuntu 16.04 is not a current enough version for this simulation, and it cannot be direclty installed through apt.

You have two options for built installations: `pip` or `conda`.  If you use the `conda` package installer, then type:
```
conda install -c anaconda libprotobuf
```
Altneratively, you may install using pip
```
sudo pip install --upgrade pip
sudo pip install --upgrade protobuf
```
Don't forget to rebuild!
```
cd ~/vmrc_ws
catkin build
```

## Using the simulation

### Sample launch files

As a part of testing your installation, you already ran the `sandisland.launch` launch file. 

```
roslaunch robotx_gazebo sandisland.launch 
```
You can run a simulation that is configured simiarly to a typical RobotX "competition-ready" boat with the sample launch file:

```
roslaunch robotx_gazebo vmrc.launch 
```

### Thruster Configurations
There are currently 3 supported Propulsion options for the WAMV. 'H', 'T', and 'X'.  The 'H' configuration is the typical under-actuated, differential-drive thruster configuration; the 'T' configuration is a fully-actuated thruster configuration with two thrusters mounted in the surge direction and one thruster mounted in the sway direction; the 'X' configuration is an over-actuated thruster configuration with one thruster at each corner of the WAM-V frame, mounted at 45 degrees.  Each individual thruster has it's own unique ROS topic name and can be manipulated through ROS scripts.  The default thruster configuration is the 'H' configuration. To utilize a different configuration a new parameter must be put in the roslaunch command.

To launch the simulation with the WAM-V in the 'T' configuration, use:
``` 
roslaunch robotx_gazebo sandisland.launch thrust_config:=T 
```
To launch the simulation with the WAM-V in the 'X' configuration, use:
```
roslaunch robotx_gazebo sandisland.launch thrust_config:=X 
```

### Teleop-keyboard
To use the keyboard, we use the teleop_twist_keyboard package, along with a custom twist2thrust.py node to convert the Twist messages to two Float32 messages for the left and right thrusters. Forward velocity (twist.linear.x) is mapped to axial thrust (right+left) and rotational velocity (twist.linear.z) is mapped to differential thrust (usvdrive.right-usvdrive.left).

```
roslaunch robotx_gazebo usv_keydrive.launch
```

### RVIZ visualization

```
roslaunch wamv_gazebo rviz_vmrc.launch
```

### Sensors
Creating a URDF file
A URDF file is a format to describe a robot including joints, sensors, inertial properties, and more. The file is used by Gazebo, rviz, and many other ROS packages. Several example URDF files for representing a WAM-V are included in the VMRC packages.

Let's copy an example locally as a starting point:

```
cd /path_to_vmrc_ws/vmrc_ws/src/vmrc/wamv_gazebo/urdf
```
```
cp wamv_gazebo_sensors.urdf.xacro my_wamv.urdf.xacro
```
This file contains something like this:

```
cat my_wamv.urdf.xacro
```
```xacro
<?xml version="1.0"?>
<robot xmlns:xacro="http://ros.org/wiki/xacro"
       name="WAM-V">
  <!-- Basic frame of WAM-V -->
  <xacro:include filename="$(find wamv_gazebo)/urdf/wamv_gazebo.urdf.xacro" />

  <!-- ADD SENSORS -->
  <!-- Add a front camera -->
  <xacro:wamv_camera name="front_camera"/>
  <!-- Add simulated GPS -->
  <xacro:wamv_gps name="gps_wamv"/>
  <!-- Add Simulated IMU -->
  <xacro:wamv_imu name="imu_wamv"/>
  <!-- Add P3D ground truth -->
  <xacro:wamv_p3d name="p3d_wamv"/>
</robot>
```

Let's look at the contents within the <robot> tag, which describes the robot. The first line includes wamv_gazebo.urdf.xacro. This adds the basic WAM-V mesh and joints along with the plugins for dynamics. You will likely want to keep this in, unless you are using a different model or dynamics simulation.

After that several macros are added for a GPS, IMU, and ground truth pose. These macros are found in wamv_gazebo for common sensors. You can of course create your own, following those as examples.

Let's add a stereo camera pair to the robot. Add the following lines after the other sensors:

```
<xacro:property name="stereo_x" value="1.0" />
<xacro:wamv_camera name="stereo_left" x="${stereo_x}" y="0.3" z="1.5" P="${radians(15)}" />
<xacro:wamv_camera name="stereo_right" x="${stereo_x}" y="-0.3" z="1.5" P="${radians(15)}" />
```
A couple things to notice about this:

A common property "stereo_x" is used so the value is not copied in multiple places
The x,y,z and P (pitch) set where the cameras are located relative to the WAM-V base link
A python expression ${radians(15)} was used to convert 15 degrees to radians
Running robotx_gazebo with a custom WAM-V URDF
Now that you have a custom URDF modeling your WAM-V, let's run the simulation!

First, generate the compiled XML from the xacro file using this or another method:

```
rosrun xacro xacro --inorder my_wamv.urdf.xacro > my_wamv.urdf
```
Next, run the simulation with a custom urdf argument:

```
roslaunch robotx_gazebo sandisland.launch urdf:=`pwd`/my_wamv.urdf
```
You can use rqt to see your sensor topics
## Holonomic Drive and Sensors
By default when using sensors, the "H" configuration is used in the simulation. So running:
```
roslaunch robotx_gazebo sandisland.launch urdf:=`pwd`/my_wamv.urdf  thrust_config:=X
```
Will run the simulation with the sensors, but the thrust configuration will remain as the default "H" Layout. To change this you must edit the file wamv_gazebo.urdf.xacro. There are two ways of doing this. 
##### 1) The "Easy" Way
You may do it the "easy way" and replace your wamv_gazebo.urdf file with the one posted in the Kanaloa Repository [here](https://github.com/riplaboratory/Kanaloa/blob/master/Tutorials/SoftwareInstallation/RobotX%20Simulation/vmrc_ws/src/vmrc/wamv_gazebo/urdf/my_wamv.urdf). Please note that /path_to_vmrc_ws is the location of where you downloaded your simulation on your machine.
 - Remove the wamv_gazebo.urdf file 
```
cd /path_to_vmrc_ws/vmrc_ws/src/vmrc/wamv_gazebo/urdf/
```
```
rm wamv_gazebo.urdf.xacro
```
(or just use the Ubuntu File Manager application and visually delete the file)
 - [Click this link](https://raw.githubusercontent.com/riplaboratory/Kanaloa/master/Tutorials/SoftwareInstallation/RobotX%20Simulation/vmrc_ws/src/vmrc/wamv_gazebo/urdf/my_wamv.urdf)
 - Right click on the page and select "Save As"
 - Put in the name: wamv_gazebo.urdf
 - For the file type, make sure "All Files" is selected
 - Save to: /path_to_vmrc_ws/vmrc_ws/src/vmrc/wamv_gazebo/urdf/

##### 2) The "I'm a hardcore ROS developer" way
This method involved editing the wamv_gazebo.urdf.xacro file. Please note that in your simulation files there are two similarly named files, wamv_gazebo.urdf.xacro, and wamv_gazebo.urdf. Both files are located in: /path_to_vmrc_ws/vmrc_ws/src/vmrc/wamv_gazebo/urdf/

 - Open your favorite text editor
 - Edit line 7 so that
```
<xacro:wamv_gazebo thruster_layout="$(find wamv_gazebo)/urdf/thruster_layouts/wamv_aft_thrusters.xacro"/>
```
 - instead reads:
```
<xacro:wamv_gazebo thruster_layout="$(find wamv_gazebo)/urdf/thruster_layouts/wamv_x_thrusters.xacro"/>
```
 - Convert the XACRO file into a URDF
```
rosrun xacro xacro --inorder wamv_gazebo.urdf.xacro > wamv_gazebo.urdf
```
 - Run the simulation
```
roslaunch robotx_gazebo sandisland.launch urdf:=`pwd`/my_wamv.urdf  thrust_config:=X
```

** Remeber to source your simulation every time you open a new terminal:
```
source /path_to_vmrc_ws/vmrc_ws/devel/setup.bash
```
