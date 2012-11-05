#include "drive.h"

#define VF_SLOPE 938

u16 drive_vf_control(s16 freq_m)
{
    s16 tmp_amp;

    /* Vf law */
    tmp_amp = freq_m * VF_SLOPE / 256;

    /* absolute value */
    if (tmp_amp < 0)
        tmp_amp = -tmp_amp;

    /* amplitude saturation */
    if (tmp_amp > 1000)
    {
        tmp_amp = 1000;
    } 
    else if (tmp_amp < 100)
    {
        tmp_amp = 100;
    }

    return tmp_amp;
}
