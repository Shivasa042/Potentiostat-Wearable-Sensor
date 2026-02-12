/**  
 * @file       ad5940.h
 * @brief      AD5940 library. This file contains all AD5940 library functions. 
 * @author     ADI
 * @date       March 2019
 * @par Revision History:
 * 
 * Copyright (c) 2017-2019 Analog Devices, Inc. All Rights Reserved.
 * 
 * This software is proprietary to Analog Devices, Inc. and its licensors.
 * By using this software you agree to the terms of the associated
 * Analog Devices Software License Agreement.
**/
#ifndef _AD5940_H_
#define _AD5940_H_

#ifdef __cplusplus
extern "C" {
#endif

#include "math.h"
#include "string.h"
#include "stdio.h"

/** @addtogroup AD5940_Library
  * @{
  */

#define CHIPSEL_594X      /**< AD5940 or AD5941 */

/* library version number */
#define AD5940LIB_VER_MAJOR       0    /**< Major number */
#define AD5940LIB_VER_MINOR       2    /**< Minor number */
#define AD5940LIB_VER_PATCH       1    /**< Path number */
#define AD5940LIB_VER   (AD5940LIB_VER_MAJOR<<16)|(AD5940LIB_VER_MINOR<<8)|(AD5940LIB_VER_PATCH)

#define ADI_DEBUG   /**< Comment this line to remove debug info. */

#ifdef ADI_DEBUG
#define ADI_Print printf   /**< Select the method to print out debug message */
#endif

#include <stdint.h>
#include <stdbool.h>

/* Type Definitions */
typedef int32_t AD5940Err;
typedef uint8_t BoolFlag;
typedef uint32_t SInt32OP_Type;

typedef enum {
    SWEEP_DOWN = 0,
    SWEEP_UP = 1
} SoftSweepDir_Type;

/* Status Codes */
#define AD5940ERR_OK        0
#define AD5940ERR_ERROR     1
#define AD5940ERR_PARA      2

/* Boolean Definitions */
#define bTRUE  1
#define bFALSE 0

/* AFE Control Bits */
#define AFECTRL_ALL             0xFFFFFFFF
#define AFECTRL_HSTIAPWR        0x00000001
#define AFECTRL_INAMPPWR        0x00000002
#define AFECTRL_EXTBUFPWR       0x00000004
#define AFECTRL_DACREFPWR       0x00000008
#define AFECTRL_HSDACPWR        0x00000010
#define AFECTRL_SINC2NOTCH      0x00000020
#define AFECTRL_WG              0x00000040
#define AFECTRL_ADCPWR          0x00000080
#define AFECTRL_ADCCNV          0x00000100
#define AFECTRL_DFT             0x00000200
#define AFECTRL_DCBUFPWR        0x00000400
#define AFECTRL_HPREFPWR        0x00000800

/* Switch Matrix Definitions */
#define SWD_OPEN    0
#define SWD_CE0     1
#define SWD_RCAL0   2
#define SWD_SE0     3

#define SWP_OPEN    0
#define SWP_RE0     1
#define SWP_RCAL0   2

#define SWN_OPEN    0
#define SWN_SE0     1
#define SWN_RCAL1   2
#define SWN_SE0LOAD 3

#define SWT_OPEN        0
#define SWT_TRTIA       1
#define SWT_SE0LOAD     2
#define SWT_RCAL1       4
#define SWT_WGCAP       8

/* Math Constant */
#define MATH_PI 3.14159265358979323846f

/* Clock and ADC Rate Definitions */
#define ADCRATE_800KHZ      0
#define ADCRATE_1P6MHZ      1

#define ADCSINC3OSR_2       0
#define ADCSINC3OSR_4       1

#define ADCSINC2OSR_22      0x16
#define ADCSINC2OSR_1333    0x535

#define ADCAVGNUM_2         0
#define ADCAVGNUM_16        0xF

#define ADCPGA_1            0
#define ADCPGA_2            1
#define ADCPGA_4            2

#define ADCMUXP_HSTIA_P     0
#define ADCMUXN_HSTIA_N     0
#define ADCMUXP_VCE0        0
#define ADCMUXN_VSET1P1     0
#define ADCMUXP_VRE0        0
#define ADCMUXN_VREF1P1     0

/* HSRTIA Definitions */
#define HSTIARTIA_200       0
#define HSTIARTIA_1K        1
#define HSTIARTIA_5K        2
#define HSTIARTIA_10K       3
#define HSTIARTIA_20K       4
#define HSTIARTIA_40K       5
#define HSTIARTIA_80K       6
#define HSTIARTIA_160K      7

#define HSTIADERTIA_OPEN    0
#define HSTIADERLOAD_OPEN   0
#define HSTIADERLOAD_0R     0

/* HSTIA Bias Definitions */
#define HSTIABIAS_1P1       0
#define HSTIABIAS_VZERO0    1

/* DFT Definitions */
#define DFTNUM_4096         (0<<4)
#define DFTNUM_8192         (1<<4)
#define DFTNUM_16384        (2<<4)
#define DFTSRC_SINC3        0

/* LPDAC Definitions */
#define LPDAC0              0
#define LPDACVBIAS_12BIT    0
#define LPDACVZERO_6BIT     0
#define LPDACREF_2P5        0
#define LPDACSRC_MMR        0
#define LPDACSW_VBIAS2LPPA  0x01
#define LPDACSW_VBIAS2PIN   0x02
#define LPDACSW_VZERO2LPTIA 0x04
#define LPDACSW_VZERO2PIN   0x08
#define LPDACSW_VZERO2HSTIA 0x10

/* Waveform Generator Definitions */
#define WGTYPE_SIN          0

/* Power Mode Definitions */
#define AFEPWR_HP           1
#define AFEPWR_LP           0
#define AFEBW_250KHZ        0

/* AFEINTC Definitions */
#define AFEINTC_0           0
#define AFEINTC_1           1

#define AFEINTSRC_ALLINT            0xFFFFFFFF
#define AFEINTSRC_DATAFIFOTHRESH   0x00000001
#define AFEINTSRC_DFTRDY           0x00000002
#define AFEINTSRC_ADCRDY           0x00000004
#define AFEINTSRC_SINC2RDY         0x00000008

/* FIFO Definitions */
#define FIFOMODE_FIFO       0
#define FIFOSIZE_4KB        0
#define FIFOSRC_DFT         0

/* ADC/SYS Clock Source Definitions */
#define ADCCLKSRC_HFOSC     0
#define ADCCLKSRC_EXTCLK    1
#define SYSCLKSRC_HFOSC     0
#define SYSCLKSRC_EXTCLK    1

/* GPIO Function Definitions */
#define GP0_INT             0x00000001
#define GP1_SLEEP           0x00000002
#define GP2_SYNC            0x00000004
#define GP3_SYNC            0x00000008
#define GP4_SYNC            0x00000010
#define GP5_SYNC            0x00000020
#define GP6_INT             0x00000040
#define GP7_INT             0x00000080

/* AGPIO Pin Definitions */
#define AGPIO_Pin0          0x00000001
#define AGPIO_Pin1          0x00000002
#define AGPIO_Pin2          0x00000004
#define AGPIO_Pin3          0x00000008
#define AGPIO_Pin4          0x00000010
#define AGPIO_Pin5          0x00000020
#define AGPIO_Pin6          0x00000040
#define AGPIO_Pin7          0x00000080

/* Sleep Key Definitions */
#define SLPKEY_UNLOCK       1
#define SLPKEY_LOCK         0

/* AFE Result Register Definitions */
#define AFERESULT_DFTREAL               0x00002078
#define AFERESULT_DFTIMAGE              0x0000207C
#define AFERESULT_SINC2             0x00002018
#define AFERESULT_TEMP                  0x00002010
#define AFERESULT_ADCRAW               0x0000200C

/* Struct Definitions */
typedef struct {
    uint32_t Dswitch;
    uint32_t Pswitch;
    uint32_t Nswitch;
    uint32_t Tswitch;
} SWMatrixCfg_Type;

typedef struct {
    uint32_t ADCClkDiv;
    uint32_t ADCCLkSrc;
    uint32_t SysClkDiv;
    uint32_t SysClkSrc;
    uint8_t HfOSC32MHzMode;
    uint8_t HFOSCEn;
    uint8_t HFXTALEn;
    uint8_t LFOSCEn;
} CLKCfg_Type;

typedef struct {
    uint8_t FIFOEn;
    uint8_t FIFOMode;
    uint32_t FIFOSize;
    uint32_t FIFOSrc;
    uint32_t FIFOThresh;
} FIFOCfg_Type;

typedef struct {
    uint32_t FuncSet;
    uint32_t InputEnSet;
    uint32_t OutputEnSet;
    uint32_t OutVal;
    uint32_t PullEnSet;
} AGPIOCfg_Type;

typedef struct {
    float Real;
    float Image;
} fImpCar_Type;

typedef struct {
    float Magnitude;
    float Phase;
} fImpPol_Type;

typedef struct {
    uint32_t ADCMuxP;
    uint32_t ADCMuxN;
    uint32_t ADCPga;
} ADCBaseCfg_Type;

typedef struct {
    uint32_t ADCRate;
    uint32_t ADCAvgNum;
    uint32_t ADCSinc2Osr;
    uint32_t ADCSinc3Osr;
    uint8_t BpSinc3;
    uint8_t BpNotch;
    uint8_t Sinc2NotchEnable;
} ADCFilterCfg_Type;

typedef struct {
    uint32_t DftNum;
    uint32_t DftSrc;
    uint8_t HanWinEn;
} DFTCfg_Type;

typedef struct {
    uint32_t ExcitBufGain;
    uint32_t HsDacGain;
    uint32_t HsDacUpdateRate;
} HSDACCfg_Type;

typedef struct {
    uint32_t HstiaRtiaSel;
    uint32_t HstiaBias;
    uint32_t HstiaCtia;
    uint32_t HstiaDeRload;
    uint32_t HstiaDeRtia;
    uint8_t DiodeClose;
} HSTiaCfg_Type;

typedef struct {
    HSDACCfg_Type HsDacCfg;
    HSTiaCfg_Type HsTiaCfg;
    SWMatrixCfg_Type SWMatCfg;
    struct {
        uint32_t WgType;
        uint8_t GainCalEn;
        uint8_t OffsetCalEn;
        struct {
            uint32_t SinFreqWord;
            uint32_t SinAmplitudeWord;
            uint32_t SinOffsetWord;
            uint32_t SinPhaseWord;
        } SinCfg;
    } WgCfg;
} HSLoopCfg_Type;

typedef struct {
    ADCBaseCfg_Type ADCBaseCfg;
    ADCFilterCfg_Type ADCFilterCfg;
    uint8_t dummy[20];
    DFTCfg_Type DftCfg;
    uint8_t StatCfg[20];
    uint8_t ADCDigCompCfg[20];
} DSPCfg_Type;

typedef struct {
    uint8_t HpBandgapEn;
    uint8_t Hp1V1BuffEn;
    uint8_t Hp1V8BuffEn;
    uint8_t Disc1V1Cap;
    uint8_t Disc1V8Cap;
    uint8_t Hp1V8ThemBuff;
    uint8_t Hp1V8Ilimit;
    uint8_t Lp1V1BuffEn;
    uint8_t Lp1V8BuffEn;
    uint8_t LpBandgapEn;
    uint8_t LpRefBufEn;
    uint8_t LpRefBoostEn;
} AFERefCfg_Type;

typedef struct {
    uint32_t LpAmpPwrMod;
    uint8_t LpPaPwrEn;
    uint8_t LpTiaPwrEn;
    uint32_t LpTiaRf;
    uint32_t LpTiaRload;
    uint32_t LpTiaRtia;
    uint32_t LpTiaSW;
} LPAmpCfg_Type;

typedef struct {
    uint32_t LpdacSel;
    uint32_t LpDacVbiasMux;
    uint32_t LpDacVzeroMux;
    uint32_t LpDacRef;
    uint32_t LpDacSrc;
    uint8_t PowerEn;
    uint16_t DacData12Bit;
    uint32_t DacData6Bit;
    uint32_t LpDacSW;
    uint8_t DataRst;
} LPDACCfg_Type;

typedef struct {
    uint32_t DataType;
    uint32_t DftSrc;
    uint32_t DataCount;
    uint32_t ADCSinc2Osr;
    uint32_t ADCSinc3Osr;
    uint32_t ADCAvgNum;
    float RatioSys2AdcClk;
} ClksCalInfo_Type;

typedef struct {
    uint32_t ADCSinc2Osr;
    uint32_t ADCSinc3Osr;
    uint32_t DftNum;
    uint32_t DftSrc;
} FreqParams_Type;

typedef struct {
    float fFreq;
    float fRcal;
    float SysClkFreq;
    float AdcClkFreq;
    HSTiaCfg_Type HsTiaCfg;
    uint32_t ADCSinc2Osr;
    uint32_t ADCSinc3Osr;
    DFTCfg_Type DftCfg;
    uint8_t bPolarResult;
} HSRTIACal_Type;

typedef struct {
    uint32_t AdcClkFreq;
    uint32_t ADCSinc2Osr;
    uint32_t ADCSinc3Osr;
    uint32_t SysClkFreq;
    uint32_t TimeOut10us;
    float VRef1p11;
    float VRef1p82;
    uint32_t PGACalType;
    uint32_t ADCPga;
} ADCPGACal_Type;

#define PGACALTYPE_OFFSETGAIN 0
#define DATATYPE_DFT 0

#define LPAMPPWR_NORM 0
#define LPTIARF_1M 0
#define LPTIARLOAD_100R 0
#define LPTIARTIA_OPEN 0

#define LPTIASW(x) (1<<x)

/* System Clock Definition */
#define SYSCLCK 16000000

/* AD5940 Register Definitions */
#define REG_AFECON                       0x00002000
#define REG_SEQCON                       0x00002004
#define REG_FIFOCON                      0x00002008
#define REG_SWCON                        0x0000200C
#define REG_HSDACCON                     0x00002010
#define REG_WGCON                        0x00002014
#define REG_ADCFILTERCON                 0x00002044
#define REG_HSDACDAT                     0x00002048
#define REG_SYNCEXTDEVICE                0x00002054
#define REG_SEQCRC                       0x00002060
#define REG_SEQCNT                       0x00002064
#define REG_SEQTIMEOUT                   0x00002068
#define REG_DATAFIFORD                   0x0000206C
#define REG_CMDFIFOWRITE                 0x00002070
#define REG_ADCDAT                       0x00002074
#define REG_DFTREAL                      0x00002078
#define REG_DFTIMAG                      0x0000207C
#define REG_LPTIASW0                     0x000020E4
#define REG_LPTIACON0                    0x000020EC
#define REG_HSRTIACON                    0x000020F0
#define REG_DE0RESCON                    0x000020F8
#define REG_HSTIACON                     0x000020FC
#define REG_DSWFULLCON                   0x00002150
#define REG_NSWFULLCON                   0x00002154
#define REG_PSWFULLCON                   0x00002158
#define REG_TSWFULLCON                   0x0000215C
#define REG_DFTCON                       0x000020D0
#define REG_AFECON_CLKCON0               0x00000408
#define REG_AFECON_CLKEN1                0x00000410

/* ============================================================================================================================
        AFE Clock Configuration
   ============================================================================================================================ */
#define ADCCLKDIV_1         (0<<6)
#define ADCCLKDIV_2         (1<<6)
#define ADCCLKDIV_4         (2<<6)
#define ADCCLKDIV_8         (3<<6)
#define ADCCLKDIV_16        (4<<6)
#define ADCCLKDIV_32        (5<<6)

#define SYSCLKDIV_1         0
#define SYSCLKDIV_2         1
#define SYSCLKDIV_4         2
#define SYSCLKDIV_8         3
#define SYSCLKDIV_16        4

/* RTIA Values */
#define RTIA_FLOAT          0
#define RTIA_INT_200        1
#define RTIA_INT_1K         2
#define RTIA_INT_5K         3
#define RTIA_INT_10K        4
#define RTIA_INT_20K        5
#define RTIA_INT_40K        6
#define RTIA_INT_80K        7
#define RTIA_INT_160K       8

/* ============================================================================================================================
        Function Prototypes
   ============================================================================================================================ */

typedef struct
{
  uint32_t SeqId;
  uint32_t SeqLen;
  uint32_t SeqAddrStart;
} SEQInfo_Type;

typedef struct
{
  BoolFlag bEnable;
  BoolFlag SweepEn;
  BoolFlag SweepLog;
  float SweepStart;
  float SweepStop;
  uint32_t SweepPoints;
  SoftSweepDir_Type SweepDir;
  uint32_t SweepIndex;
}SoftSweepCfg_Type;

/* Function prototypes */
void AD5940_Initialize(void);
void AD5940_HWReset(void);
uint32_t AD5940_MCUResourceInit(void);
void AD5940_CLKCfg(CLKCfg_Type *pCfg);
void AD5940_FIFOCfg(FIFOCfg_Type *pCfg);
void AD5940_INTCCfg(uint32_t ulINTC, uint32_t ulINTSrc, uint8_t bEnable);
void AD5940_INTCClrFlag(uint32_t ulFlag);
uint32_t AD5940_GetMCUIntFlag(void);
uint32_t AD5940_ClrMCUIntFlag(void);
void AD5940_AGPIOCfg(AGPIOCfg_Type *pCfg);
void AD5940_SleepKeyCtrlS(uint32_t iSleepKey);
void AD5940_HSDacCfgS(HSDACCfg_Type *pCfg);
void AD5940_HSRTIACfgS(uint32_t iRtia);
void AD5940_HSLoopCfgS(HSLoopCfg_Type *pCfg);
void AD5940_DSPCfgS(DSPCfg_Type *pCfg);
void AD5940_SWMatrixCfgS(SWMatrixCfg_Type *pCfg);
void AD5940_REFCfgS(AFERefCfg_Type *pCfg);
void AD5940_LPAMPCfgS(LPAmpCfg_Type *pCfg);
void AD5940_LPDACCfgS(LPDACCfg_Type *pCfg);
void AD5940_ADCBaseCfgS(ADCBaseCfg_Type *pCfg);
void AD5940_ADCFilterCfgS(ADCFilterCfg_Type *pCfg);
void AD5940_DFTCfgS(DFTCfg_Type *pCfg);
void AD5940_AFECtrlS(uint32_t ulCtrl, uint8_t bEnable);
void AD5940_AFEPwrBW(uint32_t ulPwrMod, uint32_t ulBandwidth);
void AD5940_HPModeEn(uint8_t bEnable);
void AD5940_Delay10us(uint32_t iTime);
void AD5940_ReadWriteNBytes(uint8_t *pWdata, uint8_t *pRdata, uint32_t Length);
uint32_t AD5940_ReadAfeResult(uint32_t ulResultAddr);
uint32_t AD5940_WGFreqWordCal(float fFreq, float fSysClk);
void AD5940_WGFreqCtrlS(float fFreq, float fSysClk);
uint32_t AD5940_WakeUp(uint32_t iMaxTries);
void AD5940_ShutDownS(void);
float AD5940_ADCCode2Volt(uint32_t iCode, uint32_t iPga, float fVref);
FreqParams_Type AD5940_GetFreqParameters(float fFreq);
void AD5940_ClksCalculate(ClksCalInfo_Type *pCfg, uint32_t *pClks);
void __AD5940_SetDExRTIA(uint32_t iD, uint32_t iDERTIA, uint32_t iDERLoad);
AD5940Err AD5940_HSRtiaCal(HSRTIACal_Type *pCfg, fImpPol_Type *pResult);
int32_t AD5940_INTCTestFlag(uint32_t ulINTC, uint32_t ulFlag);
float AD5940_ComplexMag(fImpCar_Type *pCplx);
float AD5940_ComplexPhase(fImpCar_Type *pCplx);
fImpCar_Type AD5940_ComplexDivFloat(fImpCar_Type *pNumerator, fImpCar_Type *pDenominator);
fImpCar_Type AD5940_ComplexMulFloat(fImpCar_Type *pA, fImpCar_Type *pB);
fImpCar_Type AD5940_ComplexSubFloat(fImpCar_Type *pA, fImpCar_Type *pB);
void AD5940_ADCPowerCtrlS(uint8_t bEnable);
void AD5940_ADCConvtCtrlS(uint8_t bEnable);
void AD5940_StructInit(void *pStruct, uint32_t uStructSize);
int32_t AD5940_ADCPGACal(ADCPGACal_Type *pCfg);
void AD5940_SoftSweepNextFreq(SoftSweepCfg_Type *pSweepCfg, float *pNextFreq);
void AD5940_ADCMuxCfg(uint32_t PosInput, uint32_t NegInput);
void AD5940_AFEPwrUp(void);
void AD5940_AFEPwrDn(void);
void AD5940_IntC(uint32_t iMask, uint8_t bEnable);
void AD5940_ClksEN(uint32_t Clks);
void AD5940_ClksDis(uint32_t Clks);
void AD5940_SEQGenInit(uint32_t SeqMemSize);
void AD5940_SEQInfoCfg(void *pSeqInfo);
void AD5940_SRAMWrite(uint32_t Address, uint32_t *pData, uint32_t DataLen);
void AD5940_SRAMRead(uint32_t Address, uint32_t *pData, uint32_t DataLen);
void AD5940_WriteReg(uint16_t RegAddress, uint32_t RegValue);
uint32_t AD5940_ReadReg(uint16_t RegAddress);
void AD5940_ModifyReg(uint16_t RegAddress, uint32_t Mask, uint32_t RegValue);

#ifdef __cplusplus
}
#endif

#endif
