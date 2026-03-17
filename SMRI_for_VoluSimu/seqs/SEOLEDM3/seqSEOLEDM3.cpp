#include "seqSEOLEDM3.h"
#include "parse.h"
#include <iostream>

SeqSEOLEDM3::SeqSEOLEDM3(FOVStruct fover, KSpaceStruct kspacer,SeqComStruct seqComer, std::string configPath):BaseSeq(
    fover, kspacer, seqComer
)
{
    /*  */
    this->m_isEPI = true;
    this->parseConfig(configPath);
    this->initParameters();
    
}

SeqSEOLEDM3::~SeqSEOLEDM3()
{

}

bool SeqSEOLEDM3::parseConfig(std::string configPath)
{
    /* [注:] */
    /* 参考ZJ的SE-OLED, 注意本例中没有添加k空间中心的抖动 */
    // nlohmann::json configJson;
    // configJson = this->readCfs(configPath);
    std::string fileStr = this->readCfs(configPath);

    nlohmann::json configJson = nlohmann::json::parse(fileStr);
    
    this->m_epiMiddleStamp1 = configJson["EpiMiddleStamp1"].get<double>();
    this->m_epiMiddleStamp2 = configJson["EpiMiddleStamp2"].get<double>();
    this->m_epiMiddleStamp3 = configJson["EpiMiddleStamp3"].get<double>();

    this->m_shiftUseBlipT = configJson["ShiftUseBlipT"].get<bool>();
    this->m_rfInterval = configJson["RFInterval"].get<double>();

    this->m_rf1 = configJson["RF1"].get<RFCfgS>();
    this->m_rf2 = configJson["RF2"].get<RFCfgS>();
    this->m_rf3 = configJson["RF3"].get<RFCfgS>();
    this->m_rf4 = configJson["RF4"].get<RFCfgS>();

    this->m_shiftG1 = configJson["ShiftG1"].get<GradientCfgS>();
    this->m_shiftG2 = configJson["ShiftG2"].get<GradientCfgS>();
    this->m_shiftG3 = configJson["ShiftG3"].get<GradientCfgS>();
    this->m_shiftG4 = configJson["ShiftG4"].get<GradientCfgS>();

    this->m_reRF1 = configJson["RefocuseRF1"].get<RFCfgS>();
    this->m_reRF2 = configJson["RefocuseRF2"].get<RFCfgS>();
    this->m_reRF3 = configJson["RefocuseRF3"].get<RFCfgS>();

    this->m_crusher1 = configJson["Crusher1"].get<CrusherCfgS>();
    this->m_crusher2 = configJson["Crusher2"].get<CrusherCfgS>();
    this->m_crusher3 = configJson["Crusher3"].get<CrusherCfgS>();

    this->m_shiftGraFlag = configJson["ShiftGraFlag"].get<bool>();

    this->m_xShiftGra.minG = configJson["XMinShiftGra"].get<float>();
    this->m_xShiftGra.maxG = configJson["XMaxShiftGra"].get<float>();
    this->m_yShiftGra.minG = configJson["YMinShiftGra"].get<float>();
    this->m_yShiftGra.maxG = configJson["YMaxShiftGra"].get<float>();
    this->m_zShiftGra.minG = configJson["ZMinShiftGra"].get<float>();
    this->m_zShiftGra.minG = configJson["ZMaxShiftGra"].get<float>();

    this->m_crusher3 = configJson["Crusher3"].get<CrusherCfgS>();
    this->m_crusher3 = configJson["Crusher3"].get<CrusherCfgS>();
    this->m_crusher3 = configJson["Crusher3"].get<CrusherCfgS>();

    return true;

}

