#include "PlaylistDataManager.h"

PlaylistDataManager::PlaylistDataManager() {}

juce::File PlaylistDataManager::getPlaylistFile()
{
    return juce::File::getSpecialLocation(juce::File::userDocumentsDirectory).getChildFile("PlaylistData.txt");
}

void PlaylistDataManager::savePlaylist(const juce::StringArray& playlistItems)
{
    juce::File playlistFile = getPlaylistFile();
    juce::String data = playlistItems.joinIntoString("\n");
    playlistFile.replaceWithText(data);
}

juce::StringArray PlaylistDataManager::loadPlaylist()
{
    juce::File playlistFile = getPlaylistFile();
    if (playlistFile.existsAsFile())
    {
        return juce::StringArray::fromLines(playlistFile.loadFileAsString());
    }
    return juce::StringArray();
}
