#pragma once
#include <JuceHeader.h>

class FileScanner
{
public:
    FileScanner();
    juce::Array<juce::File> scanForMusicFiles(const juce::File& directory);
    juce::StringArray extractFileInfo(const juce::File& file);

private:
    juce::StringArray supportedFormats = { ".mp3", ".wav", ".flac" };
};
