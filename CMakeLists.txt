cmake_minimum_required(VERSION 3.22)

project(EA_V-CORE VERSION 1.0.0)

include(FetchContent)
FetchContent_Declare(
    juce
    GIT_REPOSITORY https://github.com/juce-framework/JUCE.git
    GIT_TAG        8.0.4
)
FetchContent_MakeAvailable(juce)

juce_add_plugin(EA_V-CORE
    COMPANY_NAME "EMU AUDIO"
    IS_SYNTH FALSE
    NEEDS_MIDI_INPUT FALSE
    NEEDS_MIDI_OUTPUT FALSE
    IS_MIDI_EFFECT FALSE
    EDITOR_WANTS_KEYBOARD_FOCUS TRUE
    COPY_PLUGIN_AFTER_BUILD TRUE
    PLUGIN_MANUFACTURER_CODE Educ
    PLUGIN_CODE Vcor
    FORMATS AU VST3
    PRODUCT_NAME "EA V-CORE"
    VERSION "1.0.0"
    BUNDLE_ID "com.emuaudio.eavcore"
)

target_sources(EA_V-CORE
    PRIVATE
        Source/PluginProcessor.cpp
        Source/PluginProcessor.h
        Source/PluginEditor.cpp
        Source/PluginEditor.h
)

juce_add_binary_data(SharedAssets
    SOURCES
        Resources/background.jpg
        Resources/knob.png
)

target_link_libraries(EA_V-CORE
    PRIVATE
        juce::juce_audio_utils
        juce::juce_audio_processors
        juce::juce_dsp
        juce::juce_gui_basics
        juce::juce_graphics
        SharedAssets
)

target_compile_features(EA_V-CORE PRIVATE cxx_std_20)

juce_generate_juce_header(EA_V-CORE)

if(APPLE)
    target_link_options(EA_V-CORE PRIVATE "-Wl,-ld_classic")
endif()
