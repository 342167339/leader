//订阅"/mobile_base/commands/velocity"，获取leader的速度
//通过tf_broadcast发布follower相对与leader的tf变换
//通过tf_listener获取follower相对于odom原点的tf变换
//发布follower_odom话题

#include <ros/ros.h>
//#include <tf/transform.h>
#include <tf/transform_listener.h>
#include <tf/transform_broadcaster.h>
#include <geometry_msgs/PoseStamped.h>
#include <geometry_msgs/Twist.h>
#include <tf2_msgs/TFMessage.h>
#include <nav_msgs/Odometry.h>

#define dis 1
#define ang 135
#define pi 3.1415926

class FollowerBroadcaster{
  public:
  FollowerBroadcaster(){
    sub = n.subscribe("/mobile_base/commands/velocity", 1, &FollowerBroadcaster::callback, this);
  }
  void callback(const geometry_msgs::Twist input);
  bool getVelocity(geometry_msgs::Twist &vel);

  private:
    ros::NodeHandle n;
    ros::Subscriber sub;  
    geometry_msgs::Twist vel;
};

void FollowerBroadcaster::callback(const geometry_msgs::Twist input){
  vel.linear.x = input.linear.x;
  vel.angular.z = input.angular.z;
}

bool FollowerBroadcaster::getVelocity(geometry_msgs::Twist &twist){
  if(!vel.linear.x && !vel.angular.z){
    return false;
  }else{
    twist.linear.x = vel.linear.x;
    twist.angular.z = vel.angular.z;
    //ROS_INFO("angular: %f",twist.angular.z);
    return true;
  }
  return false;
}

int main(int argc, char** argv){
  ros::init(argc, argv, "my_tf_broadcaster");
  ros::NodeHandle n; 
  FollowerBroadcaster FB;  
  ros::Publisher pub = n.advertise<nav_msgs::Odometry>("/follower_odom", 10);
  tf::TransformListener listener;
  tf::StampedTransform transform_listen;
  tf::Transform transform_publish;
  static tf::TransformBroadcaster br;
  geometry_msgs::Twist vel_pub;
  nav_msgs::Odometry follower_odom;

  while(n.ok()){
    ros::Rate rate(10.0);

    transform_publish.setOrigin( tf::Vector3(-1, 1, 0.0) );
    transform_publish.setRotation( tf::Quaternion(0, 0, 0, 1) );
    br.sendTransform(tf::StampedTransform(transform_publish, ros::Time::now(), "base_footprint", "follower_odom"));

    try{
      listener.lookupTransform("odom",  "follower_odom", 
	  //listener.lookupTransform("follower_odom",  "odom", 
                                ros::Time(0), transform_listen);
    }
    catch (tf::TransformException ex){
      ROS_ERROR("%s",ex.what());
      ros::Duration(1.0).sleep();
    }

    follower_odom.header.stamp = ros::Time::now();
    follower_odom.header.frame_id = "odom";
    follower_odom.pose.pose.position.x = transform_listen.getOrigin().x();
    follower_odom.pose.pose.position.y = transform_listen.getOrigin().y();
    follower_odom.pose.pose.orientation.z = transform_listen.getRotation().getZ();
    follower_odom.pose.pose.orientation.w = transform_listen.getRotation().getW();
    
    ros::spinOnce();  

    if(FB.getVelocity(vel_pub)){
      follower_odom.twist.twist.linear.x = vel_pub.linear.x;
      follower_odom.twist.twist.angular.z = vel_pub.angular.z;
    }else{
      follower_odom.twist.twist.linear.x = 0;
      follower_odom.twist.twist.angular.z = 0;
      ROS_INFO("NO velocity command!");
    }

    pub.publish(follower_odom);

    rate.sleep();
  }

  return 0;  
};


