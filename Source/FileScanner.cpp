#include "FileScanner.h"

FileScanner::FileScanner() {}

juce::Array<juce::File> FileScanner::scanForMusicFiles(const juce::File& directory)
{
    juce::Array<juce::File> musicFiles;
    for (const auto& format : supportedFormats)
    {
        directory.findChildFiles(musicFiles, juce::File::findFiles, true, "*" + format);
    }
    return musicFiles;
}

juce::StringArray FileScanner::extractFileInfo(const juce::File& file)
{
    juce::StringArray info;
    info.add(file.getFileNameWithoutExtension()); // Title
    info.add("Unknown Artist"); // Default artist
    info.add(file.getFullPathName()); // File path
    return info;
}
