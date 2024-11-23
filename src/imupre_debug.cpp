#include <nav_msgs/msg/odometry.hpp>
#include <iostream>
#include <rclcpp/rclcpp.hpp>
#include <std_msgs/msg/header.hpp>
#include <tf2/LinearMath/Quaternion.h>
#include <tf2/transform_datatypes.h>
#include <tf2/convert.h>
#include <tf2_geometry_msgs/tf2_geometry_msgs.hpp>
class IMUPreintegrationDebug : public rclcpp::Node
{
    public:
        rclcpp::Subscription<nav_msgs::msg::Odometry>::SharedPtr sub_gt_odom_;
        rclcpp::Publisher<nav_msgs::msg::Odometry>::SharedPtr pub_odom_;
        rclcpp::Publisher<nav_msgs::msg::Odometry>::SharedPtr pub_inc_odom_;

        nav_msgs::msg::Odometry::SharedPtr old_odom_;
        bool init_;

    IMUPreintegrationDebug() : Node("imu_pre_deb")
    {
        init_ = false;
        old_odom_ = std::make_shared<nav_msgs::msg::Odometry>();
        sub_gt_odom_ = create_subscription<nav_msgs::msg::Odometry>(
            "/model/mobile_robot/odometry", 10,
            std::bind(&IMUPreintegrationDebug::subHandler, this, std::placeholders::_1));

        pub_odom_  = create_publisher<nav_msgs::msg::Odometry>("lio_sam/mapping/odometry", 10);
        pub_inc_odom_ = create_publisher<nav_msgs::msg::Odometry>("lio_sam/mapping/odometry_incremental", 10);
    };

    void subHandler(const nav_msgs::msg::Odometry::SharedPtr odomMsg)
    {
        nav_msgs::msg::Odometry inc_odom;
        tf2::Quaternion prev_quat, curr_quat, delta_q;

        if(!init_)
        {
            init_ = true; 
            old_odom_->set__header(odomMsg->header);
            old_odom_->child_frame_id = odomMsg->child_frame_id;
            old_odom_->set__pose(odomMsg->pose);
            old_odom_->set__twist(odomMsg->twist);
        }
        else
        {
            odomMsg->child_frame_id = "odom_mapping";
            pub_odom_->publish(*odomMsg);
            inc_odom.set__header(odomMsg->header);
            inc_odom.child_frame_id = "odom_mapping";
            inc_odom.pose.pose.position.x = odomMsg->pose.pose.position.x - old_odom_->pose.pose.position.x;
            inc_odom.pose.pose.position.y = odomMsg->pose.pose.position.y - old_odom_->pose.pose.position.y;
            inc_odom.pose.pose.position.z = odomMsg->pose.pose.position.z - old_odom_->pose.pose.position.z;

            
            delta_q = prev_quat.inverse() * curr_quat;

            inc_odom.pose.pose.orientation.x = delta_q.x();
            inc_odom.pose.pose.orientation.y = delta_q.y();
            inc_odom.pose.pose.orientation.z = delta_q.z();
            inc_odom.pose.pose.orientation.w = delta_q.w();

            pub_inc_odom_->publish(inc_odom);
            old_odom_->set__header(odomMsg->header);
            old_odom_->child_frame_id = odomMsg->child_frame_id;
            old_odom_->set__pose(odomMsg->pose);
            old_odom_->set__twist(odomMsg->twist);
            
        }
    }
};


int main(int argc, char** argv)
{   
    rclcpp::init(argc, argv);

    rclcpp::executors::MultiThreadedExecutor e;

    auto ImuP = std::make_shared<IMUPreintegrationDebug>();
   
    e.add_node(ImuP);

    RCLCPP_INFO(rclcpp::get_logger("rclcpp"), "\033[1;32m----> IMU Preintegration DEBUG Started.\033[0m");

    e.spin();

    rclcpp::shutdown();
    return 0;
}