//
// Created by ASUS on 20/01/2025.
//

#ifndef FROM_19_1_25_BPF_H
#define FROM_19_1_25_BPF_H

// BPF class: Band-Pass Filter for processing input signals
class BPF {
private:
    // Filter coefficients for the difference equation
    double a[2] = { 1.64927209, -0.70219636 };  // Feedback coefficients
    double b[3] = { 0.01323107, 0.02646213, 0.01323107 };  // Feedforward coefficients

    // State variables for input and output history
    double x[3] = { 0, 0, 0 }; // Input history: { x[t], x[t-1], x[t-2] }
    double y[3] = { 0, 0, 0 }; // Output history: { y[t], y[t-1], y[t-2] }

public:
    /**
     * @brief Push a new value into the band-pass filter.
     * 
     * @param value Raw input value to filter.
     * @param dc DC offset value to subtract from the input.
     * @return int Filtered output value.
     */
    int push(int value, int dc);
};

#endif // FROM_19_1_25_BPF_H
