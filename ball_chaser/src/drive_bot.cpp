#include <ros/ros.h>
#include <geometry_msgs/Twist.h>
#include <ball_chaser/DriveToTarget.h>

ros::Publisher motor_command_publisher;

void stopRobot();
bool handle_drive_request(ball_chaser::DriveToTarget::Request &reqs, ball_chaser::DriveToTarget::Response & resp){
    ROS_INFO("Running Service");
    static int counter = 0;
    geometry_msgs::Twist vel;
    ros::Rate loop_rate(10);
    
        vel.linear.x = reqs.linear_x;
        vel.angular.z = reqs.angular_z;

        resp.msg_feedback = "Moving the robot with linear_x of " + std::to_string(reqs.linear_x ) + " and angular_z of " + std::to_string(reqs.angular_z);
        motor_command_publisher.publish(vel);

        loop_rate.sleep();
    

    if(counter == 10){counter = 0; stopRobot();
    }

    counter++;
    ROS_INFO("End of Service");
}

void stopRobot(){
    geometry_msgs::Twist vel;
    vel.linear.x = 0;
    vel.angular.z = 0;

    motor_command_publisher.publish(vel);
    ros::Duration(0.1).sleep();
}
int main(int argc, char ** argv){
    ros::init(argc, argv, "drive_bot");

    ros::NodeHandle n;

    motor_command_publisher = n.advertise<geometry_msgs::Twist>("/cmd_vel", 10);
    ros::ServiceServer my_server = n.advertiseService("/ball_chaser/command_robot", handle_drive_request);

    ROS_INFO("Drive_bot service ready."); 

    while(ros::ok()){
    ros::spinOnce();
    }
    stopRobot();
    return 0;
}