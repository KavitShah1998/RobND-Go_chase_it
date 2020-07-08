#include <ros/ros.h>
#include "ball_chaser/DriveToTarget.h"
#include <sensor_msgs/Image.h>

ros::ServiceClient client;

void drive_robot(float lin_x, float ang_z){
   ball_chaser::DriveToTarget obj;

   obj.request.linear_x = lin_x;
   obj.request.angular_z = ang_z; 

   if(client.call(obj)){ 
       ROS_INFO("%s", obj.response.msg_feedback.c_str());
    }
   else { ROS_ERROR("Failed to call service /ball_chaser/command_robot");}
   
}


void process_image_callback(const sensor_msgs::Image img){
    int white_pixel = 255;
    bool spotted{false};
    for(int i=0; i < img.step * img.height; i++){
        if(img.data[i] == white_pixel) {
            // decide if i is in |Left|Middle|Right| of image
            int col = i%(img.width);
            spotted = true;
            if (col > img.width*1/3 && col < img.width*2/3) // Go_Straight
            {
                drive_robot(0.5, 0);

                break;
            }
            else if(col < img.width*1/3){  // Go_Left
                drive_robot(0, 0.1);

                break;
            }
            else{ // Go_Right
                drive_robot(0, -0.5);

                break;
            }
            
        }
    }

    if(spotted==false){drive_robot(0,0);}

}

int main(int argc, char ** argv){

    ros::init(argc, argv, "process_image");

    ros::NodeHandle n;

    client = n.serviceClient<ball_chaser::DriveToTarget>("/ball_chaser/command_robot");
    //client = n.serviceClient<ball_chaser::DriveToTarget>("/ball_chaser/command_robot");
    ros::Subscriber sub1 = n.subscribe("/camera/rgb/image_raw", 10, process_image_callback);

    ros::spin();

    return 0;
}