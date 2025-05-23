#include "MainComponent.h"
#include "FileScanner.h"
#include "Logger.h"
#include "PlaylistDataManager.h"
MainComponent::MainComponent()
{
    LOGD("Initializing MainComponent");

    if (juce::RuntimePermissions::isRequired(juce::RuntimePermissions::readMediaAudio) &&
        !juce::RuntimePermissions::isGranted(juce::RuntimePermissions::readMediaAudio)) {
        juce::RuntimePermissions::request(
            juce::RuntimePermissions::readMediaAudio,
            [this](bool granted) {
                if (granted) {
                    LOGD("READ_MEDIA_AUDIO permission granted");
                } else {
                    LOGE("READ_MEDIA_AUDIO permission denied");
                    juce::AlertWindow::showMessageBoxAsync(
                        juce::AlertWindow::WarningIcon,
                        "Permission Required",
                        "READ_MEDIA_AUDIO permission is required to scan for music files. Please grant the permission in the app settings.",
                        "OK");
                }
            });
    } else {
        LOGD("READ_MEDIA_AUDIO permission already granted");
    }

    if (juce::RuntimePermissions::isRequired(juce::RuntimePermissions::readExternalStorage) &&
        !juce::RuntimePermissions::isGranted(juce::RuntimePermissions::readExternalStorage)) {
        juce::RuntimePermissions::request(
            juce::RuntimePermissions::readExternalStorage,
            [this](bool granted) {
                if (granted) {
                    LOGD("READ_EXTERNAL_STORAGE permission granted");
                } else {
                    LOGE("READ_EXTERNAL_STORAGE permission denied");
                    juce::AlertWindow::showMessageBoxAsync(
                        juce::AlertWindow::WarningIcon,
                        "Permission Required",
                        "READ_EXTERNAL_STORAGE permission is required to scan for music files. Please grant the permission in the app settings.",
                        "OK");
                }
            });
    } else {
        LOGD("READ_EXTERNAL_STORAGE permission already granted");
    }
    initializeAfterPermissionGranted();
}

void MainComponent::initializeAfterPermissionGranted()
{
    // Initialize playlist data manager
    PlaylistDataManager playlistDataManager;

    // Load playlist from file
    playlistItems = playlistDataManager.loadPlaylist();
    LOGD("Loaded %d playlist items from file", playlistItems.size());

    // If no playlist data, scan for music files and save to file
    if (playlistItems.isEmpty()) {
        LOGD("No playlist data found, scanning for music files");
        FileScanner fileScanner;
        playlistItems = fileScanner.scanForMusicFiles(juce::File::getSpecialLocation(juce::File::userMusicDirectory));
        LOGD("Found %d music files", playlistItems.size());

        playlistDataManager.savePlaylist(playlistItems);
        LOGD("Saved playlist data to file");
    }

    // Set up UI components
    addAndMakeVisible(coverImage);
    addAndMakeVisible(titleLabel);
    addAndMakeVisible(artistLabel);
    addAndMakeVisible(progressSlider);
    addAndMakeVisible(loopButton);
    addAndMakeVisible(prevButton);
    addAndMakeVisible(playPauseButton);
    addAndMakeVisible(nextButton);
    addAndMakeVisible(playlistBox);
    addAndMakeVisible(playlistButton);

    // Configure components
    titleLabel.setText("Audio Title", juce::dontSendNotification);
    titleLabel.setJustificationType(juce::Justification::centred);
    artistLabel.setText("Artist Name", juce::dontSendNotification);
    artistLabel.setJustificationType(juce::Justification::centred);

    progressSlider.setRange(0.0, 1.0);
    progressSlider.setSliderStyle(juce::Slider::LinearHorizontal);
    progressSlider.setTextBoxStyle(juce::Slider::NoTextBox, false, 0, 0);
    progressSlider.addListener(this);

    loopButton.setButtonText("Loop");
    loopButton.addListener(this);

    prevButton.setButtonText("Prev");
    prevButton.addListener(this);

    playPauseButton.setButtonText("Play");
    playPauseButton.addListener(this);

    nextButton.setButtonText("Next");
    nextButton.addListener(this);

    playlistButton.setButtonText("Playlist");
    playlistButton.addListener(this);

    // Initialize playlist data
    playlistBox.setModel(this); // Set the model

    // Set component size
    setSize(800, 600);

    // Set up audio
    setAudioChannels(0, 2);
    // 初始化音频格式管理器
    formatManager.registerBasicFormats(); // 注册常见音频格式解码器
    LOGD("Audio format manager initialized");
    LOGD("MainComponent initialized successfully");
}

MainComponent::~MainComponent()
{
    shutdownAudio();
}

//==============================================================================
void MainComponent::prepareToPlay(int samplesPerBlockExpected, double sampleRate)
{
    if (currentAudioFile.existsAsFile())
    {
        // 创建音频读取器（直接使用原始指针）
        juce::AudioFormatReader* reader = formatManager.createReaderFor(currentAudioFile);
        
        if (reader != nullptr)
        {
            // 使用原始指针初始化 AudioFormatReaderSource
            currentAudioSource.reset(new juce::AudioFormatReaderSource(reader, true));
            
            // 设置音频源参数
            currentAudioSource->prepareToPlay(samplesPerBlockExpected, sampleRate);
            LOGD("Prepared to play audio: %s", currentAudioFile.getFileName().toStdString().c_str());
        }
        else
        {
            LOGE("Failed to create audio reader for: %s", currentAudioFile.getFullPathName().toStdString().c_str());
        }
    }
}

