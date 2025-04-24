#pragma once
#include <JuceHeader.h>

class PlaylistDataManager
{
public:
    PlaylistDataManager();
    void savePlaylist(const juce::StringArray& playlistItems);
    juce::StringArray loadPlaylist();

private:
    juce::File getPlaylistFile();
};
