/**
 * \file
 * \brief 
 * \author 
 * \version 0.1
 * \date 
 * 
 * \param[in] 
 * 
 * Subscribes to: <BR>
 *    ° 
 * 
 * Publishes to: <BR>
 *    ° 
 *
 * Description
 *     
 */


//Cpp
#include <sstream>
#include <stdio.h>
#include <vector>
#include <iostream>
#include <stdlib.h>
#include <math.h>

//ROS
#include "ros/ros.h"

#include "geometry_msgs/Pose2D.h"
#include "geometry_msgs/Twist.h"
#include <tf/transform_listener.h>

geometry_msgs::Twist twist;

// Your callback functions here if any. With a reasonable function name...

bool pose1InfoReceived = false;
geometry_msgs::Pose2D pose1;
void pose1CallBack(geometry_msgs::Pose2D pose_msg){
    pose1 = pose_msg;
    pose1InfoReceived = true;
}

bool pose2InfoReceived = false;
geometry_msgs::Pose2D pose2;
void pose2CallBack(geometry_msgs::Pose2D pose_msg){
    pose2 = pose_msg;
    pose2InfoReceived = true;
}

// Take an angle to [-pi,pi[ interval whatever its initial value.
double limit_angle(double a){
    while( a >=  M_PI ) a -= 2*M_PI ;
    while( a <  -M_PI ) a += 2*M_PI ;
    return a ;
}
 
int main (int argc, char** argv)
{

	//ROS Initialization
    ros::init(argc, argv, "node_controller");

    // Define your node handles: YOU NEED AT LEAST ONE !
    ros::NodeHandle nh_glob, nh_loc("~") ;

    // Declare your node's subscriptions and service clients
    // A subscriber looks like this:
    ros::Subscriber subtoRobo1Pose = nh_glob.subscribe<geometry_msgs::Pose2D>("/robot1/pose",1,pose1CallBack) ;
    ros::Subscriber subtoRobo2Pose = nh_glob.subscribe<geometry_msgs::Pose2D>("/robot2/pose",1,pose2CallBack) ;
    // ...

    // Declare you publishers and service servers

    ros::Publisher Cmdpublisher = nh_glob.advertise<geometry_msgs::Twist>("cmd",1) ;
    
    // Intializations may be required here.

    double kd=2.7;
    double kalpha=2.9;
    double d0 = 1;
    double dx, dy, alpha, rho, thetaf;
        
    ros::Rate rate(50);   // Or other rate.
std::cout<<"Hi"<<std::endl;
    while (ros::ok()){
        ros::spinOnce();

        if(pose1InfoReceived && pose2InfoReceived){

            dx = pose1.x - pose2.x;
            dy = pose1.y - pose2.y;
            thetaf=pose2.theta;
            alpha = atan2(dy,dx) - thetaf;
            alpha = limit_angle(alpha);
            rho = dx*cos(thetaf)+dy*sin(thetaf)-d0;
            std::cout<<rho<<std::endl;
            twist.linear.x = kd*rho;
            twist.angular.z = kalpha*alpha;


        }
        Cmdpublisher.publish(twist);


        rate.sleep();
    }
}

