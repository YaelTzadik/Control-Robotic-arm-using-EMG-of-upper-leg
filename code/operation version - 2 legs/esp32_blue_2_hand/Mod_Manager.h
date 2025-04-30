//
// Created by ASUS on 29/10/2024.
//

#ifndef INITAL_ALGORITHEM_MOD_MANAGER_H
#define INITAL_ALGORITHEM_MOD_MANAGER_H
#include <array>
#include "Loop.h"
class Mod_Manager {

private:

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
  Loop* main_loop;

public:
  static int const CMD_SHORT = 1;
  static int const CMD_LONG = 2;
  static int const CMD_SUPER_LONG = 150;
  static int const NO_RESPONSE = -1;

  Mod_Manager();
  int processCommand(int cmd);
  ~Mod_Manager();
};


#endif  //INITAL_ALGORITHEM_MOD_MANAGER_H
