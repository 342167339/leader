//订阅leader的odom，再重新发，没什么意义

#include <ros/ros.h>  
#include "std_msgs/String.h"
#include <geometry_msgs/PoseStamped.h>
#include <nav_msgs/Odometry.h>
#include <math.h>

#define pi 3.1415926;

class Publish_follower_odom
{
public:
    Publish_follower_odom(){
        pub = n.advertise<nav_msgs::Odometry>("/follower_odom", 10);
        sub = n.subscribe("/odom", 1, &Publish_follower_odom::callback, this);
    }
    void callback(const nav_msgs::Odometry A);
private:
    ros::NodeHandle n;   
    ros::Publisher pub;  
    ros::Subscriber sub;  
};

void Publish_follower_odom::callback(const nav_msgs::Odometry A)  
{  
  nav_msgs::Odometry follower_odom;

  follower_odom.header.stamp = ros::Time::now();
  follower_odom.header.frame_id = "/odom";
  follower_odom.child_frame_id = "";
  follower_odom.pose.pose.position.x = A.pose.pose.position.x;
  follower_odom.pose.pose.position.y = A.pose.pose.position.y;
  follower_odom.pose.pose.orientation.z = A.pose.pose.orientation.z;
  follower_odom.pose.pose.orientation.w = A.pose.pose.orientation.w;

  pub.publish(follower_odom); 
}  


int main(int argc, char **argv)  
{  
  ros::init(argc, argv, "leader_follower"); 
  Publish_follower_odom PFO;
  
  ros::spin();  
  
  return 0;  
}  
