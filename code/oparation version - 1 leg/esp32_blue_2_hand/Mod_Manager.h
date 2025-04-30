//
// Created by ASUS on 29/10/2024.
//

#ifndef INITAL_ALGORITHEM_MOD_MANAGER_H
#define INITAL_ALGORITHEM_MOD_MANAGER_H

#include "Loop.h";
#include <array>

class Mod_Manager {

private:
  enum e_main_loop {
    idle,
    quick,
    special,
    numbers
  };
  enum e_commands { open,
                    close,
                    cw,
                    counter_cw,
                    rev_two,
                    two,
                    spongeBob,
                    point,
                    peace,
                    xlx,
                    semi_open,
                    semi_close,
                    back };

  std::array<int, 5> quick_loop = { open, close, spongeBob, peace, back };
  std::array<int, 3> special_loop = { peace, xlx, back };
  std::array<int, 5> numbers_loop = { two, rev_two, open, close, back };


  bool b_main_loop = true;
  Loop *main_loop = new Loop(4);
  Loop *inner_loop = nullptr;


public:
  static int const CMD_RIGHT = 1;
  static int const CMD_LEFT = 2;
  static int const CMD_BOTH = 3;
  static int const NO_RESPONSE = -1;

  int processCommand(int cmd);

  ~Mod_Manager();

private:


  void return_to_main_idle();

  void move_to_inner_loop(int i);

  int get_response();
};


#endif  //INITAL_ALGORITHEM_MOD_MANAGER_H
