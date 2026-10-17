#pragma once
#include "PluginProcessor.h"

struct IndustrialLAF : public juce::LookAndFeel_V4
{
    IndustrialLAF();
    void drawRotarySlider (juce::Graphics&, int x, int y, int w, int h, float pos, float a0, float a1, juce::Slider&) override;
    void drawToggleButton (juce::Graphics&, juce::ToggleButton&, bool, bool) override;
    void drawButtonBackground (juce::Graphics&, juce::Button&, const juce::Colour&, bool, bool) override;
    juce::Font getTextButtonFont (juce::TextButton&, int) override;
    static juce::Colour orange() { return juce::Colour (0xffe8590c); }
    static juce::Colour ink()    { return juce::Colour (0xff17181a); }
    static juce::Colour plate()  { return juce::Colour (0xff232427); }
};

class WaveView : public juce::Component
{
public:
    explicit WaveView (TraperetorProcessor& p) : proc (p) {}
    void paint (juce::Graphics&) override;
private:
    TraperetorProcessor& proc;
    std::shared_ptr<Sample> cached;
    bool cachedRev = false;
    std::vector<float> peaks;
};

class TraperetorEditor : public juce::AudioProcessorEditor,
                         public juce::FileDragAndDropTarget,
                         private juce::Timer
{
public:
    explicit TraperetorEditor (TraperetorProcessor&);
    ~TraperetorEditor() override;
    void paint (juce::Graphics&) override;
    void resized() override;
    bool isInterestedInFileDrag (const juce::StringArray&) override { return true; }
    void filesDropped (const juce::StringArray&, int, int) override;
    void fileDragEnter (const juce::StringArray&, int, int) override { dragging = true; repaint(); }
    void fileDragExit (const juce::StringArray&) override { dragging = false; repaint(); }

private:
    struct Knob { juce::Slider s; juce::Label l; std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> a; };
    struct Panel { juce::String title; std::vector<Knob*> knobs; std::vector<juce::Component*> extras; int row; juce::Rectangle<int> r; };

    void timerCallback() override;
    void refreshSlots();
    void loadFromFiles (const juce::Array<juce::File>&);
    int addPanel (const juce::String& t, int row);
    void addKnob (int panel, const char* id, const char* label);
    void addToggle (int panel, const char* id, const char* label);
    void addCombo (int panel, const char* id);

    TraperetorProcessor& proc;
    IndustrialLAF laf;
    WaveView wave;
    juce::OwnedArray<juce::TextButton> slotBtns;
    juce::TextButton loadBtn { "LOAD" };
    juce::Label status;
    juce::MidiKeyboardComponent keys;
    std::unique_ptr<juce::FileChooser> chooser;

    std::vector<Panel> panels;
    juce::OwnedArray<Knob> knobs;
    juce::OwnedArray<juce::ToggleButton> toggles;
    juce::OwnedArray<juce::ComboBox> combos;
    juce::OwnedArray<juce::AudioProcessorValueTreeState::ButtonAttachment> bAtt;
    juce::OwnedArray<juce::AudioProcessorValueTreeState::ComboBoxAttachment> cAtt;
    int lastVersion = -1;
    bool dragging = false;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (TraperetorEditor)
};