bool SeqSEOLEDM3::makeSeq(int fileId)
{
    this->setFileId(fileId);

    /* process before make sequence */
    this->processBeforeRun();

    /* Init SEOLEDM3 */
    /* 初始化参数，尤其是随机的梯度漂移+梯度 */
    this->initSEOLEDParameters();

    double dGxAmp = this->m_dGxAmp;
    double dGyAmp = this->m_dGyAmp;
    double tmpDGxAmp = 0;
    double shiftDt = 0;

    double crusherTime1 = 0, crusherTime2=0, crusherTime3 = 0;
    double currentTime = 0;

    crusherTime1 = this->m_crusher1.endStamp - this->m_crusher1.startStamp;
    crusherTime2 = this->m_crusher2.endStamp - this->m_crusher2.startStamp;
    crusherTime3 = this->m_crusher3.endStamp - this->m_crusher3.startStamp;

    /* [1] RF1 Pulse */
    this->pluginRF( 
        this->rectRF(this->m_rf1.rfFlipAngle, this->m_rf1.rfDt, this->m_rf1.rfPhase, this->m_rf1.rfFreq) 
        );
    currentTime = currentTime + this->m_rf1.rfDt /2.0f;

    /* [1] Shift Gradient1 */
    if (this->m_shiftUseBlipT)
    {
        /* Relaxtion */
        this->pluginRelaxation(this->m_shiftG1.middleStamp - this->m_samplingDt/2.0f -currentTime);
        /* ShiftGradient1 */
        this->pluginGradient(this->m_shiftG1XAmp, this->m_shiftG1YAmp, 0, false, this->m_samplingDt);
        currentTime = this->m_shiftG1.middleStamp + this->m_samplingDt/2.0f;
    }else{
        /* Relaxtion */
        this->pluginRelaxation(this->m_shiftG1.middleStamp - this->m_shiftG1.duration/2.0f -currentTime);
        /* ShiftGradient1 */
        shiftDt = this->m_shiftG1.duration;
        this->pluginGradient(this->m_shiftG1XAmp, this->m_shiftG1YAmp, 0, false, shiftDt);
        currentTime = this->m_shiftG1.middleStamp + shiftDt/2.0f;
    }

    /* [1] Relaxtion */
    this->pluginRelaxation( this->m_rf2.middleStamp - this->m_rf2.rfDt/2.0f - currentTime );

    /* [2] RF2 Pulse */
    this->pluginRF( 
        this->rectRF(this->m_rf2.rfFlipAngle, this->m_rf2.rfDt, this->m_rf2.rfPhase, this->m_rf2.rfFreq) 
        );
    currentTime = this->m_rf2.middleStamp + this->m_rf2.rfDt/2.0f;

    /* [2] Shift Gradient2 */
    if (this->m_shiftUseBlipT)
    {
        /* Relaxtion */
        this->pluginRelaxation( this->m_shiftG2.middleStamp - this->m_samplingDt/2.0f - currentTime );
        /* ShiftGradient2 */
        this->pluginGradient(this->m_shiftG2XAmp, this->m_shiftG2YAmp, 0, false, this->m_samplingDt);
        currentTime = this->m_shiftG2.middleStamp + this->m_samplingDt /2.0f;
    }else{
        /* Relaxtion */
        this->pluginRelaxation( this->m_shiftG2.middleStamp -this->m_shiftG2.duration/2.0f - currentTime );
        /* ShiftGradient3 */
        shiftDt = this->m_shiftG2.duration;
        this->pluginGradient(this->m_shiftG2XAmp, this->m_shiftG2YAmp, 0, false, shiftDt);
        currentTime = this->m_shiftG2.middleStamp + shiftDt /2.0f;
    }

    /* [2] Relaxtion */
    this->pluginRelaxation( this->m_rf3.middleStamp - this->m_rf3.rfDt/2.0f - currentTime );

    /* [3] RF3 Pulse */
    this->pluginRF(
        this->rectRF(this->m_rf3.rfFlipAngle, this->m_rf3.rfDt, this->m_rf3.rfPhase, this->m_rf3.rfFreq) 
    );
    currentTime = this->m_rf3.middleStamp + this->m_rf3.rfDt/2.0f;
    

    /* [3] Shift Gradient3 */
    if (this->m_shiftUseBlipT)
    {
        /* Relaxtion */
        this->pluginRelaxation( this->m_shiftG3.middleStamp - this->m_samplingDt/2.0f - currentTime );
        /* ShiftGradient3 */
        this->pluginGradient(this->m_shiftG3XAmp, this->m_shiftG3YAmp, 0, false, this->m_samplingDt);
        currentTime = this->m_shiftG3.middleStamp + this->m_samplingDt/2.0f;
    }else{
        /* Relaxtion */
        this->pluginRelaxation( this->m_shiftG3.middleStamp -this->m_shiftG3.duration/2.0f - currentTime );
        /* ShiftGradient3 */
        shiftDt = this->m_shiftG3.duration;
        this->pluginGradient(this->m_shiftG3XAmp, this->m_shiftG3YAmp, 0, false, shiftDt);
        currentTime = this->m_shiftG3.middleStamp + shiftDt/2.0f;
    }

    /* [3] Relaxtion */
    this->pluginRelaxation( this->m_rf4.middleStamp - this->m_rf4.rfDt/2.0f - currentTime );

    /* [4] RF4 Pulse */
    this->pluginRF(
        this->rectRF(this->m_rf4.rfFlipAngle, this->m_rf4.rfDt, this->m_rf4.rfPhase, this->m_rf4.rfFreq)
    );
    currentTime = this->m_rf4.middleStamp + this->m_rf4.rfDt/2.0f;

    /* [4] Shift Gradient4 */
    if (this->m_shiftUseBlipT)
    {
        /* [4] Relaxtion */
        this->pluginRelaxation( this->m_shiftG4.middleStamp - this->m_samplingDt/2.0f - currentTime );
        this->pluginGradient(this->m_shiftG4XAmp, this->m_shiftG4YAmp, 0, false, this->m_samplingDt);
        currentTime = this->m_shiftG4.middleStamp + this->m_samplingDt/2.0f;
    }else{
        /* [4] Relaxtion */
        this->pluginRelaxation( this->m_shiftG4.middleStamp - this->m_shiftG4.duration/2.0f - currentTime );

        shiftDt = this->m_shiftG4.duration;
        this->pluginGradient(this->m_shiftG4XAmp, this->m_shiftG4YAmp, 0, false, shiftDt);
        currentTime = this->m_shiftG4.middleStamp + shiftDt/2.0f;
    }

    /* [4] Relaxtion */
    this->pluginRelaxation(
        this->m_reRF1.middleStamp - currentTime - this->m_crusher1.endStamp
    );

    /* [4] Left Crusher */
    this->pluginGradient(
        this->m_crusherXAmp1, this->m_crusherYAmp1, 0, false, crusherTime1
    );

    /* [4] Relaxation */
    this->pluginRelaxation( this->m_crusher1.startStamp - this->m_reRF1.rfDt/2.0f );

    /* Refocuse Gradient */
    this->pluginRF(
        this->rectRF(this->m_reRF1.rfFlipAngle, this->m_reRF1.rfDt, this->m_reRF1.rfPhase, this->m_reRF1.rfFreq)
    );
    currentTime = 0 + this->m_reRF1.rfDt/2.0f;

    /* Relaxation */
    this->pluginRelaxation( this->m_crusher1.startStamp - currentTime );

    /* Right Crusher */
    this->pluginGradient( this->m_crusherXAmp1, this->m_crusherYAmp1, 0, false, crusherTime1 );

    /* Relaxation */
    this->pluginRelaxation( 
        this->m_epiMiddleStamp1 - this->m_reRF1.middleStamp - this->m_crusher1.endStamp - this->m_esp*this->m_yN/2.0f
    );

    /* Sampling */
    for (int phase_index=0; phase_index<this->m_yN; ++phase_index)
    {
        for (int freq_index=0; freq_index<this->m_xN; ++freq_index)
        {
            if (phase_index%2 ==0)
            {
                tmpDGxAmp = -dGxAmp;
            }else{
                tmpDGxAmp = dGxAmp;
            }
            this->pluginGradient(tmpDGxAmp, 0, 0, true, this->m_samplingDt);
        }
        this->pluginGradient(0, -dGyAmp, 0, false, this->m_samplingDt);
    }

    /* ===Second EPI Chain ===*/
    currentTime = this->m_epiMiddleStamp1 + this->m_esp*this->m_yN/2.0f;
    this->pluginRelaxation(
        this->m_reRF2.middleStamp - this->m_crusher2.endStamp - currentTime
    );

    this->pluginGradient(
        this->m_crusherXAmp2, this->m_crusherYAmp2, 0, false, crusherTime2
    );

    this->pluginRelaxation(
        this->m_crusher2.startStamp - this->m_reRF2.rfDt/2.0f
    );

    this->pluginRF(
        this->rectRF(this->m_reRF2.rfFlipAngle, this->m_reRF2.rfDt, this->m_reRF2.rfPhase, this->m_reRF2.rfFreq)
    );

    this->pluginRelaxation(
        this->m_crusher2.startStamp - this->m_reRF2.rfDt/2.0f
    );

    this->pluginGradient(
        this->m_crusherXAmp2, this->m_crusherYAmp2, 0, false, crusherTime2
    );

    currentTime = this->m_reRF2.middleStamp + this->m_crusher2.endStamp ;

    this->pluginRelaxation(
        this->m_epiMiddleStamp2 - currentTime - this->m_esp*this->m_yN/2.0f
    );

    /* Sampling */
    for (int phase_index=0; phase_index<this->m_yN; ++phase_index)
    {
        for (int freq_index=0; freq_index<this->m_xN; ++freq_index)
        {
            if (phase_index%2 ==0)
            {
                tmpDGxAmp = dGxAmp;
            }else{
                tmpDGxAmp = -dGxAmp;
            }

            this->pluginGradient(tmpDGxAmp, 0, 0, true, this->m_samplingDt);
        }
        
        this->pluginGradient(0, -dGyAmp, 0, false, this->m_samplingDt);
    }

    /* ===Third EPI Chain ===*/
    currentTime = this->m_epiMiddleStamp2 + this->m_esp*this->m_yN/2.0f;
    this->pluginRelaxation(
        this->m_reRF3.middleStamp - this->m_crusher3.endStamp - currentTime
    );

    this->pluginGradient(
        this->m_crusherXAmp3, this->m_crusherYAmp3, 0, false, crusherTime3
    );

    this->pluginRelaxation(
        this->m_crusher3.startStamp - this->m_reRF3.rfDt/2.0f
    );

    this->pluginRF(
        this->rectRF(this->m_reRF3.rfFlipAngle, this->m_reRF3.rfDt, this->m_reRF3.rfPhase, this->m_reRF3.rfFreq)
    );

    this->pluginRelaxation(
        this->m_crusher3.startStamp - this->m_reRF3.rfDt/2.0f
    );

    this->pluginGradient(
        this->m_crusherXAmp3, this->m_crusherYAmp3, 0, false, crusherTime3
    );

    currentTime = this->m_reRF3.middleStamp + this->m_crusher3.endStamp;

    this->pluginRelaxation(
        this->m_epiMiddleStamp3 - currentTime - this->m_esp*this->m_yN/2.0f
    );

    /* Sampling */
    for (int phase_index=0; phase_index<this->m_yN; ++phase_index)
    {
        for (int freq_index=0; freq_index<this->m_xN; ++freq_index)
        {
            if (phase_index%2 ==0)
            {
                tmpDGxAmp = -dGxAmp;
            }else{
                tmpDGxAmp = dGxAmp;
            }

            this->pluginGradient(tmpDGxAmp, 0, 0, true, this->m_samplingDt);
        }
        
        this->pluginGradient(0, -dGyAmp, 0, false, this->m_samplingDt);
    }
    return true;
}   

