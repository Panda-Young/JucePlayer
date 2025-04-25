#pragma once
#include <JuceHeader.h>

class FileScanner
{
public:
    FileScanner() = default;
    juce::StringArray scanForMusicFiles(const juce::File& directory);

private:
    juce::StringArray supportedFormats = { ".mp3", ".wav", ".flac", ".ogg", ".m4a" };
};
