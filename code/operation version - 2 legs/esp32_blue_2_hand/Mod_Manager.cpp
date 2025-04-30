//
// Created by ASUS on 29/10/2024.
//

#include "Mod_Manager.h"

int Mod_Manager::processCommand(int cmd)
{
    if (cmd >= CMD_SUPER_LONG) {
        this->main_loop->reset();
        return NO_RESPONSE;
    }

    if (cmd == CMD_SHORT)
        this->main_loop->next(); //next
    else if (cmd >= CMD_LONG) {
        if (this->main_loop->on_end()) {
            return NO_RESPONSE; //activate nothing
        } else {
            int response = quick_loop[this->main_loop->current];
            this->main_loop->reset();
            return response;  //activate something
        }
    }

    return NO_RESPONSE;
}



Mod_Manager::~Mod_Manager()
{
    delete main_loop;
}

Mod_Manager::Mod_Manager()
{
    this->main_loop = new Loop(quick_loop.size());
}


