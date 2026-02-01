#pragma once
#include <JuceHeader.h>

class CustomLookAndFeel : public juce::LookAndFeel_V4 {
public:
  CustomLookAndFeel() {
    knobImage = juce::ImageCache::getFromMemory(BinaryData::knob_png,
                                                BinaryData::knob_pngSize);
  }

  void drawRotarySlider(juce::Graphics &g, int x, int y, int width, int height,
                        float sliderPos, const float rotaryStartAngle,
                        const float rotaryEndAngle,
                        juce::Slider &slider) override {
    // sliderPos is 0.0 to 1.0
    // We want to map this to our steps 0..4
    // Logic:
    // Step 0 (0.0): +135 deg
    // Step 2 (0.5): 0 deg
    // Step 4 (1.0): -135 deg

    // However, Slider usually maps min->max to start->end.
    // We will specificially ignore start/end and use our custom angle logic.

    // Calculate step (0 to 4)
    float value = slider.getValue(); // Assumed 0 to 4

    // Angle in degrees: (2 - value) * 67.5
    float angleDeg = (2.0f - value) * 67.5f;
    float angleRad = juce::degreesToRadians(angleDeg);

    if (knobImage.isValid()) {
      auto bounds = juce::Rectangle<int>(x, y, width, height).toFloat();
      auto center = bounds.getCentre();

      // Draw image rotated
      juce::AffineTransform transform = juce::AffineTransform::rotation(
          angleRad, center.getX(), center.getY());

      // Scale if necessary to fit?
      // Assuming image is high res, scale to fit bounds.
      float scale =
          juce::jmin(bounds.getWidth() / (float)knobImage.getWidth(),
                     bounds.getHeight() / (float)knobImage.getHeight());

      // Center the image scaling
      // We want to pivot around center of image too
      auto imageBounds = knobImage.getBounds().toFloat();
      auto imageCentre = imageBounds.getCentre();

      // Transform: Move image center to 0,0 -> Scale -> Rotate -> Move to
      // bounds center
      transform = juce::AffineTransform::translation(-imageCentre.getX(),
                                                     -imageCentre.getY())
                      .scaled(scale)
                      .rotated(angleRad)
                      .translated(center.getX(), center.getY());

      g.drawImageTransformed(knobImage, transform);
    } else {
      // Fallback
      g.setColour(juce::Colours::orange);
      g.fillEllipse(x, y, width, height);
    }
  }

private:
  juce::Image knobImage;
};
