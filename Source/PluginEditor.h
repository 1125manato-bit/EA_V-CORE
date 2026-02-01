#pragma once

#include "PluginProcessor.h"
#include "UI/CustomLookAndFeel.h"
#include <JuceHeader.h>

class EAVCOREAudioProcessorEditor : public juce::AudioProcessorEditor {
public:
  EAVCOREAudioProcessorEditor(EAVCOREAudioProcessor &);
  ~EAVCOREAudioProcessorEditor() override;

  void paint(juce::Graphics &) override;
  void resized() override;

private:
  EAVCOREAudioProcessor &audioProcessor;

  juce::Image background;

  CustomLookAndFeel customLookAndFeel;
  juce::Slider mainKnob;
  std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment>
      mainKnobAttachment;

  JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(EAVCOREAudioProcessorEditor)
};
