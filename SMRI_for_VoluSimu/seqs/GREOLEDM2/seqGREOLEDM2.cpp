#include "seqGREOLEDM2.h"
#include "parse.h"
#include <iostream>

SeqGREOLEDM2::SeqGREOLEDM2(FOVStruct fover, KSpaceStruct kspacer,SeqComStruct seqComer, std::string configPath):BaseSeq(
    fover, kspacer, seqComer
)
{
    /*  */
    this->m_isEPI = true;
    this->parseConfig(configPath);
    this->initParameters();

}
SeqGREOLEDM2::~SeqGREOLEDM2()
{

}

bool SeqGREOLEDM2::parseConfig(std::string configPath)
{
    /* [注:] */
    /* 参考YQQ的GRE-OLED */
    // nlohmann::json configJson;
    // configJson = this->readCfs(configPath);
    std::string fileStr = this->readCfs(configPath);

    nlohmann::json configJson = nlohmann::json::parse(fileStr);
    std::cout<<"GREOLEDM2 Config"<<std::endl;
    std::cout<<configJson.dump(4)<<std::endl;

    this->m_epiMiddleStamp = configJson["EpiMiddleStamp"].get<double>();
    this->m_shiftUseBlipT = configJson["ShiftUseBlipT"].get<bool>();
    this->m_rfInterval = configJson["RFInterval"].get<double>();
    this->m_gapdt = configJson["GAPdt"].get<double>();

    this->m_rf1 = configJson["RF1"].get<RFCfgS>();
    this->m_rf2 = configJson["RF2"].get<RFCfgS>();
    this->m_rf3 = configJson["RF3"].get<RFCfgS>();
    this->m_rf4 = configJson["RF4"].get<RFCfgS>();

    this->m_shiftG1 = configJson["ShiftG1"].get<GradientCfgS>();
    this->m_shiftG2 = configJson["ShiftG2"].get<GradientCfgS>();
    this->m_shiftG3 = configJson["ShiftG3"].get<GradientCfgS>();
    this->m_shiftG4 = configJson["ShiftG4"].get<GradientCfgS>();

    this->m_shiftGraFlag = configJson["ShiftGraFlag"].get<bool>();

    this->m_xShiftGra.minG = configJson["XMinShiftGra"].get<float>();
    this->m_xShiftGra.maxG = configJson["XMaxShiftGra"].get<float>();
    this->m_yShiftGra.minG = configJson["YMinShiftGra"].get<float>();
    this->m_yShiftGra.maxG = configJson["YMaxShiftGra"].get<float>();
    this->m_zShiftGra.minG = configJson["ZMinShiftGra"].get<float>();
    this->m_zShiftGra.minG = configJson["ZMaxShiftGra"].get<float>();

    return true;
}

bool SeqGREOLEDM2::makeSeq(int fileId)
{
    this->setFileId(fileId);
    
    /* process before make sequence */
    this->processBeforeRun();
    /* 初始化参数，尤其是随机的梯度漂移+梯度 */
    this->initGREOLEDM2Parameters();

    double shiftDt = 0;
    double currentTime = 0;
    double tmpDGxAmp = 0;

    /* [1]: RF1 Pulse */
    this->pluginRF( 
        this->rectRF(this->m_rf1.rfFlipAngle, this->m_rf1.rfDt, this->m_rf1.rfPhase, this->m_rf1.rfFreq) 
        );

    currentTime = this->m_rf1.middleStamp + this->m_rf1.rfDt /2.0f;

    /* [2]: Relaxtion */
    this->pluginRelaxation(this->m_shiftG1.middleStamp - this->m_shiftG1.duration/2.0f - currentTime);

    /* [3]: Shift Gradient1 */
    this->pluginGradient(this->m_shiftG1XAmp, this->m_shiftG1YAmp, 0, false, this->m_shiftG1.duration);
    currentTime = this->m_shiftG1.middleStamp + this->m_shiftG1.duration /2.0f;

    /* [4]: Relaxtion */
    this->pluginRelaxation( this->m_rf2.middleStamp - this->m_rf2.rfDt/2.0f - currentTime );

    /* [5]: RF2 Pulse */
    this->pluginRF(
        this->rectRF(this->m_rf2.rfFlipAngle, this->m_rf2.rfDt, this->m_rf2.rfPhase, this->m_rf2.rfFreq) 
        );
    currentTime = this->m_rf2.middleStamp + this->m_rf2.rfDt/2.0f;

    /* [6]: Relaxtion */
    this->pluginRelaxation( this->m_shiftG2.middleStamp - this->m_shiftG2.duration/2.0f - currentTime );

    /* [7]: Shift Gradient2 */
    this->pluginGradient(this->m_shiftG2XAmp, this->m_shiftG2YAmp, 0, false, this->m_shiftG2.duration);
    currentTime = this->m_shiftG2.middleStamp + this->m_shiftG2.duration/2.0f;

    /* [8]: Relaxtion */
    this->pluginRelaxation( this->m_rf3.middleStamp - this->m_rf3.rfDt/2.0f - currentTime );

    /* [9]: RF3 Pulse */
    this->pluginRF(
        this->rectRF(this->m_rf3.rfFlipAngle, this->m_rf3.rfDt, this->m_rf3.rfPhase, this->m_rf3.rfFreq) 
    );
    currentTime = this->m_rf3.middleStamp + this->m_rf3.rfDt/2.0f;

    /* [10]: Relaxtion */
    this->pluginRelaxation( this->m_shiftG3.middleStamp - this->m_shiftG3.duration/2.0f - currentTime );

    /* [11]: Shift Gradient3 */
    this->pluginGradient(this->m_shiftG3XAmp, this->m_shiftG3YAmp, 0, false, this->m_shiftG3.duration);
    currentTime = this->m_shiftG3.middleStamp + this->m_shiftG3.duration/2.0f;

    /* [12]: Relaxtion */
    this->pluginRelaxation( this->m_rf4.middleStamp - currentTime );

    /* [13]: RF4 Pulse */
    this->pluginRF(
        this->rectRF(this->m_rf4.rfFlipAngle, this->m_rf4.rfDt, this->m_rf4.rfPhase, this->m_rf4.rfFreq)
    );
    currentTime = this->m_rf4.middleStamp + this->m_rf4.rfDt /2.0f;

    /* [14]: Relaxtion */
    this->pluginRelaxation( this->m_shiftG4.middleStamp - this->m_shiftG4.duration/2.0f - currentTime );

    /* [15]: Shift Gradient4 */
    this->pluginGradient(this->m_shiftG4XAmp, this->m_shiftG4YAmp, 0, false, this->m_shiftG4.duration);
    currentTime = this->m_shiftG4.middleStamp + this->m_shiftG4.duration/2.0f;

    /* [16]: Relaxation */
    this->pluginRelaxation(
        this->m_epiMiddleStamp - currentTime - this->m_esp*this->m_yN/2.0f
    );

    /* [17]: Sampling */
    for (int phase_index=0; phase_index<this->m_yN; ++phase_index)
    {
        for (int freq_index=0; freq_index<this->m_xN; ++freq_index)
        {
            if (phase_index%2 ==0)
            {
                tmpDGxAmp = this->m_dGxAmp;
            }else{
                tmpDGxAmp = -this->m_dGxAmp;
            }

            this->pluginGradient(tmpDGxAmp, 0, 0, true, this->m_samplingDt);
        }

        this->pluginGradient(0, -this->m_dGyAmp, 0, false, this->m_samplingDt);
    }
    this->pluginRelaxation(
        this->m_gapdt
    );

    /* [18]: Sampling */
    for (int phase_index=0; phase_index<this->m_yN; ++phase_index)
    {
        for (int freq_index=0; freq_index<this->m_xN; ++freq_index)
        {
            if (phase_index%2 ==0)
            {
                tmpDGxAmp = this->m_dGxAmp;
            }else{
                tmpDGxAmp = -this->m_dGxAmp;
            }

            this->pluginGradient(tmpDGxAmp, 0, 0, true, this->m_samplingDt);
        }

        this->pluginGradient(0, this->m_dGyAmp, 0, false, this->m_samplingDt);
    }

    return true;
}

