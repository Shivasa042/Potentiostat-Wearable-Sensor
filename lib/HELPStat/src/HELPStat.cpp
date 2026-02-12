/*
    FILENAME: HELPStat.cpp
    AUTHOR: Kevin Alessandro Bautista
    EMAIL: kbautis@purdue.edu

    DISCLAIMER: 
    Linnes Lab code, firmware, and software is released under the MIT License
    (http://opensource.org/licenses/MIT).
    
    The MIT License (MIT)
    
    Copyright (c) 2024 Linnes Lab, Purdue University, West Lafayette, IN, USA
    
    Permission is hereby granted, free of charge, to any person obtaining a copy
    of this software and associated documentation files (the "Software"), to deal
    in the Software without restriction, including without limitation the rights to
    use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies
    of the Software, and to permit persons to whom the Software is furnished to do
    so, subject to the following conditions:
    
    The above copyright notice and this permission notice shall be included in all
    copies or substantial portions of the Software.
    
    THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
    IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
    FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
    AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
    LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
    OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
    SOFTWARE.
*/
#include <HELPStat.h>

HELPStat::HELPStat() {}

AD5940Err HELPStat::AD5940Start(void) {
    uint32_t err = AD5940_MCUResourceInit();
    if(err != 0) return AD5940ERR_ERROR;
    
    /* PIN DISPLAY */
    delay(5000);
    Serial.println("Start sequence successful.");
    Serial.print("SCK: ");
    Serial.println(SCK);
    Serial.print("MOSI: ");
    Serial.println(MOSI);
    Serial.print("MISO: ");
    Serial.println(MISO);
    Serial.print("RESET: ");
    Serial.println(RESET);
    Serial.print("CS: ");
    Serial.println(CS);
    Serial.print("INTERRUPT PIN: ");
    Serial.println(ESP32_INTERRUPT);
    
    return AD5940ERR_OK;

}

int32_t HELPStat::AD5940PlatformCfg(void) {
  CLKCfg_Type clk_cfg;
  FIFOCfg_Type fifo_cfg;
  AGPIOCfg_Type gpio_cfg;

  /* Use hardware reset */
  AD5940_HWReset();
  AD5940_Initialize();
  /* Platform configuration */
  /* Step1. Configure clock */
  clk_cfg.ADCClkDiv = ADCCLKDIV_1;
  clk_cfg.ADCCLkSrc = ADCCLKSRC_HFOSC;
  clk_cfg.SysClkDiv = SYSCLKDIV_1;
  clk_cfg.SysClkSrc = SYSCLKSRC_HFOSC;
  clk_cfg.HfOSC32MHzMode = bFALSE;
  clk_cfg.HFOSCEn = bTRUE;
  clk_cfg.HFXTALEn = bFALSE;
  clk_cfg.LFOSCEn = bTRUE;
  AD5940_CLKCfg(&clk_cfg);
  /* Step2. Configure FIFO and Sequencer*/
  fifo_cfg.FIFOEn = bFALSE;
  fifo_cfg.FIFOMode = FIFOMODE_FIFO;
  fifo_cfg.FIFOSize = FIFOSIZE_4KB;                       /* 4kB for FIFO, The reset 2kB for sequencer */
  fifo_cfg.FIFOSrc = FIFOSRC_DFT;
  fifo_cfg.FIFOThresh = 4;//AppIMPCfg.FifoThresh;        /* DFT result. One pair for RCAL, another for Rz. One DFT result have real part and imaginary part */
  AD5940_FIFOCfg(&fifo_cfg);
  fifo_cfg.FIFOEn = bTRUE;
  AD5940_FIFOCfg(&fifo_cfg);
  
  /* Step3. Interrupt controller */
  AD5940_INTCCfg(AFEINTC_1, AFEINTSRC_ALLINT, bTRUE);   /* Enable all interrupt in INTC1, so we can check INTC flags */
  AD5940_INTCClrFlag(AFEINTSRC_ALLINT);
  AD5940_INTCCfg(AFEINTC_0, AFEINTSRC_DATAFIFOTHRESH, bTRUE); 
  AD5940_INTCClrFlag(AFEINTSRC_ALLINT);

  /* Step4: Reconfigure GPIO */
  gpio_cfg.FuncSet = GP0_INT|GP1_SLEEP|GP2_SYNC;
  gpio_cfg.InputEnSet = 0;
  gpio_cfg.OutputEnSet = AGPIO_Pin0|AGPIO_Pin1|AGPIO_Pin2;
  gpio_cfg.OutVal = 0;
  gpio_cfg.PullEnSet = 0;
  AD5940_AGPIOCfg(&gpio_cfg);
  AD5940_SleepKeyCtrlS(SLPKEY_UNLOCK);  /* Allow AFE to enter sleep mode. */
  return 0;
}

