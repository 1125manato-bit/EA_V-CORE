#include "PluginEditor.h"
#include "PluginProcessor.h"

EAVCOREAudioProcessorEditor::EAVCOREAudioProcessorEditor(
    EAVCOREAudioProcessor &p)
    : AudioProcessorEditor(&p), audioProcessor(p) {
  background = juce::ImageCache::getFromMemory(BinaryData::background_jpg,
                                               BinaryData::background_jpgSize);

  if (background.isValid())
    setSize((int)(background.getWidth() * 0.75f),
            (int)(background.getHeight() * 0.75f)); // 1.5x of previous (0.5)
  else
    setSize(600, 750); // Scaled up fallback

  // Setup Knob
  mainKnob.setSliderStyle(juce::Slider::RotaryHorizontalVerticalDrag);
  mainKnob.setTextBoxStyle(juce::Slider::NoTextBox, false, 0, 0);
  mainKnob.setLookAndFeel(&customLookAndFeel);
  addAndMakeVisible(mainKnob);

  mainKnobAttachment =
      std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(
          audioProcessor.apvts, "main_knob", mainKnob);
}

EAVCOREAudioProcessorEditor::~EAVCOREAudioProcessorEditor() {
  mainKnob.setLookAndFeel(nullptr);
}

void EAVCOREAudioProcessorEditor::paint(juce::Graphics &g) {
  if (background.isValid())
    g.drawImage(background, getLocalBounds().toFloat());
  else
    g.fillAll(juce::Colours::black);
}

void EAVCOREAudioProcessorEditor::resized() {
  auto bounds = getLocalBounds();
  auto center = bounds.getCentre();

  // Knob Size - Scaled 1.5x from 220 -> 330
  int knobSize = 330;

  // Position - Shifted down, scaled 1.5x from 20 -> 30
  int yOffset = 30;

  mainKnob.setBounds(center.getX() - knobSize / 2,
                     center.getY() - knobSize / 2 + yOffset, knobSize,
                     knobSize);
}
