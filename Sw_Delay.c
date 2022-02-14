 /******************************************************************************
 *
 * Module: Sw_Delay
 *
 * File Name: Sw_Delay.c
 *
 * Description: Source file for Sw_Delay
 *
 * Author: Youssef El-Nemaky
 ******************************************************************************/

#include "sw_delay.h"

void Delay_Ms(unsigned long long timeInMs)
{
    volatile unsigned long long count = 0;
    while(count++ < (NUMBER_OF_ITERATIONS_PER_ONE_MILI_SECOND * timeInMs) );
}
