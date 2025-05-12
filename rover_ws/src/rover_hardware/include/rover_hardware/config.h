#ifndef DIFFDRIVE_ARDUINO_CONFIG_H
#define DIFFDRIVE_ARDUINO_CONFIG_H

#include <string>


struct Config
{
  string left_wheel_name = "";
  string right_wheel_name = "";
  float loop_rate = 30;
  std::string device = "/dev/ttyUSB0";
  int baud_rate = 57600;
  int timeout = 1000;
  int enc_counts_per_rev = 1920;
};


#endif // DIFFDRIVE_ARDUINO_CONFIG_H