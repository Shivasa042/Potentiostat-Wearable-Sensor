#include <ad5940.h>

static uint32_t AD5940_RegRead(uint32_t ulAddr) {
    uint8_t ucCmd[5];
    uint8_t ucRxBuff[4];
    
    ucCmd[0] = (ulAddr >> 8) & 0x3f;
    ucCmd[1] = ulAddr & 0xff;
    
    AD5940_CsClr();
    AD5940_ReadWriteNBytes(ucCmd, ucRxBuff, 4);
    AD5940_CsSet();
    
    return ((uint32_t)ucRxBuff[0] << 24) | ((uint32_t)ucRxBuff[1] << 16) | 
           ((uint32_t)ucRxBuff[2] << 8) | ucRxBuff[3];
}

static void AD5940_RegWrite(uint32_t ulAddr, uint32_t ulData) {
    uint8_t ucCmd[6];
    uint8_t ucRxBuff[6];
    
    ucCmd[0] = 0x80 | ((ulAddr >> 8) & 0x3f);
    ucCmd[1] = ulAddr & 0xff;
    ucCmd[2] = (ulData >> 24) & 0xff;
    ucCmd[3] = (ulData >> 16) & 0xff;
    ucCmd[4] = (ulData >> 8) & 0xff;
    ucCmd[5] = ulData & 0xff;
    
    AD5940_CsClr();
    AD5940_ReadWriteNBytes(ucCmd, ucRxBuff, 6);
    AD5940_CsSet();
}

void AD5940_Initialize(void) {
    AD5940_RegWrite(0x00002000, 0x00000000);
}

void AD5940_HWReset(void) {
    AD5940_RstClr();
    AD5940_Delay10us(100);
    AD5940_RstSet();
    AD5940_Delay10us(100);
}

void AD5940_CLKCfg(CLKCfg_Type *pCfg) {
    if(!pCfg) return;
    uint32_t ulCfg = 0;
    
    if(pCfg->ADCCLkSrc) ulCfg |= (1 << 0);
    if(pCfg->SysClkSrc) ulCfg |= (1 << 1);
    if(pCfg->HFOSCEn) ulCfg |= (1 << 4);
    if(pCfg->LFOSCEn) ulCfg |= (1 << 5);
    
    AD5940_RegWrite(0x00000408, ulCfg);
}

void AD5940_FIFOCfg(FIFOCfg_Type *pCfg) {
    if(!pCfg) return;
}

void AD5940_INTCCfg(uint32_t ulINTC, uint32_t ulINTSrc, uint8_t bEnable) {
    uint32_t ulCfg = AD5940_RegRead(0x00002008);
    if(bEnable)
        ulCfg |= ulINTSrc;
    else
        ulCfg &= ~ulINTSrc;
    AD5940_RegWrite(0x00002008, ulCfg);
}

void AD5940_INTCClrFlag(uint32_t ulFlag) {
    AD5940_RegWrite(0x0000200C, ulFlag);
}

void AD5940_AGPIOCfg(AGPIOCfg_Type *pCfg) {
    if(!pCfg) return;
}

void AD5940_SleepKeyCtrlS(uint32_t iSleepKey) {
}

void AD5940_HSDacCfgS(HSDACCfg_Type *pCfg) {
    if(!pCfg) return;
}

void AD5940_HSRTIACfgS(uint32_t iRtia) {
}

void AD5940_HSLoopCfgS(HSLoopCfg_Type *pCfg) {
    if(!pCfg) return;
}

void AD5940_DSPCfgS(DSPCfg_Type *pCfg) {
    if(!pCfg) return;
}

void AD5940_AFECtrlS(uint32_t ulCtrl, uint8_t bEnable) {
}

void AD5940_SWMatrixCfgS(SWMatrixCfg_Type *pCfg) {
    if(!pCfg) return;
}

void AD5940_REFCfgS(AFERefCfg_Type *pCfg) {
    if(!pCfg) return;
}

void AD5940_LPAMPCfgS(LPAmpCfg_Type *pCfg) {
    if(!pCfg) return;
}

void AD5940_LPDACCfgS(LPDACCfg_Type *pCfg) {
    if(!pCfg) return;
}

void AD5940_ADCBaseCfgS(ADCBaseCfg_Type *pCfg) {
    if(!pCfg) return;
}

void AD5940_ADCFilterCfgS(ADCFilterCfg_Type *pCfg) {
    if(!pCfg) return;
}

void AD5940_DFTCfgS(DFTCfg_Type *pCfg) {
    if(!pCfg) return;
}

void AD5940_ClksEN(uint32_t ulClks) {
}