double SeqGREOLEDM2::calculateGraAmp(double dt, double gAmpCoeff, int dType)
{
    double graAmp=0;
    switch (dType)
    {
        case 0:
        {
            /* X方向 */
            graAmp = gAmpCoeff*this->m_dGxAmp*dt/this->m_samplingDt;
            break;  
        };
        case 1:
        {
            /* Y方向 */
            graAmp = gAmpCoeff*this->m_dGyAmp*dt/this->m_blipT;
            break;
        };
    };
    return graAmp;
}

void SeqGREOLEDM2::initGREOLEDM2Parameters()
{
    if (this->m_shiftUseBlipT)
    {
        this->m_shiftG1.duration = this->m_samplingDt;
        this->m_shiftG2.duration = this->m_samplingDt;
        this->m_shiftG3.duration = this->m_samplingDt;
        this->m_shiftG4.duration = this->m_samplingDt;
    }
    this->randomShiftGra(4);

    this->m_shiftG1XAmp = this->calculateGraAmp(this->m_shiftG1.duration, this->m_shiftG1.xGradient + this->getShiftGra(1,1), 0);
    this->m_shiftG2XAmp = this->calculateGraAmp(this->m_shiftG2.duration, this->m_shiftG2.xGradient + this->getShiftGra(2,1), 0);
    this->m_shiftG3XAmp = this->calculateGraAmp(this->m_shiftG3.duration, this->m_shiftG3.xGradient + this->getShiftGra(3,1), 0);
    this->m_shiftG4XAmp = this->calculateGraAmp(this->m_shiftG4.duration, this->m_shiftG4.xGradient + this->getShiftGra(4,1), 0);
    
    this->m_shiftG1YAmp = this->calculateGraAmp(this->m_shiftG1.duration, this->m_shiftG1.yGradient + this->getShiftGra(1,2), 1);
    this->m_shiftG2YAmp = this->calculateGraAmp(this->m_shiftG2.duration, this->m_shiftG2.yGradient + this->getShiftGra(2,2), 1);
    this->m_shiftG3YAmp = this->calculateGraAmp(this->m_shiftG3.duration, this->m_shiftG3.yGradient + this->getShiftGra(3,2), 1);
    this->m_shiftG4YAmp = this->calculateGraAmp(this->m_shiftG4.duration, this->m_shiftG4.yGradient + this->getShiftGra(4,2), 1);

}

void SeqGREOLEDM2::randomShiftGra(int loopi)
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

float SeqGREOLEDM2::getShiftGra(int time, int direc)
{
    /*
    time: 第几次, 从第一次开始算起为1
    direc: [ x:1, y:2, z:3 ]
    */
    int index = 8 + (time-1)*3 + direc-1;
    return this->m_fileV.at(index);
}
