#include "PluginProcessor.h"
#include "PluginEditor.h"

EAVCOREAudioProcessor::EAVCOREAudioProcessor()
#ifndef JucePlugin_PreferredChannelConfigurations
    : AudioProcessor(
          BusesProperties()
#if !JucePlugin_IsMidiEffect
#if !JucePlugin_IsSynth
              .withInput("Input", juce::AudioChannelSet::stereo(), true)
#endif
              .withOutput("Output", juce::AudioChannelSet::stereo(), true)
#endif
              ),
      apvts(*this, nullptr, "Parameters", createParameterLayout())
#endif
{
}

EAVCOREAudioProcessor::~EAVCOREAudioProcessor() {}

const juce::String EAVCOREAudioProcessor::getName() const {
  return "EA V-CORE";
}

bool EAVCOREAudioProcessor::acceptsMidi() const {
#if JucePlugin_WantsMidiInput
  return true;
#else
  return false;
#endif
}

bool EAVCOREAudioProcessor::producesMidi() const {
#if JucePlugin_ProducesMidiOutput
  return true;
#else
  return false;
#endif
}

bool EAVCOREAudioProcessor::isMidiEffect() const {
#if JucePlugin_IsMidiEffect
  return true;
#else
  return false;
#endif
}

double EAVCOREAudioProcessor::getTailLengthSeconds() const { return 0.0; }

int EAVCOREAudioProcessor::getNumPrograms() {
  return 1; // NB: some hosts don't cope very well if you tell them there are 0
            // programs, so this should be at least 1, even if you're not really
            // implementing programs.
}

int EAVCOREAudioProcessor::getCurrentProgram() { return 0; }

void EAVCOREAudioProcessor::setCurrentProgram(int index) {}

const juce::String EAVCOREAudioProcessor::getProgramName(int index) {
  return {};
}

void EAVCOREAudioProcessor::changeProgramName(int index,
                                              const juce::String &newName) {}

void EAVCOREAudioProcessor::prepareToPlay(double sampleRate,
                                          int samplesPerBlock) {
  juce::dsp::ProcessSpec spec;
  spec.sampleRate = sampleRate;
  spec.maximumBlockSize = samplesPerBlock;
  spec.numChannels = getTotalNumOutputChannels();

  vCoreEngine.prepare(spec);
}

void EAVCOREAudioProcessor::releaseResources() { vCoreEngine.reset(); }

#ifndef JucePlugin_PreferredChannelConfigurations
bool EAVCOREAudioProcessor::isBusesLayoutSupported(
    const BusesLayout &layouts) const {
#if JucePlugin_IsMidiEffect
  juce::ignoreUnused(layouts);
  return true;
#else
  if (layouts.getMainOutputChannelSet() != juce::AudioChannelSet::mono() &&
      layouts.getMainOutputChannelSet() != juce::AudioChannelSet::stereo())
    return false;

#if !JucePlugin_IsSynth
  if (layouts.getMainOutputChannelSet() != layouts.getMainInputChannelSet())
    return false;
#endif

  return true;
#endif
}
#endif

void EAVCOREAudioProcessor::processBlock(juce::AudioBuffer<float> &buffer,
                                         juce::MidiBuffer &midiMessages) {
  juce::ScopedNoDenormals noDenormals;
  auto totalNumInputChannels = getTotalNumInputChannels();
  auto totalNumOutputChannels = getTotalNumOutputChannels();

  for (auto i = totalNumInputChannels; i < totalNumOutputChannels; ++i)
    buffer.clear(i, 0, buffer.getNumSamples());

  // Update Parameters from APVTS
  // This is thread-safe for reading raw values
  auto *knobParam = apvts.getRawParameterValue("main_knob");
  if (knobParam != nullptr) {
    // Round to nearest integer for step
    int mode = std::round(knobParam->load());
    vCoreEngine.setParameters(mode);
  }

  // Process Audio
  vCoreEngine.process(buffer);
}

bool EAVCOREAudioProcessor::hasEditor() const { return true; }

juce::AudioProcessorEditor *EAVCOREAudioProcessor::createEditor() {
  return new EAVCOREAudioProcessorEditor(*this);
}

void EAVCOREAudioProcessor::getStateInformation(juce::MemoryBlock &destData) {
  auto state = apvts.copyState();
  std::unique_ptr<juce::XmlElement> xml(state.createXml());
  copyXmlToBinary(*xml, destData);
}

void EAVCOREAudioProcessor::setStateInformation(const void *data,
                                                int sizeInBytes) {
  std::unique_ptr<juce::XmlElement> xmlState(
      getXmlFromBinary(data, sizeInBytes));

  if (xmlState.get() != nullptr)
    if (xmlState->hasTagName(apvts.state.getType()))
      apvts.replaceState(juce::ValueTree::fromXml(*xmlState));
}

juce::AudioProcessorValueTreeState::ParameterLayout
EAVCOREAudioProcessor::createParameterLayout() {
  juce::AudioProcessorValueTreeState::ParameterLayout layout;

  // 5 Steps: 0 to 4
  layout.add(
      std::make_unique<juce::AudioParameterInt>("main_knob", // parameterID
                                                "Mode",      // parameter name
                                                0,           // minimum value
                                                4,           // maximum value
                                                0            // default value
                                                ));

  return layout;
}

// This creates new instances of the plugin..
juce::AudioProcessor *JUCE_CALLTYPE createPluginFilter() {
  return new EAVCOREAudioProcessor();
}
