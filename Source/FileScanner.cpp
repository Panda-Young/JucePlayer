#include "FileScanner.h"
#include "Logger.h"

juce::StringArray FileScanner::scanForMusicFiles(const juce::File &directory)
{
    juce::StringArray musicFiles;
    LOGD("Scanning directory: %s", directory.getFullPathName().toRawUTF8());

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
        juce::String extension = file.getFileExtension().toLowerCase();
        if (supportedFormats.contains(extension)) {
            LOGD("Found music file: %s", file.getFullPathName().toRawUTF8());
            musicFiles.add(file.getFullPathName()); // Add full path of the file
        } else {
            LOGD("Unsupported format: %s", extension.toRawUTF8());
        }
    }

    LOGD("Filtered %d music files", musicFiles.size());
    return musicFiles;
}
