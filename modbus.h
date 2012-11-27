#ifndef MODBUS_H
#define MODBUS_H

#include <stdint.h>

#define MB_READ_REG_U(reg)     ((uint16_t) modbus_reg_table [ (reg) ])
#define MB_READ_REG_S(reg)     ((int16_t)  modbus_reg_table [ (reg) ])
#define MB_READ_REG(reg)       MB_READ_REG_U(reg) 
#define MB_WRITE_REG(reg, val) do { modbus_reg_table [ (reg) ] = (val); } while (0)

enum MB_REGISTERS {
    MB_REG_STATUS,
    MB_REG_CONTROL,

    MB_REG_SPEED_REF,
    MB_REG_SPEED_MEAS,

    MB_REG_KFB,

    MB_REG_PID_PROP,
    MB_REG_PID_INT,
    MB_REG_PID_DIF,
    MB_REG_PID_OUTPUT,


    /* This is not a register. Must be last
     */
    MB_REGS_NUMBER
};

extern uint16_t modbus_reg_table[ MB_REGS_NUMBER ];

void modbus_init ( void );
void modbus_fsm  ( void );

#endif /* end of include guard: MODBUS_H */
