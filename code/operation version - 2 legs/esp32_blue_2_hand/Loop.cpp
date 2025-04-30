//
// Created by ASUS on 29/10/2024.
//

#include "Loop.h"

Loop::Loop(int length)
{
    this->length = length;
    this->current = this->length-1;
}

void Loop::next()
{
    this->current++;
    if (this->current == this->length)
        this->current = 0;
}

void Loop::reset()
{
    this->current = this->length-1;
}

bool Loop::on_end()
{
    return this->current == (this->length-1);
}
