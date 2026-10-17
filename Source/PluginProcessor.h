#pragma once
#include <juce_audio_processors/juce_audio_processors.h>
#include <juce_audio_utils/juce_audio_utils.h>
#include <juce_dsp/juce_dsp.h>
#include <array>
#include <memory>

struct Sample
{
    juce::AudioBuffer<float> fwd, rev;
    double sr = 44100.0;
    juce::String name, path;
};

struct Params
{
    float smp, start, len, pitch, fine, osc, detune, sub, noise, cut, res, atk, dec, sus, rel,
          fenv, fatk, fdec, fsus, lrate, ldepth, drive, dtime, dfb, dmix, rsize, rmix, tone, vol;
    int uni, wave, ftype, lshape, ltgt;
    bool loop, rev;
};

struct Voice
{
    bool active = false, gate = false, sampleDone = false;
    int note = 0, uniN = 1;
    float vel = 0;
    uint64_t age = 0;
    std::shared_ptr<Sample> s;
    const juce::AudioBuffer<float>* buf = nullptr;
    double pos = 0, rate = 1, startPos = 0, endPos = 0, freq = 440, subPh = 0, lfoPh = 0;
    double ph[5] {}, ratio[5] {};
    juce::ADSR amp, fe;
    juce::dsp::StateVariableTPTFilter<float> flt;

    void start (int n, float v, std::shared_ptr<Sample> smp, const Params& p, double sr, uint64_t a);
    void release() { gate = false; amp.noteOff(); fe.noteOff(); }
    void render (float* L, float* R, int n, const Params& p, double sr, juce::Random& rnd);
};

class TraperetorProcessor : public juce::AudioProcessor
{
public:
    TraperetorProcessor();
    ~TraperetorProcessor() override = default;

    void prepareToPlay (double sampleRate, int samplesPerBlock) override;
    void releaseResources() override {}
    bool isBusesLayoutSupported (const BusesLayout&) const override;
    void processBlock (juce::AudioBuffer<float>&, juce::MidiBuffer&) override;

    juce::AudioProcessorEditor* createEditor() override;
    bool hasEditor() const override { return true; }
    const juce::String getName() const override { return "TRAPERETOR"; }
    bool acceptsMidi() const override { return true; }
    bool producesMidi() const override { return false; }
    bool isMidiEffect() const override { return false; }
    double getTailLengthSeconds() const override { return 6.0; }
    int getNumPrograms() override { return 1; }
    int getCurrentProgram() override { return 0; }
    void setCurrentProgram (int) override {}
    const juce::String getProgramName (int) override { return {}; }
    void changeProgramName (int, const juce::String&) override {}
    void getStateInformation (juce::MemoryBlock&) override;
    void setStateInformation (const void*, int) override;

    int loadFiles (const juce::Array<juce::File>& files, int firstSlot);
    std::shared_ptr<Sample> getSlot (int i);

    juce::AudioProcessorValueTreeState apvts;
    juce::MidiKeyboardState keyState;
    std::atomic<int> selected { 0 }, slotsVersion { 0 };

private:
    static juce::AudioProcessorValueTreeState::ParameterLayout createLayout();
    Params snapshot();
    void startVoice (int note, float vel, const Params& p);
    void renderVoices (float* L, float* R, int n, const Params& p);
    void applyFx (float* L, float* R, int n, const Params& p);
    void makeDemo();

    std::array<std::shared_ptr<Sample>, 8> slots;
    juce::SpinLock slotLock;
    juce::AudioFormatManager formats;
    std::array<Voice, 16> voices;
    uint64_t ageCounter = 0;
    double sr = 44100.0;
    juce::Random rnd;
    std::vector<float> dBuf[2];
    int dW = 0;
    juce::dsp::StateVariableTPTFilter<float> toneF;
    juce::Reverb reverb;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (TraperetorProcessor)
};
