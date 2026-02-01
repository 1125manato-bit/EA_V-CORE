#pragma once
#include <JuceHeader.h>

namespace DSP {
class StereoWidener {
public:
  StereoWidener() {
    // Crossover at 2kHz
    lpFilter.setType(juce::dsp::LinkwitzRileyFilterType::lowpass);
    hpFilter.setType(juce::dsp::LinkwitzRileyFilterType::highpass);
    lpFilter.setCutoffFrequency(2000.0f);
    hpFilter.setCutoffFrequency(2000.0f);
  }

  void prepare(const juce::dsp::ProcessSpec &spec) {
    sampleRate = spec.sampleRate;
    lpFilter.prepare(spec);
    hpFilter.prepare(spec);

    // Max delay 20ms
    delayBuffer.setSize(2, (int)(spec.sampleRate * 0.02) + 1);
    delayBuffer.clear();
    writeIndex = 0;
  }

  void reset() {
    lpFilter.reset();
    hpFilter.reset();
    delayBuffer.clear();
    writeIndex = 0;
  }

  void setWidth(float newWidth) {
    widthAmount = newWidth; // 0.0 to 1.0
  }

  void process(juce::dsp::AudioBlock<float> &block) {
    if (widthAmount < 0.01f)
      return;

    auto numSamples = block.getNumSamples();
    auto numChannels = block.getNumChannels();

    // We need a temporary buffer for HP processing since filters process
    // in-place and we need both LP and HP separated. Using a scratch buffer or
    // copying the block.

    // Allocate scratch buffer on stack or member? Member is safer for
    // performance. For now, let's assume we can allocate a temporary helper.
    juce::AudioBuffer<float> hpBuffer(numChannels, (int)numSamples);

    // Copy data to hpBuffer
    for (size_t ch = 0; ch < numChannels; ++ch) {
      juce::FloatVectorOperations::copy(hpBuffer.getWritePointer((int)ch),
                                        block.getChannelPointer(ch),
                                        (int)numSamples);
    }

    // Process LP on original block
    juce::dsp::ProcessContextReplacing<float> context(block);
    lpFilter.process(context); // block now has LP only

    // Process HP on copy
    juce::dsp::AudioBlock<float> hpBlock(hpBuffer);
    juce::dsp::ProcessContextReplacing<float> hpContext(hpBlock);
    hpFilter.process(hpContext); // hpBuffer now has HP only

    // Apply delay to HP buffer and add back to block
    // L: 5ms, R: 8ms
    int delaySamplesL = (int)(0.005 * sampleRate);
    int delaySamplesR = (int)(0.008 * sampleRate);

    auto *delayL = delayBuffer.getWritePointer(0);
    auto *delayR = delayBuffer.getWritePointer(1);
    int delayLen = delayBuffer.getNumSamples();

    const auto *srcL = hpBuffer.getReadPointer(0);
    const auto *srcR = hpBuffer.getReadPointer(1 > numChannels - 1 ? 0 : 1);

    auto *dstL = block.getChannelPointer(0);
    auto *dstR = block.getChannelPointer(1 > numChannels - 1 ? 0 : 1);

    // We need to match writeIndex across calls, so use member
    int localWriteIndex = writeIndex;

    for (size_t i = 0; i < numSamples; ++i) {
      // Write to delay line
      delayL[localWriteIndex] = srcL[i];
      delayR[localWriteIndex] = srcR[i];

      // Read from delay line
      int readIndexL = (localWriteIndex - delaySamplesL + delayLen) % delayLen;
      int readIndexR = (localWriteIndex - delaySamplesR + delayLen) % delayLen;

      float delayedL = delayL[readIndexL];
      float delayedR = delayR[readIndexR];

      // Add to LP signal in dst (block)
      // We must add the DRY HP signal back, otherwise we lose high frequencies!
      // Result = LP (already in dst) + Dry HP + Wet HP (Width)

      dstL[i] += srcL[i] + (delayedL * widthAmount);
      dstR[i] += srcR[i] + (delayedR * widthAmount);

      localWriteIndex = (localWriteIndex + 1) % delayLen;
    }

    writeIndex = localWriteIndex;
  }

private:
  double sampleRate = 44100.0;
  float widthAmount = 0.0f;

  juce::dsp::LinkwitzRileyFilter<float> lpFilter;
  juce::dsp::LinkwitzRileyFilter<float> hpFilter;

  juce::AudioBuffer<float> delayBuffer;
  int writeIndex = 0;
};
} // namespace DSP