void HELPStat::AD5940ImpedanceStructInit(float startFreq, float endFreq, uint32_t numPoints) {
    AppIMPCfg_Type *pImpedanceCfg;

    AppIMPGetCfg(&pImpedanceCfg);
    /* Step1: configure initialization sequence Info */
    pImpedanceCfg->SeqStartAddr = 0;
    pImpedanceCfg->MaxSeqLen = 512; /* @todo add checker in function */

    pImpedanceCfg->RcalVal = 10000;
    pImpedanceCfg->SinFreq = 60000.0;
    pImpedanceCfg->FifoThresh = 4;
        
    /* Set switch matrix to onboard(EVAL-AD5940ELECZ) dummy sensor. */
    /* Note the RCAL0 resistor is 10kOhm. */
    pImpedanceCfg->DswitchSel = SWD_CE0;
    pImpedanceCfg->PswitchSel = SWP_RE0;
    pImpedanceCfg->NswitchSel = SWN_SE0;
    pImpedanceCfg->TswitchSel = SWT_SE0LOAD;
    
    /* The dummy sensor is as low as 5kOhm. We need to make sure RTIA is small enough that HSTIA won't be saturated. */
    pImpedanceCfg->HstiaRtiaSel = HSTIARTIA_5K;	
    
    /* Configure the sweep function. */
    pImpedanceCfg->SweepCfg.SweepEn = bTRUE;
    /* Getting some quick results with 1 kHz, but set to 10 Hz to see how slow it takes to boot up */
    pImpedanceCfg->SweepCfg.SweepStart = startFreq;	/* Start from 1kHz */
    pImpedanceCfg->SweepCfg.SweepStop = endFreq;		/* Stop at 100kHz */
    // pImpedanceCfg->SweepCfg.SweepPoints = 51;		/* Points is 101 */
    
    /* Using custom points - defaults to downward log sweep */
    if(startFreq > endFreq) pImpedanceCfg->SweepCfg.SweepPoints = (uint32_t)(1.5 + (log10(startFreq) - log10(endFreq)) * (numPoints)) - 1;
    else pImpedanceCfg->SweepCfg.SweepPoints = (uint32_t)(1.5 + (log10(endFreq) - log10(startFreq)) * (numPoints)) - 1;

    /* Print Statement - REMOVE*/
    Serial.println(pImpedanceCfg->SweepCfg.SweepPoints);

    pImpedanceCfg->SweepCfg.SweepLog = bTRUE;

    /* Configure Power Mode. Use HP mode if frequency is higher than 80kHz. */
    pImpedanceCfg->PwrMod = AFEPWR_LP;

    /* Configure filters if necessary */
    pImpedanceCfg->ADCSinc3Osr = ADCSINC3OSR_2;		/* Sample rate is 800kSPS/2 = 400kSPS */
    pImpedanceCfg->DftNum = DFTNUM_16384;
    pImpedanceCfg->DftSrc = DFTSRC_SINC3;
}

int32_t HELPStat::ImpedanceShowResult(uint32_t *pData, uint32_t DataCount) {
  float freq;

  fImpPol_Type *pImp = (fImpPol_Type*)pData;
  AppIMPCtrl(IMPCTRL_GETFREQ, &freq);

  // printf("Freq:%.2f ", freq);
  // printf("Freq, RzMag (ohm), RzPhase (degrees)\n");
  /*Process data*/
  for(int i=0;i<DataCount;i++)
  {
    // printf("RzMag: %f Ohm , RzPhase: %f \n",pImp[i].Magnitude,pImp[i].Phase*180/MATH_PI);
    // printf("%.2f, %f, %f \n", freq, pImp[i].Magnitude,pImp[i].Phase*180/MATH_PI);
    printf("%0.3f, %f, %f, %f, %f, \n", freq, pImp[i].Magnitude,pImp[i].Phase, pImp[i].Magnitude * cos(pImp[i].Phase), -pImp[i].Magnitude * sin(pImp[i].Phase));

  }

  return 0;
}

