#pragma once
#include "Saturator.h"
#include "StereoWidener.h"
#include "W1Limiter.h"
#include <JuceHeader.h>

namespace DSP {
class VCoreEngine {
public:
  VCoreEngine()
      : oversampling(2, 2,
                     juce::dsp::Oversampling<
                         float>::filterHalfBandPolyphaseIIR) // Factor 4x (2^2)
  {}

  void prepare(const juce::dsp::ProcessSpec &spec) {
    sampleRate = spec.sampleRate;

    oversampling.reset();
    oversampling.initProcessing(spec.maximumBlockSize);

    auto osSpec = spec;
    osSpec.sampleRate *= 4.0; // 4x Oversampling

    saturator.prepare(osSpec);
    widener.prepare(osSpec);
    limiter.prepare(osSpec);
  }

  void reset() {
    oversampling.reset();
    saturator.reset();
    widener.reset();
    limiter.reset();
  }

  void setParameters(int modeIndex) {
    float thresholdDB = 0.0f;
    float width = 0.0f;
    float saturationDrive = 0.0f;
    float makeupGainDB = 0.0f;

    switch (modeIndex) {
    case 0: // BYPASS / CLEAN
      thresholdDB = 0.0f;
      width = 0.0f;
      saturationDrive = 0.0f;
      makeupGainDB = 0.0f;
      break;
    case 1: // NATURAL
      thresholdDB = -3.0f;
      width = 0.10f;
      saturationDrive = 0.1f;
      makeupGainDB = 2.0f;
      break;
    case 2: // LIVE / STREAM
      thresholdDB = -6.0f;
      width = 0.25f;
      saturationDrive = 0.2f;
      makeupGainDB = 5.0f;
      break;
    case 3: // VOCAL / POWER
      thresholdDB = -9.0f;
      width = 0.40f;
      saturationDrive = 0.3f;
      makeupGainDB = 8.0f;
      break;
    case 4: // BROADCAST
      thresholdDB = -12.0f;
      width = 0.55f;
      saturationDrive = 0.4f;
      makeupGainDB = 11.0f;
      break;
    }

    saturator.setDrive(saturationDrive);
    widener.setWidth(width);
    limiter.setThreshold(thresholdDB);

    currentMakeupGain = juce::Decibels::decibelsToGain(makeupGainDB);
  }

  void process(juce::AudioBuffer<float> &buffer) {
    juce::dsp::AudioBlock<float> block(buffer);
    auto osBlock = oversampling.processSamplesUp(block);

    // 1. Saturation
    juce::dsp::ProcessContextReplacing<float> satContext(osBlock);
    saturator.process(satContext);

    // 2. Stereo Widener (Now accepts block)
    widener.process(osBlock);

    // 3. Makeup Gain
    osBlock.multiplyBy(currentMakeupGain);

    // 4. Limiter (Now accepts block)
    limiter.process(osBlock);

    oversampling.processSamplesDown(block);
  }

private:
  double sampleRate = 44100.0;
  juce::dsp::Oversampling<float> oversampling;

  Saturator saturator;
  StereoWidener widener;
  W1Limiter limiter;

  float currentMakeupGain = 1.0f;
};
} // namespace DSP
