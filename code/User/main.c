#define __MAIN_C_
#include "main.h"

int main(void)
{
    IntDisable();

    BspInit();

    TinyOSInit();

    AppInit();

    TinyOSStart();

    IntEnable();

    return 0;
}