void MainComponent::getNextAudioBlock(const juce::AudioSourceChannelInfo& bufferToFill)
{
    if (isPlaying && currentAudioSource != nullptr)
    {
        currentAudioSource->getNextAudioBlock(bufferToFill);
    }
    else
    {
        bufferToFill.clearActiveBufferRegion();
    }
}

void MainComponent::releaseResources()
{
    // Release audio resources
    currentAudioSource.reset();
}

//==============================================================================
void MainComponent::paint(juce::Graphics &g)
{
    g.fillAll(getLookAndFeel().findColour(juce::ResizableWindow::backgroundColourId));

    // Draw a white background for the cover image if no image is set
    if (coverImage.getImage().isNull()) {
        g.setColour(juce::Colours::white);
        g.fillRect(coverImage.getBounds());
    }
}

void MainComponent::resized()
{
    auto area = getLocalBounds();

    // Top area: Cover, Title, Artist
    auto topArea = area.removeFromTop(270);

    // Center the cover image, title, and artist
    int coverSize = 150; // Size of the cover image
    int padding = 20;    // Padding between components

    // Calculate the center position for the cover image
    int coverX = (topArea.getWidth() - coverSize) / 2;
    coverImage.setBounds(coverX, padding, coverSize, coverSize);

    // Calculate the position for the title and artist labels
    int labelWidth = topArea.getWidth() - 2 * padding;
    int labelHeight = 30;
    int labelY = coverImage.getBottom() + padding;

    titleLabel.setBounds(padding, labelY, labelWidth, labelHeight);
    artistLabel.setBounds(padding, titleLabel.getBottom(), labelWidth, labelHeight);

    // Bottom area: Progress bar and controls
    auto bottomArea = area.removeFromBottom(100);
    progressSlider.setBounds(bottomArea.removeFromTop(30));

    // Control buttons area
    auto controlArea = bottomArea.removeFromTop(60);
    int buttonWidth = controlArea.getWidth() / 5; // Divide width equally among 5 buttons

    loopButton.setBounds(controlArea.removeFromLeft(buttonWidth));
    prevButton.setBounds(controlArea.removeFromLeft(buttonWidth));
    playPauseButton.setBounds(controlArea.removeFromLeft(buttonWidth));
    nextButton.setBounds(controlArea.removeFromLeft(buttonWidth));
    playlistButton.setBounds(controlArea.removeFromLeft(buttonWidth));

    if (isPlaylistVisible) {
        playlistBox.setBounds(area);
        playlistBox.setVisible(true); // Ensure it's visible
    } else {
        playlistBox.setVisible(false); // Hide it
    }
}

//==============================================================================
void MainComponent::buttonClicked(juce::Button *button)
{
    if (button == &playPauseButton) {
        isPlaying = !isPlaying;
        playPauseButton.setButtonText(isPlaying ? "Pause" : "Play");
        
        if (isPlaying && currentAudioSource == nullptr && currentAudioFile.existsAsFile()) {
            // 重新准备音频
            prepareToPlay(512, 44100.0); // 使用合适的缓冲区大小和采样率
        }
    } 
    else if (button == &prevButton) {
        // Handle previous track
    } else if (button == &nextButton) {
        // Handle next track
    } else if (button == &loopButton) {
        // Handle loop mode
    } else if (button == &playlistButton) {
        isPlaylistVisible = !isPlaylistVisible;    // Toggle visibility
        playlistBox.setVisible(isPlaylistVisible); // Set visibility
        resized();                                 // Re-layout components
    }
}

void MainComponent::sliderValueChanged(juce::Slider *slider)
{
    if (slider == &progressSlider) {
        // Handle progress change
    }
}

//==============================================================================
// ListBoxModel overrides
int MainComponent::getNumRows()
{
    return playlistItems.size(); // Return the number of items in the playlist
}

void MainComponent::paintListBoxItem(int rowNumber, juce::Graphics &g, int width, int height, bool rowIsSelected)
{
    if (rowIsSelected)
        g.fillAll(juce::Colours::lightblue); // Highlight selected row

    g.setColour(juce::Colours::black);
    g.drawText(playlistItems[rowNumber], 0, 0, width, height, juce::Justification::centredLeft);
}

void MainComponent::listBoxItemClicked(int row, const juce::MouseEvent&)
{
    if (row >= 0 && row < playlistItems.size())
    {
        currentRow = row;
        juce::String selectedTrack = playlistItems[row];
        titleLabel.setText(selectedTrack, juce::dontSendNotification);
        
        // 假设播放列表项是文件路径，实际需根据数据结构调整
        juce::File audioFile(selectedTrack);
        if (audioFile.existsAsFile())
        {
            currentAudioFile = audioFile;
            LOGD("Selected audio file: %s", audioFile.getFullPathName().toStdString().c_str());
            
            // 停止当前播放并准备新文件
            isPlaying = false;
            playPauseButton.setButtonText("Play");
            currentAudioSource.reset(); // 释放旧资源
            prepareToPlay(512, 44100.0); // 准备新文件
        }
        else
        {
            LOGE("Selected file does not exist: %s", audioFile.getFullPathName().toStdString().c_str());
        }
    }
}
