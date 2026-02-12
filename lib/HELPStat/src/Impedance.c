#include <Impedance.h>

AppIMPCfg_Type AppIMPCfg;

int32_t AppIMPGetCfg(void *pCfg) {
    *(AppIMPCfg_Type**)pCfg = &AppIMPCfg;
    return AD5940ERR_OK;
}

int32_t AppIMPCtrl(uint32_t Command, void *pPara) {
    switch(Command) {
        case IMPCTRL_START:
            AppIMPCfg.IMPInited = bTRUE;
            AppIMPCfg.StopRequired = bFALSE;
            return AD5940ERR_OK;
        
        case IMPCTRL_STOPNOW:
            AppIMPCfg.StopRequired = bTRUE;
            return AD5940ERR_OK;
        
        case IMPCTRL_STOPSYNC:
            AppIMPCfg.StopRequired = bTRUE;
            return AD5940ERR_OK;
        
        case IMPCTRL_GETFREQ:
            if(pPara) {
                float *pFreq = (float*)pPara;
                *pFreq = AppIMPCfg.FreqofData;
                return AD5940ERR_OK;
            }
            return AD5940ERR_PARA;
        
        default:
            return AD5940ERR_PARA;
    }
}

int32_t AppIMPInit(uint32_t *pBuffer, uint32_t BufferLen) {
    if(!pBuffer || !BufferLen)
        return AD5940ERR_PARA;
    
    AppIMPCfg.IMPInited = bTRUE;
    return AD5940ERR_OK;
}

int32_t AppIMPISR(void *pBuff, uint32_t *pCount) {
    if(!pBuff || !pCount)
        return AD5940ERR_PARA;
    
    *pCount = 0;
    return AD5940ERR_OK;
}

void logSweep(SoftSweepCfg_Type *pSweepCfg, float *pNextFreq) {
    if(!pSweepCfg || !pNextFreq)
        return;
    
    if(++pSweepCfg->SweepIndex >= pSweepCfg->SweepPoints)
        pSweepCfg->SweepEn = bFALSE;
    else {
        float frequency = pSweepCfg->SweepStart * 
            pow(10, (pSweepCfg->SweepIndex * 
            log10(pSweepCfg->SweepStop/pSweepCfg->SweepStart) / 
            (pSweepCfg->SweepPoints-1)));
        *pNextFreq = frequency;
    }
}
