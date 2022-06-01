#ifndef CELESTA_HPP
#define CELESTA_HPP

#include "Gamma/Effects.h"
#include "Gamma/Envelope.h"
#include "Gamma/Oscillator.h"

#include "al/scene/al_PolySynth.hpp"

using namespace al;

class Celesta : public SynthVoice
{
public:
    gam::Pan<> mPan;
    gam::Sine<> mOsc;
    gam::Sine<> mOsc2;
    gam::Env<4> mAmpEnv;

    void init() override
    {
        mAmpEnv.curve(-2);
        mAmpEnv.levels(0, 1.0, 0.8, 0, 0);
        mAmpEnv.sustainPoint(3);

        createInternalTriggerParameter("amp", 0.3, 0.0, 1.0);
        createInternalTriggerParameter("freq", 60, 20, 5000);
        createInternalTriggerParameter("pan", 0.0, -1.0, 1.0);
        createInternalTriggerParameter("pedalDuration", 3.0, 0.1, 5.0);

        mAmpEnv.lengths()[0] = 0.01; // attack
        mAmpEnv.lengths()[1] = 0.02; // decay
        mAmpEnv.lengths()[2] = 2.0; // sustain decay
        mAmpEnv.lengths()[3] = 1.0; // release
    }

    virtual void onProcess(AudioIOData& io) override
    {
        float f = getInternalParameterValue("freq");
        mOsc.freq(f);
        mOsc2.freq(f * 2);

        float a = getInternalParameterValue("amp");

        mPan.pos(getInternalParameterValue("pan"));
        while (io())
        {
            float s1 = mAmpEnv() * (mOsc() * (a / 4.0f) + mOsc2() * (a / 2.0f));
            float s2;
            mPan(s1, s1, s2);
            io.out(0) += s1;
            io.out(1) += s2;
        }
        
        if (mAmpEnv.done()) free();
    }
    
    void onTriggerOn() override { mAmpEnv.reset(); }
    void onTriggerOff() override { mAmpEnv.release(); }
};

#endif
