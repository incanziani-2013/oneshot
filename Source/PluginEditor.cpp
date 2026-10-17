#include "PluginEditor.h"
using namespace juce;

// ---------------------------------------------------------------- look
IndustrialLAF::IndustrialLAF()
{
    setColour (Slider::textBoxTextColourId, Colour (0xffffb27d));
    setColour (Slider::textBoxBackgroundColourId, ink());
    setColour (Slider::textBoxOutlineColourId, Colours::transparentBlack);
    setColour (ComboBox::backgroundColourId, Colour (0xffc4c6c9));
    setColour (ComboBox::textColourId, ink());
    setColour (ComboBox::outlineColourId, ink());
    setColour (ComboBox::arrowColourId, ink());
    setColour (PopupMenu::backgroundColourId, Colour (0xffc4c6c9));
    setColour (PopupMenu::textColourId, ink());
    setColour (PopupMenu::highlightedBackgroundColourId, plate());
    setColour (PopupMenu::highlightedTextColourId, Colour (0xffffb27d));
    setColour (Label::textColourId, ink());
}

void IndustrialLAF::drawRotarySlider (Graphics& g, int x, int y, int w, int h, float pos, float a0, float a1, Slider&)
{
    auto b = Rectangle<float> ((float) x, (float) y, (float) w, (float) h).reduced (3.0f);
    const float r = jmin (b.getWidth(), b.getHeight()) / 2.0f;
    const auto c = b.getCentre();
    Path t; t.addCentredArc (c.x, c.y, r - 2, r - 2, 0, a0, a1, true);
    g.setColour (Colour (0x55000000)); g.strokePath (t, PathStrokeType (4.0f));
    Path v; v.addCentredArc (c.x, c.y, r - 2, r - 2, 0, a0, a0 + pos * (a1 - a0), true);
    g.setColour (orange()); g.strokePath (v, PathStrokeType (4.0f));
    const float dr = r * 0.72f;
    g.setColour (Colour (0xff2a2b2d)); g.fillEllipse (c.x - dr, c.y - dr, dr * 2, dr * 2);
    g.setColour (ink()); g.drawEllipse (c.x - dr, c.y - dr, dr * 2, dr * 2, 2.0f);
    g.setColour (Colour (0xff414245));
    for (int i = 0; i < 24; ++i)
    {
        const float a = i * MathConstants<float>::twoPi / 24.0f;
        g.drawLine (c.x + std::sin (a) * dr * 0.8f, c.y - std::cos (a) * dr * 0.8f,
                    c.x + std::sin (a) * dr * 0.97f, c.y - std::cos (a) * dr * 0.97f, 1.5f);
    }
    const float ang = a0 + pos * (a1 - a0);
    g.setColour (orange());
    g.drawLine (c.x + std::sin (ang) * dr * 0.3f, c.y - std::cos (ang) * dr * 0.3f,
                c.x + std::sin (ang) * dr * 0.92f, c.y - std::cos (ang) * dr * 0.92f, 3.5f);
}

void IndustrialLAF::drawToggleButton (Graphics& g, ToggleButton& b, bool, bool)
{
    const auto r = b.getLocalBounds().toFloat().reduced (1.0f);
    const bool on = b.getToggleState();
    g.setColour (on ? plate() : Colour (0xffc4c6c9)); g.fillRect (r);
    g.setColour (ink()); g.drawRect (r, 2.0f);
    g.setColour (on ? orange() : Colour (0xff555555)); g.fillEllipse (r.getX() + 7, r.getCentreY() - 4, 8, 8);
    g.setColour (on ? Colour (0xffffb27d) : ink());
    g.setFont (Font (FontOptions (13.0f, Font::bold)));
    g.drawText (b.getButtonText(), r.withTrimmedLeft (20).toNearestInt(), Justification::centredLeft);
}

void IndustrialLAF::drawButtonBackground (Graphics& g, Button& b, const Colour&, bool, bool down)
{
    const auto r = b.getLocalBounds().toFloat().reduced (1.0f);
    const bool on = b.getToggleState() || down;
    g.setColour (on ? plate() : Colour (0xffc4c6c9)); g.fillRect (r);
    g.setColour (on ? orange() : ink()); g.drawRect (r, 2.0f);
}

Font IndustrialLAF::getTextButtonFont (TextButton&, int) { return Font (FontOptions (14.0f, Font::bold)); }

