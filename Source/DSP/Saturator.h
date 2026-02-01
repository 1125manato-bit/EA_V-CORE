#pragma once
#include <JuceHeader.h>

namespace DSP {
class Saturator {
public:
  void prepare(const juce::dsp::ProcessSpec &spec) {
    sampleRate = spec.sampleRate;
  }

  void reset() {}

  void setDrive(float newDrive) { drive = newDrive; }

  template <typename ProcessContext>
  void process(const ProcessContext &context) {
    auto &&inputBlock = context.getInputBlock();
    auto &&outputBlock = context.getOutputBlock();

    for (size_t ch = 0; ch < outputBlock.getNumChannels(); ++ch) {
      auto *src = inputBlock.getChannelPointer(ch);
      auto *dst = outputBlock.getChannelPointer(ch);

      for (size_t i = 0; i < outputBlock.getNumSamples(); ++i) {
        // Gentle tanh saturation
        // Input is boosted by drive, then saturated
        float x = src[i] * (1.0f + drive * 2.0f);
        dst[i] = std::tanh(x);
      }
    }
  }

private:
  double sampleRate = 44100.0;
  float drive = 0.0f; // 0.0 to 1.0
};
} // namespace DSP
