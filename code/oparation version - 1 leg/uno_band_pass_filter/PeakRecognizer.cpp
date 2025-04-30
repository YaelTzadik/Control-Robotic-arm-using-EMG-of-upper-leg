//
// Created by ASUS on 20/01/2025.
//

#include "PeakRecognizer.h"

// Constructor: Initializes internal states
PeakRecognizer::PeakRecognizer()
    : signal_is_up(false),
      signal_up_2_down_timer(0),
      reaction_time_sleep_timer(0)
{
}

// Processes a new incoming value and attempts to detect peaks
int PeakRecognizer::push(int value)
{
    // Detect signal rising edge (value exceeds threshold)
    if ((!signal_is_up) && (value > UP_TRESH) && (this->reaction_time_sleep_timer <= 0)) {
        this->signal_is_up = true;
        this->signal_up_2_down_timer = 0;
    }
    // Detect signal falling edge (value drops below threshold)
    else if (this->signal_is_up && (value < DOWN_TRESH)) {
        this->reaction_time_sleep_timer = REACTION_TIME;

        if (this->signal_up_2_down_timer > 2 * REACTION_TIME)
            this->res_press = this->signal_up_2_down_timer;  // Long press detected
        else
            this->res_press = 1;  // Short press detected

        this->signal_is_up = false;
    }
    else {
        // No significant change; increment timer
        this->signal_up_2_down_timer++;
    }

    // Decrease cooldown timer after a peak
    this->reaction_time_sleep_timer--;

    // Output the result (peak detected or 0) and clear for next cycle
    int temp = this->res_press;
    this->res_press = 0;
    return temp;
}

// Resets the peak recognizer internal states
void PeakRecognizer::reset()
{
    this->signal_is_up = false;
    this->signal_up_2_down_timer = 0;
    this->reaction_time_sleep_timer = REACTION_TIME / 2;  // Half cooldown to prevent immediate re-trigger
}
