#pragma once
#include <JuceHeader.h>
#include <cmath>

namespace DSP {
class W1Limiter {
public:
  void prepare(const juce::dsp::ProcessSpec &spec) {
    sampleRate = spec.sampleRate;
    // 5ms lookahead
    lookaheadSamples = (int)(0.005 * sampleRate);
    ringBuffer.setSize(2, lookaheadSamples + 1024);
    ringBuffer.clear();
    writePos = 0;

    // Release time: Adaptive usually, but let's set a safe 200ms base for
    // smooth vocal
    releaseCoef = std::exp(-1.0 / (0.2 * sampleRate));

    currentGain = 1.0f;
  }

  void reset() {
    ringBuffer.clear();
    writePos = 0;
    currentGain = 1.0f;
  }

  void setThreshold(float dB) {
    // Unused for ceiling-based limiting, but if linked...
    // Logic handled in Engine usually.
  }

  void setCeiling(float dB) { ceilingLin = juce::Decibels::decibelsToGain(dB); }

  void process(juce::dsp::AudioBlock<float> &block) {
    auto numSamples = block.getNumSamples();
    auto numChannels = block.getNumChannels();

    int startWritePos = writePos; // Sync write pos for logic

    // Check max peak in the input block to drive envelop?
    // Better: Sample by sample lookahead limiter

    // To do proper lookahead limiting with shared gain reduction across
    // channels:
    // 1. Find max input peak at Lookahead point
    // 2. Update Gain Reduction
    // 3. Apply Gain Reduction to Delayed signal

    auto *channel0 = block.getChannelPointer(0);
    auto *channel1 = (numChannels > 1) ? block.getChannelPointer(1) : nullptr;

    auto *rb0 = ringBuffer.getWritePointer(0);
    auto *rb1 = ringBuffer.getWritePointer(1);
    int rbSize = ringBuffer.getNumSamples();

    int localWritePos = startWritePos;

    for (size_t i = 0; i < numSamples; ++i) {
      float in0 = channel0[i];
      float in1 = (channel1) ? channel1[i] : in0;

      // 1. Write to Lookahead Buffer
      rb0[(localWritePos + i) % rbSize] = in0;
      rb1[(localWritePos + i) % rbSize] = in1;

      // 2. Read from Lookahead (Delayed Signal) to be output implemented later?
      // No, Limiter logic needs to know the peak *ahead* of time.
      // The "Input" to the gain computer is the signal we just wrote (Future).
      // The "Output" signal to apply gain to is the signal from the past (Now).

      // Peak of the "Future" signal
      float maxIn = std::max(std::abs(in0), std::abs(in1));

      // Desired Gain to keep this Future signal under ceiling
      float desiredGain = 1.0f;
      if (maxIn > ceilingLin) {
        desiredGain = ceilingLin / maxIn;
      }

      // Attack / Release Logic
      // If we need to reduce gain (desired < current), attack is instant (or
      // very fast) If we can increase gain (desired > current), release is slow

      if (desiredGain < currentGain) {
        currentGain = desiredGain; // Instant attack to catch peak
      } else {
        currentGain = desiredGain + releaseCoef * (currentGain - desiredGain);
      }

      // 3. Apply Current Gain to the "Past" (Output) signal
      int readIndex = (localWritePos + i - lookaheadSamples + rbSize) % rbSize;

      float delayed0 = rb0[readIndex];
      float delayed1 = rb1[readIndex];

      channel0[i] = delayed0 * currentGain;
      if (channel1)
        channel1[i] = delayed1 * currentGain;
    }

    writePos = (writePos + (int)numSamples) % ringBuffer.getNumSamples();
  }

private:
  double sampleRate = 44100.0;
  int lookaheadSamples = 0;

  juce::AudioBuffer<float> ringBuffer;
  int writePos = 0;

  float ceilingLin = 0.891f; // -1.0dB

  float currentGain = 1.0f;
  double releaseCoef = 0.9995;
};
} // namespace DSP