double SeqSEOLEDM3::calculateGraAmp(double dt, double gAmpCoeff, int dType)
{
    double graAmp=0;
    switch (dType)
    {
        case 0:
        {
            /* X方向 */
            graAmp = gAmpCoeff*this->m_dGxAmp*this->m_samplingDt/dt;
            break;  
        };
        case 1:
        {
            /* Y方向 */
            graAmp = gAmpCoeff*this->m_dGyAmp*this->m_blipT/dt;
            break;
        };
    }
    return graAmp;
}

void SeqSEOLEDM3::initSEOLEDParameters()
{   
    if (this->m_shiftUseBlipT)
    {
        this->m_shiftG1.duration = this->m_samplingDt;
        this->m_shiftG2.duration = this->m_samplingDt;
        this->m_shiftG3.duration = this->m_samplingDt;
        this->m_shiftG4.duration = this->m_samplingDt;
    }
    this->randomShiftGra(4);
    this->m_shiftG1XAmp = this->calculateGraAmp(this->m_shiftG1.duration, this->m_shiftG1.xGradient+this->getShiftGra(1,1), 0);
    this->m_shiftG2XAmp = this->calculateGraAmp(this->m_shiftG2.duration, this->m_shiftG2.xGradient+this->getShiftGra(2,1), 0);
    this->m_shiftG3XAmp = this->calculateGraAmp(this->m_shiftG3.duration, this->m_shiftG3.xGradient+this->getShiftGra(3,1), 0);
    this->m_shiftG4XAmp = this->calculateGraAmp(this->m_shiftG4.duration, this->m_shiftG4.xGradient+this->getShiftGra(4,1), 0);

    this->m_shiftG1YAmp = this->calculateGraAmp(this->m_shiftG1.duration, this->m_shiftG1.yGradient+this->getShiftGra(1,2), 1);
    this->m_shiftG2YAmp = this->calculateGraAmp(this->m_shiftG2.duration, this->m_shiftG2.yGradient+this->getShiftGra(2,2), 1);
    this->m_shiftG3YAmp = this->calculateGraAmp(this->m_shiftG3.duration, this->m_shiftG3.yGradient+this->getShiftGra(3,2), 1);
    this->m_shiftG4YAmp = this->calculateGraAmp(this->m_shiftG4.duration, this->m_shiftG4.yGradient+this->getShiftGra(4,2), 1);

    float dt = this->m_crusher1.endStamp-this->m_crusher1.startStamp;
    this->m_crusherXAmp1 = this->calculateGraAmp(dt, this->m_crusher1.xGradient*this->m_xN, 0);
    this->m_crusherYAmp1 = this->calculateGraAmp(dt, this->m_crusher1.yGradient*this->m_yN, 1);

    dt = this->m_crusher2.endStamp-this->m_crusher2.startStamp;
    this->m_crusherXAmp2 = this->calculateGraAmp(dt, this->m_crusher2.xGradient*this->m_xN, 0);
    this->m_crusherYAmp2 = this->calculateGraAmp(dt, this->m_crusher2.yGradient*this->m_yN, 1);

    dt = this->m_crusher3.endStamp-this->m_crusher3.startStamp;
    this->m_crusherXAmp3 = this->calculateGraAmp(dt, this->m_crusher3.xGradient*this->m_xN, 0);
    this->m_crusherYAmp3 = this->calculateGraAmp(dt, this->m_crusher3.yGradient*this->m_yN, 1);
}

