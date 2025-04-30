//
// Created by ASUS on 19/01/2025.
//
#ifndef FROM_19_1_25_DCAUTOCALIBRATOR_H
#define FROM_19_1_25_DCAUTOCALIBRATOR_H


class DCAutoCalibrator {

private:
  static const int REACTION_TIME = 30;
  static const int DEFAULT_DC = 30;
  static const int SIGNAL_TRESH = 80;
  int last_detected_dc = DEFAULT_DC;

  static const int MAX_SIZE_SHIFT = 7;              // Maximum allowed size
  static const int MAX_SIZE = 1 << MAX_SIZE_SHIFT;  // Maximum allowed size

  int* data_arr;
  int data_arr_next_index;


public:

  DCAutoCalibrator();

  ~DCAutoCalibrator();

  bool push(int value);

  int get_calibrated_dc();
};


#endif  //FROM_19_1_25_DCAUTOCALIBRATOR_H
