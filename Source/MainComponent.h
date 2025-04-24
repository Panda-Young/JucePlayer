#pragma once

#include <JuceHeader.h>

//==============================================================================
class MainComponent : public juce::AudioAppComponent,
                     public juce::Button::Listener,
                     public juce::Slider::Listener,
                     public juce::ListBoxModel // Add this
{
public:
    //==============================================================================
    MainComponent();
    ~MainComponent() override;

    //==============================================================================
    void prepareToPlay(int samplesPerBlockExpected, double sampleRate) override;
    void getNextAudioBlock(const juce::AudioSourceChannelInfo& bufferToFill) override;
    void releaseResources() override;

    //==============================================================================
    void paint(juce::Graphics& g) override;
    void resized() override;

    //==============================================================================
    void buttonClicked(juce::Button* button) override;
    void sliderValueChanged(juce::Slider* slider) override;

    //==============================================================================
    // ListBoxModel overrides
    int getNumRows() override;
    void paintListBoxItem(int rowNumber, juce::Graphics& g, int width, int height, bool rowIsSelected) override;
    void listBoxItemClicked(int row, const juce::MouseEvent&) override;

private:
    //==============================================================================
    // UI Components
    juce::ImageComponent coverImage;
    juce::Label titleLabel, artistLabel;
    juce::Slider progressSlider;
    juce::TextButton loopButton, prevButton, playPauseButton, nextButton, playlistButton;
    juce::ListBox playlistBox;

    // Audio State
    bool isPlaying = false;

    // Playlist Data
    juce::StringArray playlistItems; // Store playlist items

    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(MainComponent)
};