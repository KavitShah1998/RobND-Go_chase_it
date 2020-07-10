#include <ros/ros.h>
#include "ball_chaser/DriveToTarget.h"
#include <sensor_msgs/Image.h>
#include <cv_bridge/cv_bridge.h>
#include <sensor_msgs/image_encodings.h>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/highgui/highgui.hpp>

using namespace cv;
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

    cv_bridge::CvImagePtr cv_ptr;

    try{
        cv_ptr = cv_bridge::toCvCopy(img, sensor_msgs::image_encodings::BGR8);
    }
    catch(cv_bridge::Exception & e){
        ROS_ERROR("cv_bridge exception: %s", e.what());
    }

    for(int row=0; row < cv_ptr->image.rows ; row++){
        for(int col=0; col < cv_ptr->image.cols ; col++){
            Vec3b pixel = cv_ptr->image.at<Vec3b>(row,col);

            if(pixel.val[0] == white_pixel && pixel.val[1] == white_pixel && pixel.val[2] == white_pixel) {
                // decide if i is in |Left|Middle|Right| of image
                //int col = j%(cv_ptr->image.cols);
                spotted = true;
                if (col > cv_ptr->image.cols*1/3 && col < cv_ptr->image.cols*2/3) // Go_Straight
                {
                    drive_robot(0.3, 0);

                    break;
                }
                else if(col < cv_ptr->image.cols*1/3){  // Go_Left
                    drive_robot(0, 0.1);

                    break;
                }
                else{ // Go_Right
                    drive_robot(0, -0.1);

                    break;
                }
                
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
    ros::Subscriber sub1 = n.subscribe("/camera/rgb/image_raw", 1, process_image_callback);

    ros::spin();

    drive_robot(0.0 , 0.0);
    return 0;
}
