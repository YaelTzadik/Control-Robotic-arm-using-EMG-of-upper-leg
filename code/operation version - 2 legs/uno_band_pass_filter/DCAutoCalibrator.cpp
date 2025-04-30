//
// Created by ASUS on 19/01/2025.
//

#include "DCAutoCalibrator.h"
#include <stdlib.h>
DCAutoCalibrator::DCAutoCalibrator()
{
    this->data_arr = new int[MAX_SIZE];
    this->data_arr_next_index = 0;
}

DCAutoCalibrator::~DCAutoCalibrator()
{
    delete[]  this->data_arr;
}

bool DCAutoCalibrator::push(int value)
{
    // if array not full
    if (this->data_arr_next_index < MAX_SIZE) {
        this->data_arr[this->data_arr_next_index] = value;
        this->data_arr_next_index++;
    } else {
        // move every value by 1, find avg and min\max
        int max = 0;
        int min = 1024;
        int long sum = 0;
        for (int i = 0; i < MAX_SIZE - 1; ++i) {
            int val  = data_arr[i + 1];
            this->data_arr[i] = val;
            if (val > max)
                max = val;
            if (val < min)
                min = val;
            sum += val;
        }
        this->data_arr[MAX_SIZE - 1] = value;
        sum += value;

        // if no big changes
        if(max-min < SIGNAL_TRESH)
          { 
            int temp = this->last_detected_dc;
            this->last_detected_dc = sum>>MAX_SIZE_SHIFT; 
            if (abs(temp - this->last_detected_dc) > SIGNAL_TRESH/2)
              return false;
          }
    }
    return true;
}

int DCAutoCalibrator::get_calibrated_dc()
{
    return this->last_detected_dc;
}
