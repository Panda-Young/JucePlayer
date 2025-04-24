#include "FileScanner.h"
#include "Logger.h"

FileScanner::FileScanner() {}

juce::Array<juce::File> FileScanner::scanForMusicFiles(const juce::File &directory)
{
    juce::Array<juce::File> musicFiles;
    LOGD("Scanning directory: %s", directory.getFullPathName().toRawUTF8());
    for (const auto &format : supportedFormats) {
        LOGD("Looking for files with format: %s", format.toRawUTF8());
        directory.findChildFiles(musicFiles, juce::File::findFiles, true, "*" + format);
    }
    LOGD("Found %d music files", musicFiles.size());
    return musicFiles;
}

juce::StringArray FileScanner::extractFileInfo(const juce::File &file)
{
    juce::StringArray info;
    info.add(file.getFileNameWithoutExtension()); // Title
    info.add("Unknown Artist");                   // Default artist
    info.add(file.getFullPathName());             // File path
    return info;
}