void HELPStat::AD5940_Main(float startFreq, float endFreq, uint32_t numPoints, uint32_t gainArrSize, calHSTIA* gainArr) {
  uint32_t temp;  

  _gainArrSize = gainArrSize;
  printf("Gain array size: %d\n", _gainArrSize);
  for(uint32_t i = 0; i < _gainArrSize; i++)
  {
    _gainArr[i] = gainArr[i];
    // _arrHSTIA[i] = arrHSTIA[i];
  }

  /* Can also try moving this to .h file */
  uint32_t AppBuff[APPBUFF_SIZE];

  AD5940PlatformCfg();
  AD5940ImpedanceStructInit(startFreq, endFreq, numPoints);
  
  AppIMPInit(AppBuff, APPBUFF_SIZE);    /* Initialize IMP application. Provide a buffer, which is used to store sequencer commands */
  AppIMPCtrl(IMPCTRL_START, 0);          /* Control IMP measurement to start. Second parameter has no meaning with this command. */
  printf("Freq, RzMag (ohm), RzPhase (degrees). Rreal, Rimag\n");

  while(1)
  {
    if(AD5940_GetMCUIntFlag())
    {
      AD5940_ClrMCUIntFlag();
      temp = APPBUFF_SIZE;
      AppIMPISR(AppBuff, &temp);
      ImpedanceShowResult(AppBuff, temp);
    }
  }
}

void HELPStat::configureFrequency(float freq) {
  AD5940Err check = setHSTIA(freq);

  if(check != AD5940ERR_OK) Serial.println("Unable to configure.");
}

void HELPStat::resetSweep(SoftSweepCfg_Type *pSweepCfg, float *pNextFreq) {
  /* Sets the index back to 0 and enables the sweep again */
  pSweepCfg->SweepIndex = 0; 
  pSweepCfg->SweepEn = bTRUE;

  /* Reset frequency back to start frequency */
  *pNextFreq = _startFreq; 
  AD5940_WGFreqCtrlS(_startFreq, SYSCLCK);
}

void HELPStat::runSweep(void) {
  _currentCycle = 0; 
  printf("Total points to run: %d\n", (_numCycles + 1) * _sweepCfg.SweepPoints);
  printf("Set array size: %d\n", ARRAY_SIZE);
  printf("Calibration resistor value: %f\n", _rcalVal);

  for(uint32_t i = 0; i <= _numCycles; i++) {
    AD5940_SleepKeyCtrlS(SLPKEY_LOCK);
    if(_delaySecs) {
      unsigned long prevTime = millis();
      unsigned long currTime = millis();
      printf("Delaying for %d seconds\n", _delaySecs);
      while(currTime - prevTime < _delaySecs * 1000) {
        currTime = millis();
      }
    } 
    unsigned long timeStart = millis();

    if(i > 0){
      if(AD5940_WakeUp(10) > 10) Serial.println("Wakeup failed!");       
       resetSweep(&_sweepCfg, &_currentFreq);
       delay(300);
       _currentCycle++;
    }
    
    configureFrequency(_currentFreq);
    delay(10);

    printf("Cycle %d\n", i);
    printf("Index, Frequency (Hz), DFT Cal, DFT Mag, Rz (Ohms), Rreal, Rimag, Rphase (rads)\n");
    
    while(_sweepCfg.SweepEn == bTRUE) {
      AD5940_DFTMeasure();
      AD5940_AFECtrlS(AFECTRL_HSTIAPWR|AFECTRL_INAMPPWR|AFECTRL_EXTBUFPWR|\
              AFECTRL_WG|AFECTRL_DACREFPWR|AFECTRL_HSDACPWR|\
              AFECTRL_SINC2NOTCH, bTRUE);
      delay(200);
    }

    unsigned long timeEnd = millis(); 
    printf("Time spent running Cycle %d (seconds): %lu\n", i, (timeEnd-timeStart)/1000);
  }
  
  AD5940_ShutDownS();
  printf("All cycles finished.");
  printf("AD594x shutting down.");
}

void HELPStat::calculateResistors() {
  std::vector<float> Z_real;
  std::vector<float> Z_imag;

  for(uint32_t i = 0; i < _sweepCfg.SweepPoints; i++) {
    for(uint32_t j = 0; j <= _numCycles; j++) {
      impStruct eis;
      eis = eisArr[i + (j * _sweepCfg.SweepPoints)];
      Z_real.push_back(eis.real);
      Z_imag.push_back(eis.imag);
    }
  }

  _calculated_Rct = calculate_Rct(_rct_estimate, _rs_estimate, Z_real, Z_imag);
  _calculated_Rs  = calculate_Rs(_rct_estimate, _rs_estimate, Z_real, Z_imag);

  Serial.print("Calculated Rct: ");
  Serial.println(_calculated_Rct);
  Serial.print("Calculated Rs:  ");
  Serial.println(_calculated_Rs);

  return;
}

