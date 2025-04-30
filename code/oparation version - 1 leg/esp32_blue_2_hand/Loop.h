//
// Created by ASUS on 29/10/2024.
//

#ifndef INITAL_ALGORITHEM_LOOP_H
#define INITAL_ALGORITHEM_LOOP_H


class Loop {
    int length;
public:
    int current;
    explicit Loop(int length);
    void next();
    void reset();
    bool on_end();
};


#endif //INITAL_ALGORITHEM_LOOP_H
