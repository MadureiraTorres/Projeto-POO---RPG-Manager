/**
 * @file main.cpp
 * @brief RPG Manager — Dark Souls edition.
 *        Entry point do sistema.
 *
 * Compile:
 *   g++ -std=c++17 -Wall -Wextra main.cpp -lsqlite3 -o rpg_manager
 *
 * Run:
 *   ./rpg_manager
 */

#include "src/menu.h"

int main(){
    RPG::Menu menu;
    menu.executar();
    return 0;
}