void HELPStat::print_settings() {
  Serial.println("SETTINGS");
  
  Serial.print("Rct Estimation:  ");
  Serial.println(_rct_estimate);
  Serial.print("Rs Estimation:   ");
  Serial.println(_rs_estimate);
  Serial.print("Rcal:            ");
  Serial.println(_rcalVal);

  Serial.print("Start Frequency: ");
  Serial.println(_startFreq);
  Serial.print("End Frequency:   ");
  Serial.println(_endFreq);
  
  Serial.print("numCycles:       ");
  Serial.println(_numCycles);
  Serial.print("numPoints:       ");
  Serial.println(_numPoints);

  Serial.print("External Gain:   ");
  Serial.println(_extGain);
  Serial.print("DAC Gain:        ");
  Serial.println(_dacGain);

  Serial.print("Bias Voltage:    ");
  Serial.println(_biasVolt);
  Serial.print("Zero Voltage:    ");
  Serial.println(_zeroVolt);

  Serial.print("Delay (s):       ");
  Serial.println(_delaySecs);

  Serial.print("Folder Name:     ");
  Serial.println(_folderName);
  Serial.print("File Name:       ");
  Serial.println(_fileName);
}

void HELPStat::settlingDelay(float freq) {
  unsigned long constDelay = 1000;
  if(freq <= 5) {
    constDelay = 2000;
    delay((unsigned long)(2 * 1000 / freq) + constDelay);
  }
  else delay((constDelay)); 
}

AD5940Err HELPStat::setHSTIA(float freq) {
  HSDACCfg_Type hsdac_cfg;
  FreqParams_Type freq_params;
  ClksCalInfo_Type clks_cal;
  ADCFilterCfg_Type filter_cfg;
  DFTCfg_Type dft_cfg;
  float adcClck; 

  AD5940Err exitStatus = AD5940ERR_ERROR;

  hsdac_cfg.ExcitBufGain = _extGain;
  hsdac_cfg.HsDacGain = _dacGain;

  freq_params = AD5940_GetFreqParameters(freq);

  for(uint32_t i = 0; i < _gainArrSize; i++) {
    if(freq <= _gainArr[i].freq) {
      if(freq >= 80000) {
        hsdac_cfg.HsDacUpdateRate = 0x07;
        AD5940_HSDacCfgS(&hsdac_cfg);
        AD5940_HSRTIACfgS(_gainArr[i].rTIA);
        __AD5940_SetDExRTIA(0, HSTIADERTIA_OPEN, HSTIADERLOAD_0R);
        
        filter_cfg.ADCRate = ADCRATE_1P6MHZ;
        adcClck = 32e6;
        AD5940_HPModeEn(bTRUE);
        exitStatus = AD5940ERR_OK;
        break;
      }
      else {
        hsdac_cfg.HsDacUpdateRate = 0x1B;
        AD5940_HSDacCfgS(&hsdac_cfg);
        AD5940_HSRTIACfgS(_gainArr[i].rTIA);
        __AD5940_SetDExRTIA(0, HSTIADERTIA_OPEN, HSTIADERLOAD_0R);

        filter_cfg.ADCRate = ADCRATE_800KHZ;
        adcClck = 16e6;
        AD5940_HPModeEn(bFALSE);
        exitStatus = AD5940ERR_OK;
        break;
      }
    }
  }

  filter_cfg.ADCAvgNum = ADCAVGNUM_16;
  filter_cfg.ADCSinc2Osr = freq_params.ADCSinc2Osr;
  filter_cfg.ADCSinc3Osr = freq_params.ADCSinc3Osr;
  filter_cfg.BpSinc3 = bFALSE;
  filter_cfg.BpNotch = bTRUE;
  filter_cfg.Sinc2NotchEnable = bTRUE;
  
  dft_cfg.DftNum = freq_params.DftNum;
  dft_cfg.DftSrc = freq_params.DftSrc;
  dft_cfg.HanWinEn = bTRUE;

  AD5940_ADCFilterCfgS(&filter_cfg);
  AD5940_DFTCfgS(&dft_cfg);

  clks_cal.DataType = DATATYPE_DFT;
  clks_cal.DftSrc = freq_params.DftSrc;
  clks_cal.DataCount = 1L<<(freq_params.DftNum+2);
  clks_cal.ADCSinc2Osr = freq_params.ADCSinc2Osr;
  clks_cal.ADCSinc3Osr = freq_params.ADCSinc3Osr;
  clks_cal.ADCAvgNum = 0;
  clks_cal.RatioSys2AdcClk = SYSCLCK/adcClck;
  AD5940_ClksCalculate(&clks_cal, &_waitClcks);
  
  return exitStatus;
}