// ---------------------------------------------------------------- waveform
void WaveView::paint (Graphics& g)
{
    auto r = getLocalBounds();
    g.fillAll (Colour (0xff1b1c1e));
    g.setColour (IndustrialLAF::ink()); g.drawRect (r, 2);
    auto s = proc.getSlot (proc.selected.load());
    const bool rv = proc.apvts.getRawParameterValue ("rev")->load() > 0.5f;
    if (s == nullptr)
    {
        g.setColour (Colours::grey); g.setFont (Font (FontOptions (16.0f)));
        g.drawText ("Empty slot: drop an audio file here", r, Justification::centred);
        return;
    }
    const int w = r.getWidth() - 4, h = r.getHeight() - 4;
    if (s != cached || rv != cachedRev || (int) peaks.size() != w)
    {
        cached = s; cachedRev = rv; peaks.assign ((size_t) w, 0.0f);
        const auto& b = rv ? s->rev : s->fwd;
        const int len = b.getNumSamples(), step = jmax (1, len / jmax (1, w));
        const float* d = b.getReadPointer (0);
        for (int x = 0; x < w; ++x)
        {
            float m = 0;
            for (int i = x * step; i < jmin (len, (x + 1) * step); i += 4) m = jmax (m, std::abs (d[i]));
            peaks[(size_t) x] = m;
        }
    }
    g.setColour (IndustrialLAF::orange());
    for (int x = 0; x < w; ++x)
    {
        const float m = peaks[(size_t) x];
        g.fillRect (2 + x, 2 + (int) (h * 0.5f - m * h * 0.5f), 1, jmax (1, (int) (m * h)));
    }
    const float st = proc.apvts.getRawParameterValue ("start")->load();
    const float ln = proc.apvts.getRawParameterValue ("len")->load();
    const float a = st * w, b2 = a + (1 - st) * ln * w;
    g.setColour (Colour (0xa6000000));
    g.fillRect (2.0f, 2.0f, a, (float) h);
    g.fillRect (2.0f + b2, 2.0f, w - b2, (float) h);
}

// ---------------------------------------------------------------- editor
static const int W = 1000, H = 870;

TraperetorEditor::TraperetorEditor (TraperetorProcessor& p)
    : AudioProcessorEditor (&p), proc (p), wave (p),
      keys (p.keyState, MidiKeyboardComponent::horizontalKeyboard)
{
    setLookAndFeel (&laf);

    int pn = addPanel ("SAMPLE", 0);
    addKnob (pn, "smp", "LEVEL"); addKnob (pn, "start", "START"); addKnob (pn, "len", "LENGTH");
    addKnob (pn, "pitch", "PITCH"); addKnob (pn, "fine", "FINE");
    addToggle (pn, "loop", "LOOP"); addToggle (pn, "rev", "REVERSE");

    pn = addPanel ("OSCILLATOR", 0);
    addKnob (pn, "osc", "LEVEL"); addKnob (pn, "uni", "UNISON"); addKnob (pn, "detune", "DETUNE");
    addKnob (pn, "sub", "SUB"); addKnob (pn, "noise", "NOISE"); addCombo (pn, "wave");

    pn = addPanel ("FILTER", 1);
    addKnob (pn, "cut", "CUTOFF"); addKnob (pn, "res", "RESO"); addCombo (pn, "ftype");

    pn = addPanel ("AMP ENV", 1);
    addKnob (pn, "atk", "ATTACK"); addKnob (pn, "dec", "DECAY"); addKnob (pn, "sus", "SUSTAIN"); addKnob (pn, "rel", "RELEASE");

    pn = addPanel ("FILTER ENV", 1);
    addKnob (pn, "fenv", "AMOUNT"); addKnob (pn, "fatk", "ATTACK"); addKnob (pn, "fdec", "DECAY"); addKnob (pn, "fsus", "SUSTAIN");

    pn = addPanel ("LFO", 2);
    addKnob (pn, "lrate", "RATE"); addKnob (pn, "ldepth", "DEPTH"); addCombo (pn, "lshape"); addCombo (pn, "ltgt");

    pn = addPanel ("EFFECTS", 2);
    addKnob (pn, "drive", "DRIVE"); addKnob (pn, "dtime", "DLY TIME"); addKnob (pn, "dfb", "DLY FDBK");
    addKnob (pn, "dmix", "DLY MIX"); addKnob (pn, "rsize", "REV SIZE"); addKnob (pn, "rmix", "REV MIX");

    pn = addPanel ("OUTPUT", 2);
    addKnob (pn, "tone", "TONE"); addKnob (pn, "vol", "MASTER");

    for (int i = 0; i < 8; ++i)
    {
        auto* b = slotBtns.add (new TextButton());
        b->setClickingTogglesState (true); b->setRadioGroupId (1);
        b->onClick = [this, i] { proc.selected = i; wave.repaint(); };
        addAndMakeVisible (b);
    }
    loadBtn.onClick = [this]
    {
        chooser = std::make_unique<FileChooser> ("Choose oneshots", File(), "*.wav;*.aif;*.aiff;*.mp3;*.flac;*.ogg");
        chooser->launchAsync (FileBrowserComponent::openMode | FileBrowserComponent::canSelectMultipleItems,
                              [this] (const FileChooser& fc) { loadFromFiles (fc.getResults()); });
    };
    addAndMakeVisible (loadBtn);
    status.setText ("Drag audio files onto the plugin (WAV, MP3, FLAC, OGG). Each one fills a slot.", dontSendNotification);
    status.setColour (Label::textColourId, IndustrialLAF::ink());
    addAndMakeVisible (status);
    addAndMakeVisible (wave);
    keys.setAvailableRange (36, 96); keys.setLowestVisibleKey (48); keys.setKeyWidth (24.0f);
    keys.setColour (MidiKeyboardComponent::keyDownOverlayColourId, IndustrialLAF::orange());
    keys.setColour (MidiKeyboardComponent::mouseOverKeyOverlayColourId, Colour (0x44e8590c));
    addAndMakeVisible (keys);

    setSize (W, H);
    refreshSlots();
    startTimerHz (10);
}

