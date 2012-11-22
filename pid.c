#include "pid.h"

static int m_pFactor;
static int m_iFactor;
static int m_dFactor;

static int m_lastValue;
static int m_sumError;

static int m_maxError;
static int m_maxSumError;

void pid_init(int pFactor, int iFactor, int dFactor)
{
    /* Reset controller */
    m_lastValue = 0;
    m_sumError  = 0;

    /* Tuning controller */
    m_pFactor = pFactor;
    m_iFactor = iFactor;
    m_dFactor = dFactor;

    /* Calculate maximum values */
    m_maxError    = MAX_P_TERM / (m_pFactor + 1);
    m_maxSumError = MAX_I_TERM / (m_iFactor + 1);
}

int pid_controller(int reference, int measure)
{
    int error;

    int p_term;
    int i_term;
    int d_term;

    int temp;
    //int ret;

    error = reference - measure;

    if (error > m_maxError) {
        p_term = MAX_P_TERM;
    } else if (error < -m_maxError) {
        p_term = -MAX_P_TERM;
    } else {
        p_term = m_pFactor * error;
    }
    
    temp = m_sumError + error;

    if (temp > m_maxSumError) {
        i_term = MAX_I_TERM;    
        m_sumError = m_maxSumError;
    } else if (temp < -m_maxSumError) {
        i_term = -MAX_I_TERM;    
        m_sumError = -m_maxSumError;
    } else {
        m_sumError = temp;
        i_term = m_iFactor * m_sumError;
    }

    d_term = m_dFactor * (m_lastValue - measure);
    
    m_lastValue = measure;

    return (p_term + i_term + d_term) / SCALING_FACTOR;
}