void HELPStat::AD5940_TDD(calHSTIA *gainArr, int gainArrSize) {
  _gainArrSize = gainArrSize;
  for(uint32_t i = 0; i < _gainArrSize; i++)
    _gainArr[i] = gainArr[i];

  Serial.println("TDD measurement initialized.");
}

void HELPStat::getMagPhase(int32_t real, int32_t image, float *pMag, float *pPhase) {
  *pMag = sqrt((float)real*real + (float)image*image); 
  *pPhase =  atan2(-image, real);
}

void HELPStat::logSweep(SoftSweepCfg_Type *pSweepCfg, float *pNextFreq) {
  float frequency; 

  if(++pSweepCfg->SweepIndex == pSweepCfg->SweepPoints) pSweepCfg -> SweepEn = bFALSE;
  else {
    frequency = pSweepCfg->SweepStart * pow(10, (pSweepCfg->SweepIndex * log10(pSweepCfg->SweepStop/pSweepCfg->SweepStart)/(pSweepCfg->SweepPoints-1)));
    *pNextFreq = frequency;

    AD5940_WGFreqCtrlS(frequency, SYSCLCK);
    configureFrequency(frequency);
  }
}

void HELPStat::getDFT(int32_t* pReal, int32_t* pImage) { 
  *pReal = AD5940_ReadAfeResult(AFERESULT_DFTREAL);
  *pReal &= 0x3ffff;
  if(*pReal&(1<<17)) *pReal |= 0xfffc0000;     

  delay(200); 

  *pImage = AD5940_ReadAfeResult(AFERESULT_DFTIMAGE);
  *pImage &= 0x3ffff;
  if(*pImage&(1<<17)) *pImage |= 0xfffc0000; 
}

void HELPStat::pollDFT(int32_t* pReal, int32_t* pImage) {
  while(!AD5940_GetMCUIntFlag()) {
    delay(1000);
  }
  
  if(AD5940_INTCTestFlag(AFEINTC_1,AFEINTSRC_DFTRDY)) {
    getDFT(pReal, pImage);
    delay(300);
    AD5940_ClrMCUIntFlag();
    AD5940_INTCClrFlag(AFEINTSRC_DFTRDY);
  }
  else Serial.println("Flag not working!");
}

