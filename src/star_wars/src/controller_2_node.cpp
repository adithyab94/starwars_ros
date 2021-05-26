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
#include <string.h>

//ROS
#include "ros/ros.h"

// Include here the ".h" files corresponding to the topic types you use.
#include "geometry_msgs/Twist.h"
#include <tf/transform_listener.h>


// Your callback functions here if any. With a reasonable function name...
// ...
//
//void myFirstCallback(/* Define here the variable which holds the message */){
//    // ... Callback function code
//}


// Take an angle to [-pi,pi[ interval whatever its initial value.
double limit_angle(double a){
    while( a >=  M_PI ) a -= 2*M_PI ;
    while( a <  -M_PI ) a += 2*M_PI ;
    return a ;
}

int main (int argc, char** argv)
{

        //ROS Initialization
    ros::init(argc, argv, "controller_tf");

    // Define your node handles: YOU NEED AT LEAST ONE !
    ros::NodeHandle nh_glob, nh_loc("~") ;
   std::string destination_frame, original_frame;
   nh_loc.param<std::string>("parentframe",destination_frame,"/robot2/ground");
   nh_loc.param<std::string>("childframe",original_frame,"/robot1/ground");

    // Declare your node's subscriptions and service clients
    // A subscriber looks like this:
    // ros::Subscriber subsName = some_node_handler.subscribe<pacakage::Type>("topic_name_here",buffer_size_often_1,yourCallBackFn) ;
    // ...
   ros::Publisher cmdPub = nh_glob.advertise<geometry_msgs::Twist>("/robot2/cmd",1) ;
    
    // Intializations may be required here.
     tf::TransformListener listener;
    // ...

    double kd = 2.7;
    double kalpha = 2.9;
    double d0 = 1;


    ros::Rate rate(50);   // Or other rate.
    while (ros::ok()){
        ros::spinOnce();
        tf::StampedTransform transform;


        try{
            listener.lookupTransform(destination_frame,original_frame,
            ros::Time(0), transform);
            std::cout<<transform.getOrigin().x()<<std::endl;
            std::cout<<transform.getOrigin().y()<<std::endl;
        }
        catch (tf::TransformException ex){
            ROS_ERROR("%s",ex.what());
            ros::Duration(1.0).sleep();
        }

       geometry_msgs::Twist velocity;
       velocity.angular.z = kalpha * limit_angle(atan2(transform.getOrigin().y(), transform.getOrigin().x()));
       velocity.linear.x = kd * (transform.getOrigin().x() - d0);
       cmdPub.publish(velocity);


        // The code of your node goes here.
        
        rate.sleep();
    }
}

