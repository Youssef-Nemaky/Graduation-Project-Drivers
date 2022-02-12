 /******************************************************************************
 *
 * Module: I2c
 *
 * File Name: I2c.c
 *
 * Description: Source file for TM4C123GH6PM Microcontroller - I2c Driver
 *
 * Author: Youssef El-Nemaky
 ******************************************************************************/

#include "I2c.h"
#include "I2c_Regs.h"

#ifndef F_CPU
#define F_CPU (16000000U)
#endif

STATIC const I2c_ConfigModule * I2c_ModulePtr = NULL_PTR;

/************************************************************************************
* Service Name: I2c_Init
* Service ID[hex]: 0x00
* Sync/Async: Synchronous
* Reentrancy: Non Reentrant
* Parameters (in): ConfigPtr - Pointer to post-build configuration data
* Parameters (inout): None
* Parameters (out): None
* Return value: None
* Description: Function to Initialize the I2c module.

*** Note ***: The only supported modes at the moment are STANDARD speed mode and MASTER mode only. 
************************************************************************************/
void I2c_Init(const I2c_ConfigType * ConfigPtr)
{
    /* Variables Declrations */
    uint8 i2cCounter = 0;
    uint32 masterTimerPerioidValue = 0;
    volatile uint32 * i2cModuleBasePtr = NULL_PTR;

    I2c_ModulePtr = (ConfigPtr->I2cModules);

    /* Loop through the four I2C Modules */
    for(i2cCounter = 0; i2cCounter < NUMBER_OF_I2C_MODULES; i2cCounter++){
        if( (I2c_ModulePtr->i2c_status_mode) == I2C_DISABLED){
            /* I2C Module is not used so skip it */
            continue;
        }

        switch (i2cCounter)
        {
        case I2C_MODULE_0:
            i2cModuleBasePtr = (volatile uint32 *)I2C_0_BASE_ADDRESS;
            break;
        case I2C_MODULE_1:
            i2cModuleBasePtr = (volatile uint32 *)I2C_1_BASE_ADDRESS;
            break;
        case I2C_MODULE_2:
            i2cModuleBasePtr = (volatile uint32 *)I2C_2_BASE_ADDRESS;
            break;
        case I2C_MODULE_3:
            i2cModuleBasePtr = (volatile uint32 *)I2C_3_BASE_ADDRESS;
            break;
        default:
            break;
        }

        /* Enable the clock for the i2c module */
        SET_BIT(I2C_RCGC_REG, i2cCounter);
        
        if( (I2c_ModulePtr->i2c_master_slave_mode) == I2C_MASTER_MODE){
            /* Master Mode */
            SET_BIT(*(volatile uint32 *)((volatile uint8 *)i2cModuleBasePtr + I2C_MASTER_CONFIG_REG_OFFSET), I2C_MFE_BIT);
            CLEAR_BIT(*(volatile uint32 *)((volatile uint8 *)i2cModuleBasePtr + I2C_MASTER_CONFIG_REG_OFFSET), I2C_SFE_BIT);
        } else {
            /* Slave Mode */
            SET_BIT(*(volatile uint32 *)((volatile uint8 *)i2cModuleBasePtr + I2C_MASTER_CONFIG_REG_OFFSET), I2C_SFE_BIT);
            CLEAR_BIT(*(volatile uint32 *)((volatile uint8 *)i2cModuleBasePtr + I2C_MASTER_CONFIG_REG_OFFSET), I2C_MFE_BIT);
        }
        
        /* Set the frequency of I2C */
        /* (1 + TIME_PERIOD ) = SYS_CLK /(2 * ( SCL_LP + SCL_HP ) * I2C_CLK_Freq ) */
        switch (I2c_ModulePtr->i2c_speed_mode)
        {
        case I2C_STANDARD_SPEED_MODE:
            masterTimerPerioidValue = (((1.0f/I2C_STANDARD_MODE_FREQUENCY) / ( (SCL_LP+SCL_HP) * (1.0f/ F_CPU) )) / 2 ) - 1;
            *(volatile uint32 *)((volatile uint8 *)i2cModuleBasePtr + I2C_MASTER_TIMER_PERIOD_REG_OFFSET) = masterTimerPerioidValue;
            break;
        case I2C_FAST_SPEED_MODE:
            break;
        case I2C_FAST_SPEED_PLUS_MODE:
            break;
        case I2C_HIGH_SPEED_MODE:
            break;
        default:
            break;
        }
    }
}

