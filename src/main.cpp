#include "al/app/al_App.hpp"

#include "Score.hpp"
#include "parsed.hpp"


struct MyApp : public al::App {
    Score score;
    SynthSequencer seq;

    void onInit() override { // Called on app start
        std::cout << "onInit()" << std::endl;
        score = Score();
        score.registerSynthSequencer(seq);
        loadFromXML(score);
    }

    void onCreate() override { // Called when graphics context is available
        std::cout << "onCreate()" << std::endl;
        navControl().active(false);
        gam::sampleRate(audioIO().framesPerSecond());
        imguiInit();

        seq.verbose(true);
    }

    void onAnimate(double dt) override { // Called once before drawing
        imguiBeginFrame();

        imguiEndFrame();
    }

    void onDraw(Graphics& g) override { // Draw function
        g.clear();
        seq.render(g);
        imguiDraw();
    }

    void onSound(AudioIOData& io) override { // Audio callback
        seq.render(io);
    }

    void onMessage(osc::Message& m) override { // OSC message callback
        m.print();
    }

    bool onKeyDown(Keyboard const& k) override {
        if (ParameterGUI::usingKeyboard()) {
            return true;
        }

        switch (k.key()) {
        case ' ':
            std::cout << "Spacebar pressed!" << std::endl;
            score.playScore();
        }
        return true;
    }

    void onExit() override
    {
        imguiShutdown();
    }
};


int main() {
    MyApp app;
    app.dimensions(800, 600);
    app.configureAudio(48000., 256, 2, 0);

    app.start();
    return 0;
}