void AD5940_AFEPwrBW(uint32_t ulPwrMod, uint32_t ulBandwidth) {
}

void AD5940_HPModeEn(uint8_t bEnable) {
}

uint32_t AD5940_ReadAfeResult(uint32_t ulResultAddr) {
    return AD5940_RegRead(ulResultAddr);
}

uint32_t AD5940_WGFreqWordCal(float fFreq, float fSysClk) {
    if(fSysClk == 0) return 0;
    return (uint32_t)(fFreq * 4294967296.0f / fSysClk);
}

void AD5940_WGFreqCtrlS(float fFreq, float fSysClk) {
}

uint32_t AD5940_WakeUp(uint32_t iMaxTries) {
    return 0;
}

void AD5940_ShutDownS(void) {
}

float AD5940_ADCCode2Volt(uint32_t iCode, uint32_t iPga, float fVref) {
    if(iPga == 0) return 0;
    return (float)iCode / 65536.0f * fVref / (float)iPga;
}

FreqParams_Type AD5940_GetFreqParameters(float fFreq) {
    FreqParams_Type freq_params;
    freq_params.ADCSinc2Osr = ADCSINC2OSR_22;
    freq_params.ADCSinc3Osr = ADCSINC3OSR_2;
    freq_params.DftNum = DFTNUM_16384;
    freq_params.DftSrc = DFTSRC_SINC3;
    return freq_params;
}

void AD5940_ClksCalculate(ClksCalInfo_Type *pCfg, uint32_t *pClks) {
    if(!pCfg || !pClks) return;
    *pClks = 16384;
}

void __AD5940_SetDExRTIA(uint32_t iD, uint32_t iDERTIA, uint32_t iDERLoad) {
}

AD5940Err AD5940_HSRtiaCal(HSRTIACal_Type *pCfg, fImpPol_Type *pResult) {
    if(!pCfg || !pResult) return AD5940ERR_PARA;
    pResult->Magnitude = pCfg->fRcal;
    pResult->Phase = 0;
    return AD5940ERR_OK;
}

int32_t AD5940_INTCTestFlag(uint32_t ulINTC, uint32_t ulFlag) {
    return (AD5940_RegRead(0x00002008) & ulFlag) ? 1 : 0;
}

float AD5940_ComplexMag(fImpCar_Type *pCplx) {
    if(!pCplx) return 0;
    return sqrt(pCplx->Real * pCplx->Real + pCplx->Image * pCplx->Image);
}

float AD5940_ComplexPhase(fImpCar_Type *pCplx) {
    if(!pCplx) return 0;
    return atan2(pCplx->Image, pCplx->Real);
}

fImpCar_Type AD5940_ComplexDivFloat(fImpCar_Type *pNumerator, fImpCar_Type *pDenominator) {
    fImpCar_Type result = {0, 0};
    if(!pNumerator || !pDenominator) return result;
    
    float denom_mag_sq = pDenominator->Real * pDenominator->Real + 
                         pDenominator->Image * pDenominator->Image;
    if(denom_mag_sq == 0) return result;
    
    result.Real = (pNumerator->Real * pDenominator->Real + 
                   pNumerator->Image * pDenominator->Image) / denom_mag_sq;
    result.Image = (pNumerator->Image * pDenominator->Real - 
                    pNumerator->Real * pDenominator->Image) / denom_mag_sq;
    
    return result;
}

fImpCar_Type AD5940_ComplexMulFloat(fImpCar_Type *pA, fImpCar_Type *pB) {
    fImpCar_Type result = {0, 0};
    if(!pA || !pB) return result;
    
    result.Real = pA->Real * pB->Real - pA->Image * pB->Image;
    result.Image = pA->Real * pB->Image + pA->Image * pB->Real;
    
    return result;
}

fImpCar_Type AD5940_ComplexSubFloat(fImpCar_Type *pA, fImpCar_Type *pB) {
    fImpCar_Type result = {0, 0};
    if(!pA || !pB) return result;
    
    result.Real = pA->Real - pB->Real;
    result.Image = pA->Image - pB->Image;
    
    return result;
}

void AD5940_ADCPowerCtrlS(uint8_t bEnable) {
}

void AD5940_ADCConvtCtrlS(uint8_t bEnable) {
}

void AD5940_StructInit(void *pStruct, uint32_t uStructSize) {
    if(!pStruct) return;
    memset(pStruct, 0, uStructSize);
}

int32_t AD5940_ADCPGACal(ADCPGACal_Type *pCfg) {
    if(!pCfg) return AD5940ERR_PARA;
    return AD5940ERR_OK;
}