void HELPStat::AD5940_DFTMeasure(void) {

  SWMatrixCfg_Type sw_cfg;
  impStruct eis;

  int32_t realRcal, imageRcal; 
  int32_t realRz, imageRz; 

  float magRcal, phaseRcal; 
  float magRz, phaseRz;
  float calcMag, calcPhase;

  AD5940_Delay10us(_waitClcks * (1/SYSCLCK));

  sw_cfg.Dswitch = SWD_RCAL0;
  sw_cfg.Pswitch = SWP_RCAL0;
  sw_cfg.Nswitch = SWN_RCAL1;
  sw_cfg.Tswitch = SWT_RCAL1|SWT_TRTIA;
  AD5940_SWMatrixCfgS(&sw_cfg);
	
	AD5940_AFECtrlS(AFECTRL_HSTIAPWR|AFECTRL_INAMPPWR|AFECTRL_EXTBUFPWR|\
                AFECTRL_WG|AFECTRL_DACREFPWR|AFECTRL_HSDACPWR|\
                AFECTRL_SINC2NOTCH, bTRUE);

  AD5940_AFECtrlS(AFECTRL_WG|AFECTRL_ADCPWR, bTRUE);
  settlingDelay(_currentFreq);
  
  AD5940_AFECtrlS(AFECTRL_ADCCNV|AFECTRL_DFT, bTRUE);
  settlingDelay(_currentFreq);

  AD5940_Delay10us((_waitClcks / 2) * (1/SYSCLCK));
  AD5940_Delay10us((_waitClcks / 2) * (1/SYSCLCK));
  AD5940_Delay10us((_waitClcks / 2) * (1/SYSCLCK));
  AD5940_Delay10us((_waitClcks / 2) * (1/SYSCLCK));

  pollDFT(&realRcal, &imageRcal);

  AD5940_AFECtrlS(AFECTRL_ADCPWR|AFECTRL_ADCCNV|AFECTRL_DFT|AFECTRL_WG, bFALSE);

  sw_cfg.Dswitch = SWD_CE0;
  sw_cfg.Pswitch = SWP_RE0;
  sw_cfg.Nswitch = SWN_SE0;
  sw_cfg.Tswitch = SWT_TRTIA|SWT_SE0LOAD;
  AD5940_SWMatrixCfgS(&sw_cfg);

  AD5940_AFECtrlS(AFECTRL_ADCPWR|AFECTRL_WG, bTRUE);
  settlingDelay(_currentFreq);

  AD5940_AFECtrlS(AFECTRL_ADCCNV|AFECTRL_DFT, bTRUE);
  settlingDelay(_currentFreq);

  AD5940_Delay10us((_waitClcks / 2) * (1/SYSCLCK));
  AD5940_Delay10us((_waitClcks / 2) * (1/SYSCLCK));
  AD5940_Delay10us((_waitClcks / 2) * (1/SYSCLCK));
  AD5940_Delay10us((_waitClcks / 2) * (1/SYSCLCK));

  pollDFT(&realRz, &imageRz);

  AD5940_AFECtrlS(AFECTRL_ADCCNV|AFECTRL_DFT|AFECTRL_WG|AFECTRL_ADCPWR, bFALSE);
  AD5940_AFECtrlS(AFECTRL_HSTIAPWR|AFECTRL_INAMPPWR|AFECTRL_EXTBUFPWR|\
                AFECTRL_WG|AFECTRL_DACREFPWR|AFECTRL_HSDACPWR|\
                AFECTRL_SINC2NOTCH, bFALSE);

  getMagPhase(realRcal, imageRcal, &magRcal, &phaseRcal);
  getMagPhase(realRz, imageRz, &magRz, &phaseRz);

  eis.magnitude = (magRcal / magRz) * _rcalVal; 
  eis.phaseRad = phaseRcal - phaseRz;
  eis.real = eis.magnitude * cos(eis.phaseRad);
  eis.imag = eis.magnitude * sin(eis.phaseRad) * -1; 
  eis.phaseDeg = eis.phaseRad * 180 / MATH_PI; 
  eis.freq = _currentFreq;

  printf("%d,", _sweepCfg.SweepIndex);
  printf("%.2f,", _currentFreq);
  printf("%.3f,", magRcal);
  printf("%.3f,", magRz);
  printf("%f,", eis.magnitude);
  printf("%.4f,", eis.real);
  printf("%.4f,", eis.imag);
  printf("%.4f\n", eis.phaseRad);

  eisArr[_sweepCfg.SweepIndex + (_currentCycle * _sweepCfg.SweepPoints)] = eis; 

  logSweep(&_sweepCfg, &_currentFreq);
}

void HELPStat::saveDataEIS() {
  String directory = "/" + _folderName;
  
  if(!SD.begin(CS_SD)) {
    Serial.println("Card mount failed.");
    return;
  }

  if(!SD.exists(directory)) {
    if(SD.mkdir(directory)) Serial.println("Directory made successfully.");
    else {
      Serial.println("Couldn't make a directory or it already exists.");
      return;
    }
  }
  else Serial.println("Directory already exists.");

  Serial.println("Saving to folder now.");

  String filePath = _folderName + "/" + _fileName + ".csv";

  File dataFile = SD.open(filePath, FILE_WRITE); 
  if(dataFile) {
    for(uint32_t i = 0; i <= _numCycles; i++) {
      dataFile.print("Freq, Magnitude, Phase (rad), Phase (deg), Real, Imag");
    }
    dataFile.println("");
    for(uint32_t i = 0; i < _sweepCfg.SweepPoints; i++) {
      for(uint32_t j = 0; j <= _numCycles; j++) {
        impStruct eis;
        eis = eisArr[i + (j * _sweepCfg.SweepPoints)];
        dataFile.print(eis.freq);
        dataFile.print(",");
        dataFile.print(eis.magnitude);
        dataFile.print(",");
        dataFile.print(eis.phaseRad);
        dataFile.print(",");
        dataFile.print(eis.phaseDeg);
        dataFile.print(",");
        dataFile.print(eis.real);
        dataFile.print(",");
        dataFile.print(eis.imag);
        dataFile.print(",");
      }
      dataFile.println("");
    }
    dataFile.close();
    Serial.println("Data appended successfully.");
  }
}
