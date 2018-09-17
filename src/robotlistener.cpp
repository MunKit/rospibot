#include "ros/ros.h"
//#include "std_msgs/String.h"
#include <std_msgs/UInt8.h>
#include <std_msgs/Int16.h>
#include <wiringPi.h>
#include <softPwm.h>
#include <time.h>
#include <sys/time.h>
/**
 * This tutorial demonstrates simple receipt of messages over the ROS system.
 */

#define R_DIRECTION1_PIN 22
#define R_DIRECTION2_PIN 23
#define L_DIRECTION1_PIN 24
#define L_DIRECTION2_PIN 25
#define R_SPEED_PIN 27
#define L_SPEED_PIN 28
#define BASE_SPEED 100//100
int preError = 0;
int accError = 0;
void vdirectionL(bool direction);
void vdirectionR(bool direction);
void move(bool directL, bool directR, int speedL, int speedR);
struct timeval stop1, start1;
int tooggle = 0;
void chatterCallback(const std_msgs::Int16::ConstPtr& msg)
{
  //ROS_INFO("I heard: [%d]", msg->data);
  bool directL = FALSE;
  bool directR = TRUE;
  int speedR = 0;
  int speedL = 0;
  int delay = 0;
  /*struct timeval stop, start;
  gettimeofday(&stop, NULL);
  printf("%lu", msg->data);
  printf("interval took %lu\n", stop.tv_usec - msg->data*100);
  /*if (tooggle == 0){
	gettimeofday(&start1, NULL);
        tooggle = 1;
  }
  else{
	tooggle = 0;
	gettimeofday(&stop1, NULL);
	printf("interval took %lu\n", stop1.tv_usec - start1.tv_usec);
  }*/
  //unsigned int tim = millis();
  /*if (msg->data == 48)//backward
  {
    directL = TRUE;
    directR = FALSE;
    speedR = 60;
    speedL = 60;
    delay = 1000000;
    ROS_INFO("I heard: [%d]", msg->data);
  }
  else if(msg->data == 49)//forward
  {
    directR = TRUE;
    directL = FALSE;
    speedR = 60;
    speedL = 60;
    delay = 1000000;
    ROS_INFO("I heard: [%d]", msg->data);
  }
  else if(msg->data == 50)//right
  {
    directR = TRUE;
    directL = TRUE;
    speedR = 60;
    speedL = 60;
    delay = 500000;
    ROS_INFO("I heard: [%d]", msg->data);
  }
  else if(msg->data == 51)//left
  {
    directR = FALSE;
    directL = FALSE;
    speedR = 60;
    speedL = 60;
    delay = 500000;
    ROS_INFO("I heard: [%d]", msg->data);
  }
  else if(msg->data == 52)//stop
  {
    directR = FALSE;
    directL = FALSE;
    speedR = 0;
    speedL = 0;
    ROS_INFO("I heard: [%d]", msg->data);
  }*/
  double k = 1;
  double kd = 1;
  double ki = 0.0;
  if (msg->data != 1000){
    std::cout<< msg->data <<std::endl;
    
    speedL = BASE_SPEED - 1.2*(k*msg->data + ki*accError + kd*(msg->data - preError));
    speedR = BASE_SPEED + (k*msg->data + ki*accError + kd*(msg->data - preError));
    preError = msg->data;
    accError += msg->data;

    if (speedL > 255)
	speedL = 255;
    if (speedL < 0 )
	speedL = 0;
    if (speedR > 255)
        speedR = 255;
    if (speedR < 0 )
        speedR = 0;
  }
  else{
    speedL = 0;
    speedR = 0;
  }

  //std::cout<<speedL<<speedR<<std::endl;
  move(directL, directR, speedL, speedR);
  //usleep(delay);
  //move(directL, directR, 0, 0);
  //gettimeofday(&stop, NULL);
  //printf("took %lu\n", stop.tv_usec - start.tv_usec);
  //std::cout<<"time: "<< millis() - tim<<std::endl;
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
  //vdirectionL(directL);
  //vdirectionR(directR);
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
  vdirectionL(FALSE);
  vdirectionR(TRUE);
  softPwmCreate (R_SPEED_PIN, 0, 255);
  softPwmCreate (L_SPEED_PIN, 0, 255);
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
  ros::Subscriber sub = n.subscribe("robotchatter", 20, chatterCallback, ros::TransportHints().tcpNoDelay());

  /**
   * ros::spin() will enter a loop, pumping callbacks.  With this version, all
   * callbacks will be called from within this thread (the main one).  ros::spin()
   * will exit when Ctrl-C is pressed, or the node is shutdown by the master.
   */
  ros::spin();

  return 0;
}
