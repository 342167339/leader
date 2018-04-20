#! /usr/bin/env python
# -*- coding=utf-8 -*-
import socket
import struct
import rospy
import time
from geometry_msgs.msg import PoseWithCovarianceStamped,PoseStamped
from nav_msgs.msg import Odometry

#message proto
# id |  length | data
def send_msg(sock, msg ,id):
    # Prefix each message with a 4-byte id and length (network byte order)
    msg = struct.pack('>I',int(id)) + struct.pack('>I', len(msg)) + msg
    sock.sendall(msg)


def odomCallback(msg):
    global odom_socket

    data = ""

    id = msg.header.seq
    print "send id: ",id
    #child_frame_id = msg.child_frame_id
    x = msg.pose.pose.position.x
    y = msg.pose.pose.position.y
    #orientation
    orien_z = msg.pose.pose.orientation.z
    orien_w = msg.pose.pose.orientation.w
    #velocity
    vel_x = msg.twist.twist.linear.x
    vel_w = msg.twist.twist.angular.z
    data += str(x) + "," + str(y)+ "," + str(orien_z)+ "," + str(orien_w) + "," + str(vel_x) + "," + str(vel_w)

    send_msg(odom_socket,data,id)


odom_socket = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
odom_socket.connect(('192.168.1.113',8000))

rospy.init_node('server_node')

rospy.Subscriber('/odom',Odometry,odomCallback)

rospy.spin()
