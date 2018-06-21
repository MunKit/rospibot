#include "ros/ros.h"
//#include "std_msgs/String.h"
#include <std_msgs/UInt8.h>
#include <wiringPi.h>
#include <softPwm.h>

/**
 * This tutorial demonstrates simple receipt of messages over the ROS system.
 */

#define R_DIRECTION1_PIN 22
#define R_DIRECTION2_PIN 23
#define L_DIRECTION1_PIN 24
#define L_DIRECTION2_PIN 25
#define R_SPEED_PIN 27
#define L_SPEED_PIN 28


void vdirectionL(bool direction);
void vdirectionR(bool direction);
void move(bool directL, bool directR, int speedL, int speedR);

void chatterCallback(const std_msgs::UInt8::ConstPtr& msg)
{
  //ROS_INFO("I heard: [%d]", msg->data);
  bool directL = 0;
  bool directR = 0;
  int speedR = 0;
  int speedL = 0;
  if (msg->data == 48)//backward
  {
    directL = TRUE;
    directR = FALSE;
    speedR = 100;
    speedL = 100;
    ROS_INFO("I heard: [%d]", msg->data);
  }
  else if(msg->data == 49)//forward
  {
    directR = TRUE;
    directL = FALSE;
    speedR = 100;
    speedL = 100;
    ROS_INFO("I heard: [%d]", msg->data);
  }
  else if(msg->data == 50)//left
  {
    directR = TRUE;
    directL = TRUE;
    speedR = 100;
    speedL = 100;
    ROS_INFO("I heard: [%d]", msg->data);
  }
  else if(msg->data == 51)//right
  {
    directR = FALSE;
    directL = FALSE;
    speedR = 100;
    speedL = 100;
    ROS_INFO("I heard: [%d]", msg->data);
  }
  else if(msg->data == 52)//stop
  {
    directR = FALSE;
    directL = FALSE;
    speedR = 0;
    speedL = 0;
    ROS_INFO("I heard: [%d]", msg->data);
  }

  move(directL, directR, speedL, speedR);
}

void vdirectionL(bool direction)
{
  digitalWrite(L_DIRECTION1_PIN, direction);
  digitalWrite(L_DIRECTION2_PIN, !direction);
}

void vdirectionR(bool direction)
{
  digitalWrite(R_DIRECTION1_PIN, !direction);
  digitalWrite(R_DIRECTION2_PIN, direction);
}

void move(bool directL, bool directR, int speedL, int speedR)
{
  vdirectionL(directL);
  vdirectionR(directR);
  softPwmWrite (L_SPEED_PIN, speedL);
  softPwmWrite (R_SPEED_PIN, speedR);
}
int main(int argc, char **argv)
{
  /**
   * The ros::init() function needs to see argc and argv so that it can perform
   * any ROS arguments and name remapping that were provided at the command line.
   * For programmatic remappings you can use a different version of init() which takes
   * remappings directly, but for most command-line programs, passing argc and argv is
   * the easiest way to do it.  The third argument to init() is the name of the node.
   *
   * You must call one of the versions of ros::init() before using any other
   * part of the ROS system.
   */
  // init GPIO pin
  wiringPiSetup();
  pinMode (R_DIRECTION1_PIN, OUTPUT);
  pinMode (L_DIRECTION1_PIN, OUTPUT);
  pinMode (R_DIRECTION2_PIN, OUTPUT);
  pinMode (L_DIRECTION2_PIN, OUTPUT);
  softPwmCreate (R_SPEED_PIN, 0, 100);
  softPwmCreate (L_SPEED_PIN, 0, 100);
  ros::init(argc, argv, "robotlistener");

  /**
   * NodeHandle is the main access point to communications with the ROS system.
   * The first NodeHandle constructed will fully initialize this node, and the last
   * NodeHandle destructed will close down the node.
   */
  ros::NodeHandle n;

  /**
   * The subscribe() call is how you tell ROS that you want to receive messages
   * on a given topic.  This invokes a call to the ROS
   * master node, which keeps a registry of who is publishing and who
   * is subscribing.  Messages are passed to a callback function, here
   * called chatterCallback.  subscribe() returns a Subscriber object that you
   * must hold on to until you want to unsubscribe.  When all copies of the Subscriber
   * object go out of scope, this callback will automatically be unsubscribed from
   * this topic.
   *
   * The second parameter to the subscribe() function is the size of the message
   * queue.  If messages are arriving faster than they are being processed, this
   * is the number of messages that will be buffered up before beginning to throw
   * away the oldest ones.
   */
  ros::Subscriber sub = n.subscribe("robotchatter", 1000, chatterCallback);

  /**
   * ros::spin() will enter a loop, pumping callbacks.  With this version, all
   * callbacks will be called from within this thread (the main one).  ros::spin()
   * will exit when Ctrl-C is pressed, or the node is shutdown by the master.
   */
  ros::spin();

  return 0;
}