TraperetorEditor::~TraperetorEditor() { setLookAndFeel (nullptr); }

int TraperetorEditor::addPanel (const String& t, int row) { panels.push_back ({ t, {}, {}, row, {} }); return (int) panels.size() - 1; }

void TraperetorEditor::addKnob (int pi, const char* id, const char* label)
{
    auto* k = knobs.add (new Knob());
    k->s.setSliderStyle (Slider::RotaryVerticalDrag);
    k->s.setRotaryParameters (MathConstants<float>::pi * 1.25f, MathConstants<float>::pi * 2.75f, true);
    k->s.setTextBoxStyle (Slider::TextBoxBelow, false, 62, 15);
    k->s.setMouseDragSensitivity (160);
    k->s.setTooltip ("Drag up/down. Double-click to reset. Shift = fine.");
    k->a = std::make_unique<AudioProcessorValueTreeState::SliderAttachment> (proc.apvts, id, k->s);
    k->l.setText (label, dontSendNotification);
    k->l.setJustificationType (Justification::centred);
    k->l.setFont (Font (FontOptions (13.0f, Font::bold)));
    addAndMakeVisible (k->s); addAndMakeVisible (k->l);
    panels[(size_t) pi].knobs.push_back (k);
}

void TraperetorEditor::addToggle (int pi, const char* id, const char* label)
{
    auto* t = toggles.add (new ToggleButton (label));
    bAtt.add (new AudioProcessorValueTreeState::ButtonAttachment (proc.apvts, id, *t));
    addAndMakeVisible (t); panels[(size_t) pi].extras.push_back (t);
}

void TraperetorEditor::addCombo (int pi, const char* id)
{
    auto* c = combos.add (new ComboBox());
    if (auto* cp = dynamic_cast<AudioParameterChoice*> (proc.apvts.getParameter (id))) c->addItemList (cp->choices, 1);
    cAtt.add (new AudioProcessorValueTreeState::ComboBoxAttachment (proc.apvts, id, *c));
    addAndMakeVisible (c); panels[(size_t) pi].extras.push_back (c);
}

void TraperetorEditor::loadFromFiles (const Array<File>& f)
{
    const int n = proc.loadFiles (f, proc.selected.load());
    status.setText (n > 0 ? String (n) + " oneshot" + (n > 1 ? "s" : "") + " loaded. Play the keyboard or send MIDI."
                          : "Could not read those files. Try WAV, MP3, FLAC or OGG.", dontSendNotification);
}

void TraperetorEditor::filesDropped (const StringArray& files, int, int)
{
    dragging = false;
    Array<File> fs; for (auto& f : files) fs.add (File (f));
    loadFromFiles (fs); repaint();
}

void TraperetorEditor::refreshSlots()
{
    lastVersion = proc.slotsVersion.load();
    for (int i = 0; i < slotBtns.size(); ++i)
    {
        auto s = proc.getSlot (i);
        slotBtns[i]->setButtonText (String (i + 1) + "  " + (s ? s->name.substring (0, 12) : String ("empty")));
        slotBtns[i]->setToggleState (i == proc.selected.load(), dontSendNotification);
    }
}

void TraperetorEditor::timerCallback()
{
    if (lastVersion != proc.slotsVersion.load()) refreshSlots();
    wave.repaint();
}

