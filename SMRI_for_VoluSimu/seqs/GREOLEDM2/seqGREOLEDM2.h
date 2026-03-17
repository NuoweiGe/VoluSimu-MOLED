#ifndef _SEQGREOLEDM2_H
#define _SEQGREOLEDM2_H

#include "BaseSeq.h"


class SeqGREOLEDM2: public BaseSeq
{
    public:
        SeqGREOLEDM2(FOVStruct fover, KSpaceStruct kspacer,SeqComStruct seqComer, std::string configPath);
        ~SeqGREOLEDM2();
        bool parseConfig(std::string configPath);
        bool makeSeq(int fileId);

        double calculateGraAmp(double dt, double gAmpCoeff, int dType);
        void initGREOLEDM2Parameters();

        void randomShiftGra(int loopi);
        float getShiftGra(int time, int direc);
        
    private:

        /* EPI回波链中间的时间戳 */
        double m_epiMiddleStamp;
        /* 移位梯度使用blipT */
        /* 若开启该选项，则会导致移位梯度中的结束时间无用 */
        bool m_shiftUseBlipT;

        double m_gapdt;

        /* 避免重复计算 */
        double m_shiftG1XAmp;
        double m_shiftG1YAmp;
        double m_shiftG2XAmp;
        double m_shiftG2YAmp;
        double m_shiftG3XAmp;
        double m_shiftG3YAmp;
        double m_shiftG4XAmp;
        double m_shiftG4YAmp;

        /* 激发脉冲1 */
        /* [注]: 第一个激发脉冲的中间时刻是t=0的时刻 */
        RFCfgS m_rf1;
        /* 移位梯度1 */
        GradientCfgS m_shiftG1;

        /* 激发脉冲2 */
        RFCfgS m_rf2;
        /* 移位梯度2 */
        GradientCfgS m_shiftG2;

        /* 激发脉冲3 */
        RFCfgS m_rf3;
        /* 移位梯度3 */
        GradientCfgS m_shiftG3;

        /* 激发脉冲4 */
        RFCfgS m_rf4;
        /* 移位梯度4 */
        GradientCfgS m_shiftG4;

        /* 梯度漂移/梯度抖动 */
        bool m_shiftGraFlag;
        ShiftGra m_xShiftGra;
        ShiftGra m_yShiftGra;
        ShiftGra m_zShiftGra;

};
#endif