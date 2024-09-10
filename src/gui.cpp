//
// Created by Namir Garib on 2024/09/08.
//

#include "gui.h"
#include "virhwm.h"

int main() {
    initializeMemory();

    a_vector = {1, 2, 3, 4, 5, 6, 7, 8};
    b_vector = {2, 2, 1, 1, 2, 2, 1, 1};
    updateMemory();
    microprogramLoop();

    return 0;
}