void TraperetorEditor::resized()
{
    const int m = 20, gap = 12, rowH = 168;
    // bank
    const int bankY = 70, slotW = (W - 2 * m - 7 * 6) / 8;
    for (int i = 0; i < slotBtns.size(); ++i) slotBtns[i]->setBounds (m + 14 + i * (slotW + 6) - (i ? 2 : 0), bankY + 36, slotW - 4, 26);
    wave.setBounds (m + 14, bankY + 68, W - 2 * m - 28, 70);
    loadBtn.setBounds (m + 14, bankY + 144, 90, 24);
    status.setBounds (m + 114, bankY + 144, W - 2 * m - 130, 24);

    const int y0 = 250;
    for (int row = 0; row < 3; ++row)
    {
        std::vector<Panel*> ps; int sum = 0;
        for (auto& p : panels) if (p.row == row)
        {
            ps.push_back (&p);
            int need = 24 + (int) p.knobs.size() * 78, ex = 0;
            for (auto* c : p.extras) ex += (dynamic_cast<ComboBox*> (c) ? 92 : 78) + 6;
            p.r.setWidth (jmax (need, 24 + ex)); sum += p.r.getWidth();
        }
        const int avail = W - 2 * m - gap * ((int) ps.size() - 1);
        const int add = (avail - sum) / (int) ps.size();
        int x = m;
        for (auto* p : ps)
        {
            p->r = { x, y0 + row * (rowH + gap), p->r.getWidth() + add, rowH };
            x += p->r.getWidth() + gap;
            int kx = p->r.getX() + 12;
            for (auto* k : p->knobs)
            {
                k->s.setBounds (kx, p->r.getY() + 36, 74, 86);
                k->l.setBounds (kx, p->r.getY() + 122, 74, 16);
                kx += 78;
            }
            int ex = p->r.getX() + 12;
            for (auto* c : p->extras)
            {
                const int w = dynamic_cast<ComboBox*> (c) ? 92 : 78;
                c->setBounds (ex, p->r.getY() + 140, w, 22); ex += w + 6;
            }
        }
    }
    keys.setBounds (m, H - 90, W - 2 * m, 70);
}

void TraperetorEditor::paint (Graphics& g)
{
    const Colour steel (0xffaeb1b4), ink = IndustrialLAF::ink();
    g.fillAll (Colour (0xff7b7e82));

    // hazard stripe
    {
        Graphics::ScopedSaveState ss (g);
        g.reduceClipRegion (20, 16, W - 40, 14);
        g.fillAll (IndustrialLAF::plate());
        g.setColour (IndustrialLAF::orange());
        for (int x = -20; x < W; x += 28)
        {
            Path p; p.addQuadrilateral ((float) x, 30.0f, (float) x + 14, 30.0f, (float) x + 28, 16.0f, (float) x + 14, 16.0f);
            g.fillPath (p);
        }
    }
    g.setColour (ink);
    g.setFont (Font (FontOptions (44.0f, Font::bold)));
    g.drawText ("TRAPERETOR", 20, 30, 600, 46, Justification::centredLeft);

    auto drawPanel = [&] (Rectangle<int> r, const String& title)
    {
        g.setColour (steel); g.fillRect (r);
        g.setColour (Colour (0x10ffffff));
        for (int x = r.getX(); x < r.getRight(); x += 3) g.drawVerticalLine (x, (float) r.getY(), (float) r.getBottom());
        g.setColour (ink); g.drawRect (r, 2);
        g.setColour (Colour (0xff2b2c2e));
        for (int sx : { r.getX() + 9, r.getRight() - 9 }) for (int sy : { r.getY() + 9, r.getBottom() - 9 })
            g.fillEllipse ((float) sx - 3, (float) sy - 3, 6.0f, 6.0f);
        g.setFont (Font (FontOptions (15.0f, Font::bold)));
        const int tw = (int) GlyphArrangement::getStringWidth (g.getCurrentFont(), title) + 22;
        g.setColour (IndustrialLAF::plate()); g.fillRect (r.getX() + 22, r.getY() + 7, tw, 20);
        g.setColour (Colour (0xffdcdde0)); g.drawText (title, r.getX() + 22, r.getY() + 7, tw, 20, Justification::centred);
    };

    drawPanel ({ 20, 70, W - 40, 180 }, "ONESHOT BANK");
    for (auto& p : panels) drawPanel (p.r, p.title);

    if (dragging)
    {
        g.setColour (Colour (0xdde8590c)); g.fillAll();
        g.setColour (Colours::white); g.setFont (Font (FontOptions (56.0f, Font::bold)));
        g.drawText ("DROP YOUR ONESHOTS", getLocalBounds(), Justification::centred);
    }
}
