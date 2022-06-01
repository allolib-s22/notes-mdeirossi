#ifndef TUBA_HPP
#define TUBA_HPP

#include "Gamma/Analysis.h"
#include "Gamma/Effects.h"
#include "Gamma/Envelope.h"
#include "Gamma/Gamma.h"
#include "Gamma/Oscillator.h"
#include "Gamma/Types.h"
#include "al/scene/al_PolySynth.hpp"
#include "al/ui/al_Parameter.hpp"

using namespace gam;
using namespace al;
class Tuba : public SynthVoice {
public:
    // Unit generators
    float mNoiseMix;
    gam::Pan<> mPan;
    gam::ADSR<> mAmpEnv;
    gam::DSF<> mOsc;
    gam::NoiseWhite<> mNoise;
    gam::Reson<> mRes;
    gam::Env<2> mCFEnv;
    gam::Env<2> mBWEnv;
    // Additional members
    Mesh mMesh;

    // Initialize voice. This function will nly be called once per voice
    void init() override {
        mAmpEnv.curve(4); // linear segments
        mAmpEnv.levels(0, 1.0, 1.0, 0); // These tables are not normalized, so scale to 0.3
        mAmpEnv.sustainPoint(2); // Make point 2 sustain until a release is issued
        mCFEnv.curve(0);
        mBWEnv.curve(0);
        mOsc.harmonics(9);

        createInternalTriggerParameter("amp", 0.2, 0.0, 1.0);
        createInternalTriggerParameter("freq", 60, 20, 5000);
        createInternalTriggerParameter("attackTime", 0.03, 0.01, 3.0);
        createInternalTriggerParameter("releaseTime", 0.05, 0.01, 10.0);
        createInternalTriggerParameter("sustain", 0.7, 0.0, 1.0);
        createInternalTriggerParameter("noise", 0.025, 0.0, 1.0);
        createInternalTriggerParameter("envDur", 1, 0.0, 5.0);
        createInternalTriggerParameter("cf1", 400, 10.0, 5000);
        createInternalTriggerParameter("cf2", 700, 10.0, 5000);
        createInternalTriggerParameter("cfRise", 0.1, 0.1, 2);
        createInternalTriggerParameter("bw1", 5000, 10.0, 5000);
        createInternalTriggerParameter("bw2", 4700, 10.0, 5000);
        createInternalTriggerParameter("bwRise", 0.1, 0.1, 2);
        createInternalTriggerParameter("hmnum", 9.0, 5.0, 20.0);
        createInternalTriggerParameter("hmamp", 0.600, 0.0, 1.0);
        createInternalTriggerParameter("pan", 0.0, -1.0, 1.0);
    }

    virtual void onProcess(AudioIOData& io) override {
        updateFromParameters();
        float amp = getInternalParameterValue("amp");
        float noiseMix = getInternalParameterValue("noise");
        while (io()) {
            // mix oscillator with noise
            float s1 = mOsc() * (1 - noiseMix) + mNoise() * noiseMix;

            // apply resonant filter
            mRes.set(mCFEnv(), mBWEnv());
            s1 = mRes(s1);

            // appy amplitude envelope
            s1 *= mAmpEnv() * amp;

            float s2;
            mPan(s1, s1, s2);
            io.out(0) += s1;
            io.out(1) += s2;
        }


        if (mAmpEnv.done()) free();
    }

    virtual void onTriggerOn() override {
        updateFromParameters();
        mAmpEnv.reset();
        mCFEnv.reset();
        mBWEnv.reset();
    }

    virtual void onTriggerOff() override {
        mAmpEnv.triggerRelease();
        //        mCFEnv.triggerRelease();
        //        mBWEnv.triggerRelease();
    }

    void updateFromParameters() {
        mOsc.freq(getInternalParameterValue("freq"));
        mOsc.harmonics(getInternalParameterValue("hmnum"));
        mOsc.ampRatio(getInternalParameterValue("hmamp"));
        mAmpEnv.attack(getInternalParameterValue("attackTime"));
        //    mAmpEnv.decay(getInternalParameterValue("attackTime"));
        mAmpEnv.release(getInternalParameterValue("releaseTime"));
        mAmpEnv.levels()[1] = getInternalParameterValue("sustain");
        mAmpEnv.levels()[2] = getInternalParameterValue("sustain");

        mPan.pos(getInternalParameterValue("pan"));
        mCFEnv.levels(getInternalParameterValue("cf1"),
            getInternalParameterValue("cf2"),
            getInternalParameterValue("cf1"));


        mCFEnv.lengths()[0] = getInternalParameterValue("cfRise");
        mCFEnv.lengths()[1] = 1 - getInternalParameterValue("cfRise");
        mBWEnv.levels(getInternalParameterValue("bw1"),
            getInternalParameterValue("bw2"),
            getInternalParameterValue("bw1"));
        mBWEnv.lengths()[0] = getInternalParameterValue("bwRise");
        mBWEnv.lengths()[1] = 1 - getInternalParameterValue("bwRise");

        mCFEnv.totalLength(getInternalParameterValue("envDur"));
        mBWEnv.totalLength(getInternalParameterValue("envDur"));
    }
};

#endif
