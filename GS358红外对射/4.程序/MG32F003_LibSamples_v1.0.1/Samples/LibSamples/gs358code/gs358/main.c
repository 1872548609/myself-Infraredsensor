/**
 * @file    main.c
 * @brief   GS358 红外对射接收板初版程序入口
 */

#define _MAIN_C_

#include "platform.h"
#include "gs358_app.h"
#include "main.h"

int main(void)
{
    GS358_AppInit();

    while (1)
    {
        GS358_AppProcess();
    }
}
