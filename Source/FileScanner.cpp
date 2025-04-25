#include "FileScanner.h"
#include "Logger.h"

FileScanner::FileScanner() {}

juce::Array<juce::File> FileScanner::scanForMusicFiles(const juce::File &directory)
{
    juce::Array<juce::File> musicFiles;
    LOGD("Scanning directory: %s", directory.getFullPathName().toRawUTF8());

    // Supported audio formats
    juce::StringArray supportedFormats = {".mp3", ".wav", ".flac", ".ogg", ".m4a"};
    LOGD("Supported formats: %s", supportedFormats.joinIntoString(", ").toRawUTF8());

    // Find all files in the directory
    juce::Array<juce::File> allFiles;
    bool foundFiles = directory.findChildFiles(allFiles, juce::File::findFiles, true);
    if (!foundFiles || allFiles.isEmpty()) {
        LOGD("No files found in directory: %s", directory.getFullPathName().toRawUTF8());
        return musicFiles;
    }

    LOGD("Found %d files in directory", allFiles.size());

    // Filter files by supported formats
    for (const auto &file : allFiles) {
        juce::String fileName = file.getFileName();
        juce::String extension = file.getFileExtension().toLowerCase();
        LOGD("Find file %s", fileName.toRawUTF8());

        if (supportedFormats.contains(extension)) {
            musicFiles.add(file);
        } else {
            LOGD("Unsupported format: %s", extension.toRawUTF8());
        }
    }

    LOGD("Filtered %d music files", musicFiles.size());
    return musicFiles;
}

juce::StringArray FileScanner::extractFileInfo(const juce::File &file)
{
    juce::StringArray info;
    juce::String fileName = file.getFileNameWithoutExtension();
    juce::String filePath = file.getFullPathName();
    info.add(fileName);         // Title
    info.add("Unknown Artist"); // Default artist
    info.add(filePath);         // File path
    return info;
}
