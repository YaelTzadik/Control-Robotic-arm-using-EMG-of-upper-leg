//
// Created by ASUS on 20/01/2025.
//

#ifndef FROM_19_1_25_PEAKRECOGNIZER_H
#define FROM_19_1_25_PEAKRECOGNIZER_H

// Simple peak detector based on signal rise and fall
class PeakRecognizer {
private:
  // consts
  static const int REACTION_TIME = 30;  // Ticks to wait (~1s at 30Hz)
  static const int UP_TRESH = 20; // Threshold for rising signal
  static const int DOWN_TRESH = 20; // Threshold for falling signal

  // recognize peaks vars
  bool signal_is_up; // True if signal is rising
  int res_press;                 // Peak detection result
  int signal_up_2_down_timer;     //counts how much time from rise to fall of signal
  int reaction_time_sleep_timer;  //count down when the board will not listen to a new signal rise


public:
  PeakRecognizer();
  int push(int value);
  void reset();
};


#endif  //FROM_19_1_25_PEAKRECOGNIZER_H
