#!/usr/bin/env python
# license removed for brevity
import rospy
from std_msgs.msg import String
from std_msgs.msg import Int32

from sensor_msgs.msg import NavSatFix
from sensor_msgs.msg import Imu
from way_point_wamv.srv import *

from math import radians
from math import atan2
from math import cos
from math import sin
from math import acos
from math import asin
from math import degrees
from math import sqrt

import sys
import time


class WAMV_Way_Point:
	
	def __init__(self, ros_node_name="wamv_navigation_node"):

		self.ros_node_name = ros_node_name

		self.navigation_indicator = False

		# Queued Coordinates And Station Keeping List
		self.queued_coordinates = []
		self.queued_station_keep_times = [] 

		# Manual Offset If IMU heading is off 
		self.imu_degree_offset_bias = 0

		# IMU Orientation, either "ENU" or "NED"
		self.imu_orientation = "ENU"

		# List Of All Thruster Publishers And Anysubscrbers
		self.publishers = []
		self.subscribers = []

		# Current Coordinate and Next Desire Coordinates List
		self.desired_coordinates = []
		self.current_coordinates = []

		# Station Kepp Timer
		self.station_keep_timer = 0

		# Curent Degree Offset And Distance To Next Coordinate
		self.degree_offset = 0
		self.distance = 0

		self.max_thrust = 1000

		# Reverse Multiplier, either == 1 of -1
		self.q1_rm = 1
		self.q2_rm = 1
		self.q3_rm = 1
		self.q4_rm = 1

		# self.init_node()
		self.init_publishers()
		self.init_subscribers()
		self.init_services()


	##################################################
	### Class Init Scripts
	##################################################

	def init_node(self):
		rospy.init_node(self.ros_node_name, anonymous=True)
		self.rate = rospy.Rate(10) # 10hz

	def init_subscribers(self):
	    self.subscribers.append(self.create_subscriber_object("fix", NavSatFix, self.distance_manager))
	    self.subscribers.append(self.create_subscriber_object("/imu/data", Imu, self.rotation_manager))

	def init_publishers(self):
	    self.publishers.append(self.create_publisher_object("q3_thruster_input", Int32))
	    self.publishers.append(self.create_publisher_object("q4_thruster_input", Int32))



	##################################################
	### Create Publishers And Subscribers
	##################################################

	def create_publisher_object(self, pub_name, pub_type):
	    rospy_publisher = rospy.Publisher(pub_name, pub_type, queue_size=10)
	    publisher_object = {"topic": pub_name, "message": pub_type, "publisher": rospy_publisher, "node_type":"Publisher"}
	    return publisher_object

	def create_subscriber_object(self, sub_name, sub_type, callback):
	    rospy_subscriber = rospy.Subscriber(sub_name, sub_type, callback)
	    subscriber_object = {"topic": sub_name, "message": sub_type, "subscriber": rospy_subscriber, "node_type":"Subscriber"}
	    return subscriber_object

	def get_publisher_topic(self, topic_name):
		return next(item for item in self.publishers if item["topic"] == topic_name)

	def get_subscriber_topic(self, topic_name):
		return next(item for item in self.subscribers if item["topic"] == topic_name)


	##################################################
	### Getters And Setters
	##################################################

	def add_way_point(self, coordinates, station_keep_time=0):
		self.queued_coordinates.append(coordinates)
		self.queued_station_keep_times.append(station_keep_time)

		if self.navigation_indicator == False:
			self.set_next_waypoint()


    ##################################################
	### Navigation Controller
	##################################################

	def calculate_distance(self, current_gps_coords, desired_gps_coords):
	    r = 6371 #radius of Earth, km

	    lat1_in, lon1_in = self.current_gps_coords
	    lat2_in, lon2_in = self.desired_gps_coords

	    #convert to coodinates to radians
	    lat1,lon1,lat2,lon2 = map(radians, [lat1_in,lon1_in,lat2_in,lon2_in])

	    # Calculate distance between GPS coordinates (Haversine Formula)
	    deltalon = lon2 - lon1
	    deltalat = lat2 - lat1
	    alpha = 2*asin( sqrt( (sin(abs(deltalat)/2))**2 + cos(lat1)*cos(lat2)*((sin(abs(deltalon)/2))**2) ) )
	    distance = alpha * r * 1000 # Distance in meters

	    return distance

	def caclulate_rotation(self, current_gps_coords, desired_gps_coords, heading):
	    lat1, lon1 = current_gps_coords
	    lat2, lon2 = desired_gps_coords

	    deltalon = lon2 - lon1
	    deltalat = lat2 - lat1

	    #OKAY NOW turn angle calc
	    x = cos(lat2)*sin(deltalon)
	    y = cos(lat1)*sin(lat2) - sin(lat1)*cos(lat2)*cos(deltalon)
	    bearing = degrees(atan2(y,x)) #Measured from East (-180,180) ccw+ cw-


	    theta = heading - bearing #theta is going to be turn angle

	    return theta


	##################################################
	### Subscriber Manager Functions
	##################################################

	def distance_manager(self, gps_data):

		self.check_station_keep_timer()

		if self.navigation_indicator == True:

		    current_coordinates = [gps_data.latitude, gps_data.longitude]

		    self.distance = self.calculate_distance([gps_data.latitude, gps_data.longitude], self.desired_coordinates) #Distance in meters

		    thrust_values = {}

		    q3 = self.get_publisher_topic("q3_thruster_input")
		    q4 = self.get_publisher_topic("q4_thruster_input")

		    thrust_values["q3"] = 0
	        thrust_values["q4"] = 0	



		    if 90 > self.degree_offset > 30 or -90 < self.degree_offset < -180:

		        thrust_values["q3"] = -0.75*max_thrust
		        thrust_values["q4"] = 0.75*max_thrust

		    elif -90 < self.degree_offset < -30 or 90 < self.degree_offset < 180:

		        thrust_values["q3"] = 0.75*max_thrust
		        thrust_values["q4"] = -0.75*max_thrust

		    elif self.distance > 20:
		        left_thrust = 0.75*max_thrust - self.degree_offset / 2
		        right_thrust = 0.75*max_thrust + self.degree_offset / 2

		        if left_thrust >= 0.90*max_thrust: left_thrust = 0.90*max_thrust
		        if left_thrust <= -0.90*max_thrust: left_thrust = -0.90*max_thrust
		        if right_thrust >= 0.90*max_thrust: right_thrust = 0.90*max_thrust
		        if right_thrust <= -0.90*max_thrust: right_thrust = -0.90*max_thrust


		        thrust_values["q3"] = left_thrust
		        thrust_values["q4"] = right_thrust

		        if 90 < self.degree_offset < 180 or -90 < self.degree_offset < -180:
		        	thrust_values["q3"] = -1 * left_thrust
		        	thrust_values["q4"] = -1 * right_thrust


		    elif self.distance > 10:

		    	left_thrust = 0.4*max_thrust - self.degree_offset * 2
	        	right_thrust = 0.4*max_thrust + self.degree_offset * 2

	        	if left_thrust >= 0.40*max_thrust: left_thrust = 0.40*max_thrust
		        if left_thrust <= -0.40*max_thrust: left_thrust = -0.40*max_thrust
		        if right_thrust >= 0.40*max_thrust: right_thrust = 0.40*max_thrust
		        if right_thrust <= -0.40*max_thrust: right_thrust = -0.40*max_thrust

		    	if 90 < self.degree_offset < 180 or -90 < self.degree_offset < -180:

		    		thrust_values["q3"] = -1*left_thrust
		        	thrust_values["q4"] = -1*right_thrust
		        else:
			        thrust_values["q3"] = left_thrust
			        thrust_values["q4"] = right_thrust

		    elif self.distance <= 10:

		        thrust_values["q3"] = 0
		        thrust_values["q4"] = 0


		    print(thrust_values)
	        q3["publisher"].publish(thrust_values["q3"])
	        q4["publisher"].publish(thrust_values["q4"])

	        self.print_progress()



	def rotation_manager(self, imu_data):

		self.check_station_keep_timer()

		if self.navigation_indicator == True and len(self.current_coordinates)==2:

		    q = imu_data.orientation

		    if self.imu_orientation "ENU":
		    	yaw_rad = atan2(2.0 * (q.y*q.z + q.w*q.x), q.w*q.w - q.x*q.x - q.y*q.y + q.z*q.z)
		    else:
		    	yaw_rad = atan2(2.0 * (q.y*q.z + q.w*q.x), q.w*q.w - q.x*q.x - q.y*q.y + q.z*q.z)
		    	
		    yaw_deg = (yaw_rad+3.14159/2)*-180/3.14159
		    # yaw_deg -= imu_degree_offset_bias # YAW in terms of north, gets larger when going cw

		    # print("yaw_deg: " + str(yaw_deg))

		    yaw_deg += 90
		    yaw_deg *= -1

		    # print("yaw_deg post_process")

		    theta_offset = caclulate_rotation(self.current_coordinates, self.desired_coordinates, yaw_deg)
		    theta_offset += self.imu_degree_offset_bias


		    if theta_offset > 180:
		        theta_offset -= 360
		    elif theta_offset < -180:
		        theta_offset += 360
		    # print(theta_offset)

		    if -5 < theta_offset < 5:
		        self.degree_offset = 0
		        # print("no degree offset")
		    else:
		        self.degree_offset = theta_offset
		        # print(degree_offset)

	    	self.print_progress()



    ##################################################
	### Handle Requests
	##################################################


	def start_navigation(self):
		self.navigation_indicator = True

		coord_len = len(self.queued_coordinates)

		print("Navigating to " + str(coord_len) + " coordinates:\n")

		for i in range(coord_len):
			print("\t" + "GPS: " + str(self.queued_coordinates[i]))
			print("\t" + "Time: " + str(self.queued_station_keep_times[i]) + " minutes")
			print("\n")

		self.set_next_waypoint()


	def stop_navigation(self):
		self.navigation_indicator = False
		print("Stopping Navigation")


	def set_station_keep_timer(self, station_keep_time): # Input time in minutes
		self.station_keep_timer = time.time() + station_keep_time * 60 

	def check_station_keep_timer(self):
		
		if time.time() < self.station_keep_timer:
			self.set_next_waypoint()


	def set_next_waypoint(self):

		if len(self.queued_coordinates) > 0:
			self.desired_coordinates = self.queued_coordinates[0]
			self.set_station_keep_timer(self.queued_station_keep_times[0])

			self.queued_coordinates.pop(0)
			self.queued_station_keep_times.pop(0)

		else:
			self.stop_navigation()

	def print_progress(self):
		print("Current Coordinates: " + str(self.current_coordinates))
		print("Desired Coordinates: " + str(self.desired_coordinates))
		print("\nDistance: " + str(self.distance))
		print("Angle Offset: " + str(self.degree_offset))
		print("\n --------------------------------------------------------- \n\n")


	##################################################
	### ROS Services
	##################################################

	def init_services(self):
		self.way_point_service = rospy.Service('way_point', add_way_point, self.way_point_response_func)

	def way_point_response_func(self, req):
		try:
			lat = req.latitude
			lon = req.longitude
			minutes = req.minutes

			self.add_way_point([lat, lon], minutes)
			print("Recieved Way Point")

			return add_way_pointResponse(recieved=True)
			
		except:
			return add_way_pointResponse(recieved=False)





rospy.init_node("wamv_class")

wamv1 = WAMV_Way_Point()

wamv1.add_way_point([0,0], 5)
wamv1.start_navigation()
wamv1.print_progress()

rospy.spin()