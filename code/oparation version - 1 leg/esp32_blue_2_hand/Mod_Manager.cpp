//
// Created by ASUS on 29/10/2024.
//

#include "Mod_Manager.h"

int Mod_Manager::processCommand(int cmd)
{
    if (cmd == CMD_BOTH) {
        this->return_to_main_idle();
        return NO_RESPONSE;
    }

    if (this->b_main_loop) //in main loop
    {
        if (cmd == CMD_RIGHT)
            this->main_loop->next();

        else if (cmd == CMD_LEFT) {
            if (this->main_loop->current != e_main_loop::idle) {
                this->move_to_inner_loop(this->main_loop->current);
            }
        }
        return NO_RESPONSE;
    } else //in inner loop
    {
        if (cmd == CMD_LEFT)
            this->inner_loop->next();
        if (cmd == CMD_RIGHT) {
            if (inner_loop->on_end()) {
                this->return_to_main_idle();
                return NO_RESPONSE;
            } else {
                int response = this->get_response();
                this->return_to_main_idle();
                return response;
            }
        }
    }
    return NO_RESPONSE;
}

void Mod_Manager::return_to_main_idle()
{
    this->b_main_loop = true;
    this->main_loop->reset();
    delete this->inner_loop;
    this->inner_loop = nullptr;
}

void Mod_Manager::move_to_inner_loop(int type)
{
    this->b_main_loop = false;
    switch (type) {
        case e_main_loop::quick:
            this->inner_loop = new Loop(quick_loop.size());
            break;
        case e_main_loop::special:
            this->inner_loop = new Loop(special_loop.size());
            break;
        case e_main_loop::numbers:
            this->inner_loop = new Loop(numbers_loop.size());
            break;
    }
}

int Mod_Manager::get_response()
{
    switch (this->main_loop->current) {
        case e_main_loop::quick:
            return quick_loop[this->inner_loop->current];
        case e_main_loop::special:
            return special_loop[this->inner_loop->current];
        case e_main_loop::numbers:
            return numbers_loop[this->inner_loop->current];
    }
    return 0;
}

Mod_Manager::~Mod_Manager()
{
    delete main_loop;
    delete inner_loop;
}


