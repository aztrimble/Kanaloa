# Installing ROS Packages Primer

There are three methods of intalling a ROS package.  You should follow this order of preference.

 1. Using `apt-get` (preferred)
 2. Using `rosdep` (if an apt-get build is not avaialble)
 3. Installing package and dependencies manually (not recommended)
 
# Using `apt-get`

If the package author has made an apt-get build avaialble, this is the most preferred method.  It will be something to the effect of:

```
sudo apt-get install package-name
```

Then clone the git repository to your catkin workspace in the `src` directory.  

# Using `rosdep`

This method only works if the package author is compliant with `rosdep` (i.e. lists their dependencies properly).

Create your catkin workspace.  Navigate to the `src` directory of your catkin workspace.  Then clone the repository to your `src` directory.  It will be something to the effect of:

```
cd ~/catkin_ws/src
git clone git-repository-url
```

Then

```
cd ~/catkin_ws
rosdep update
rosdep install --from-paths src/ --ignore-src --rosdistro ${ROS DISTRO}
```

`rosdep` will search for the necessary dependencies.

# Installing package and dependencies manually (not recommended)

Clone the github repository to your catkin workspace.  Then copy this repository to your ROS installation directory (usually in `/opt/ros/[ROS VERSION]/share`).  You will then need to look up all of the package dependencies (typically they are listed on the ROS wiki for that repective package) and install them using the same process.  

# Running a launch file
Don't forget that if a launch file is only contained within a workspace (and not in /opt/ros/...), you need to source the directory where the `setup.bash` file is located (usually `source ~/catkin_ws/devel/setup.bash`)
