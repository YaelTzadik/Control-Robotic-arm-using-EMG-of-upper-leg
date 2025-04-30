//
// Created by ASUS on 20/01/2025.
//

#include "BPF.h"


int BPF::push(int value, int dc)
{
    //read input
    this->x[0] = value - dc;  // read the input on analog pin A1 - left

    //low pass filter
    this->y[0] = a[0] * this->y[1] + a[1] *this->y[2] + b[0] * this->x[0] + b[1] * this->x[1] + b[2] * this->x[2];
    //high pass filter
    this->y[0] = this->y[0]; // no high pass filter

    //time step
    this->x[2] = this->x[1];
    this->x[1] = this->x[0];
    this->y[2] = this->y[1];
    this->y[1] = this->y[0];
    return y[0];
}
