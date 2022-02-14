 /******************************************************************************
 *
 * Module: Sw_Delay
 *
 * File Name: Sw_Delay.h
 *
 * Description: Header file for Sw_Delay
 *
 * Author: Youssef El-Nemaky
 ******************************************************************************/

#ifndef SW_DELAY_H
#define SW_DLEAY_H

#define NUMBER_OF_ITERATIONS_PER_ONE_MILI_SECOND 762

/*******************************************************************************
 *                      Function Prototypes                                    *
 *******************************************************************************/
void Delay_Ms(unsigned long long timeInMs);

#endif /* SW_DELAY_H*/