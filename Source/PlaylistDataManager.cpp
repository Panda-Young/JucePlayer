#include "PlaylistDataManager.h"
#include "Logger.h"

PlaylistDataManager::PlaylistDataManager() {}

juce::File PlaylistDataManager::getPlaylistFile()
{
    juce::File playlistFile = juce::File::getSpecialLocation(juce::File::userDocumentsDirectory).getChildFile("PlaylistData.txt");
    LOGD("Playlist file path: %s", playlistFile.getFullPathName().toRawUTF8());
    return playlistFile;
}

void PlaylistDataManager::savePlaylist(const juce::StringArray& playlistItems)
{
    juce::File playlistFile = getPlaylistFile();
    LOGD("Saving playlist to file: %s", playlistFile.getFullPathName().toRawUTF8());

    juce::String data = playlistItems.joinIntoString("\n");
    bool success = playlistFile.replaceWithText(data);
    if (success)
    {
        LOGD("Playlist saved successfully. Item count: %d", playlistItems.size());
    }
    else
    {
        LOGE("Failed to save playlist to file.");
    }
}

juce::StringArray PlaylistDataManager::loadPlaylist()
{
    juce::File playlistFile = getPlaylistFile();
    LOGD("Loading playlist from file: %s", playlistFile.getFullPathName().toRawUTF8());

    if (playlistFile.existsAsFile())
    {
        juce::String fileContent = playlistFile.loadFileAsString();
        juce::StringArray playlistItems = juce::StringArray::fromLines(fileContent);
        LOGD("Loaded playlist successfully. Item count: %d", playlistItems.size());
        return playlistItems;
    }
    else
    {
        LOGD("Playlist file does not exist.");
        return juce::StringArray();
    }
}