void SeqSEOLEDM3::randomShiftGra(int loopi)
{
    if(this->m_shiftGraFlag==false)
    {
        for (int i =0; i<loopi; i++)
        {

            this->m_fileV.push_back(0);
            this->m_fileV.push_back(0);
            this->m_fileV.push_back(0);
        }
        return;
    }
    float gxGra=0,gyGra=0,gzGra =0;
    std::uniform_real_distribution<float> xDistri(this->m_xShiftGra.minG * this->m_shiftG1.xGradient, this->m_xShiftGra.maxG * this->m_shiftG1.xGradient);
    std::uniform_real_distribution<float> yDistri(this->m_yShiftGra.minG * this->m_shiftG1.yGradient, this->m_yShiftGra.maxG * this->m_shiftG1.yGradient);
    std::uniform_real_distribution<float> zDistri(this->m_zShiftGra.minG, this->m_zShiftGra.maxG);

    gxGra = xDistri(this->m_randomEng);
    gyGra = yDistri(this->m_randomEng);
    gzGra = zDistri(this->m_randomEng);

    this->m_fileV.push_back(gxGra);
    this->m_fileV.push_back(gyGra);
    this->m_fileV.push_back(gzGra);

    xDistri.param(std::uniform_real_distribution<float>::param_type(this->m_xShiftGra.minG * this->m_shiftG2.xGradient, this->m_xShiftGra.maxG * this->m_shiftG2.xGradient));
    yDistri.param(std::uniform_real_distribution<float>::param_type(this->m_yShiftGra.minG * this->m_shiftG2.yGradient, this->m_yShiftGra.maxG * this->m_shiftG2.yGradient));
    zDistri.param(std::uniform_real_distribution<float>::param_type(this->m_zShiftGra.minG, this->m_zShiftGra.maxG));
    gxGra = xDistri(this->m_randomEng);
    gyGra = yDistri(this->m_randomEng);
    gzGra = zDistri(this->m_randomEng);

    this->m_fileV.push_back(gxGra);
    this->m_fileV.push_back(gyGra);
    this->m_fileV.push_back(gzGra);

    xDistri.param(std::uniform_real_distribution<float>::param_type(this->m_xShiftGra.minG * this->m_shiftG3.xGradient, this->m_xShiftGra.maxG * this->m_shiftG3.xGradient));
    yDistri.param(std::uniform_real_distribution<float>::param_type(this->m_yShiftGra.minG * this->m_shiftG3.yGradient, this->m_yShiftGra.maxG * this->m_shiftG3.yGradient));
    zDistri.param(std::uniform_real_distribution<float>::param_type(this->m_zShiftGra.minG, this->m_zShiftGra.maxG));

    gxGra = xDistri(this->m_randomEng);
    gyGra = yDistri(this->m_randomEng);
    gzGra = zDistri(this->m_randomEng);

    this->m_fileV.push_back(gxGra);
    this->m_fileV.push_back(gyGra);
    this->m_fileV.push_back(gzGra);

    xDistri.param(std::uniform_real_distribution<float>::param_type(this->m_xShiftGra.minG * this->m_shiftG4.xGradient, this->m_xShiftGra.maxG * this->m_shiftG4.xGradient));
    yDistri.param(std::uniform_real_distribution<float>::param_type(this->m_yShiftGra.minG * this->m_shiftG4.yGradient, this->m_yShiftGra.maxG * this->m_shiftG4.yGradient));
    zDistri.param(std::uniform_real_distribution<float>::param_type(this->m_zShiftGra.minG, this->m_zShiftGra.maxG));

    gxGra = xDistri(this->m_randomEng);
    gyGra = yDistri(this->m_randomEng);
    gzGra = zDistri(this->m_randomEng);

    this->m_fileV.push_back(gxGra);
    this->m_fileV.push_back(gyGra);
    this->m_fileV.push_back(gzGra);

}

float SeqSEOLEDM3::getShiftGra(int time, int direc)
{
    /*
    time: 第几次, 从第一次开始算起为1
    direc: [ x:1, y:2, z:3 ]
    */
    int index = 8 + (time-1)*3 + direc-1;
    return this->m_fileV.at(index);
}