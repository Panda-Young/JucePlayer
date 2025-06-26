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

    if (juce::RuntimePermissions::isRequired(juce::RuntimePermissions::writeExternalStorage) &&
        !juce::RuntimePermissions::isGranted(juce::RuntimePermissions::writeExternalStorage)) {
        juce::RuntimePermissions::request(
            juce::RuntimePermissions::writeExternalStorage,
            [this](bool granted) {
                if (granted) {
                    LOGD("WRITE_EXTERNAL_STORAGE permission granted");
                } else {
                    LOGE("WRITE_EXTERNAL_STORAGE permission denied");
                    juce::AlertWindow::showMessageBoxAsync(
                        juce::AlertWindow::WarningIcon,
                        "Permission Required",
                        "WRITE_EXTERNAL_STORAGE permission is required to save playlist data.",
                        "OK");
                }
            });
    } else {
        LOGD("WRITE_EXTERNAL_STORAGE permission already granted");
    }

    initializeAfterPermissionGranted();
}

void MainComponent::initializeAfterPermissionGranted()
{

    PlaylistDataManager playlistDataManager;

    playlistItems = playlistDataManager.loadPlaylist();
    LOGD("Loaded %d playlist items from file", playlistItems.size());

    if (playlistItems.isEmpty()) {
        LOGD("No playlist data found, scanning for music files");
        FileScanner fileScanner;
        playlistItems = fileScanner.scanForMusicFiles(juce::File::getSpecialLocation(juce::File::userMusicDirectory));
        LOGD("Found %d music files", playlistItems.size());

        playlistDataManager.savePlaylist(playlistItems);
        LOGD("Saved playlist data to file");
    }

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

    playlistBox.setModel(this);

    setSize(800, 600);

    setAudioChannels(0, 2);

    formatManager.registerBasicFormats();
    LOGD("Audio format manager initialized");
    LOGD("MainComponent initialized successfully");
}

MainComponent::~MainComponent()
{
    shutdownAudio();
}

void MainComponent::prepareToPlay(int samplesPerBlockExpected, double sampleRate)
{
    if (currentAudioFile.existsAsFile()) {

        juce::AudioFormatReader *reader = formatManager.createReaderFor(currentAudioFile);

        if (reader != nullptr) {

            currentAudioSource.reset(new juce::AudioFormatReaderSource(reader, true));

            currentAudioSource->prepareToPlay(samplesPerBlockExpected, sampleRate);
            LOGD("Prepared to play audio: %s", currentAudioFile.getFileName().toStdString().c_str());
        } else {
            LOGE("Failed to create audio reader for: %s", currentAudioFile.getFullPathName().toStdString().c_str());
        }
    }
}

void MainComponent::getNextAudioBlock(const juce::AudioSourceChannelInfo &bufferToFill)
{
    if (isPlaying && currentAudioSource != nullptr) {
        currentAudioSource->getNextAudioBlock(bufferToFill);
    } else {
        bufferToFill.clearActiveBufferRegion();
    }
}

void MainComponent::releaseResources()
{

    currentAudioSource.reset();
}

void MainComponent::paint(juce::Graphics &g)
{
    g.fillAll(getLookAndFeel().findColour(juce::ResizableWindow::backgroundColourId));

    if (coverImage.getImage().isNull()) {
        g.setColour(juce::Colours::white);
        g.fillRect(coverImage.getBounds());
    }
}

void MainComponent::resized()
{
    auto area = getLocalBounds();

    auto topArea = area.removeFromTop(270);

    int coverSize = 150;
    int padding = 20;

    int coverX = (topArea.getWidth() - coverSize) / 2;
    coverImage.setBounds(coverX, padding, coverSize, coverSize);

    int labelWidth = topArea.getWidth() - 2 * padding;
    int labelHeight = 30;
    int labelY = coverImage.getBottom() + padding;

    titleLabel.setBounds(padding, labelY, labelWidth, labelHeight);
    artistLabel.setBounds(padding, titleLabel.getBottom(), labelWidth, labelHeight);

    auto bottomArea = area.removeFromBottom(100);
    progressSlider.setBounds(bottomArea.removeFromTop(30));

    auto controlArea = bottomArea.removeFromTop(60);
    int buttonWidth = controlArea.getWidth() / 5;

    loopButton.setBounds(controlArea.removeFromLeft(buttonWidth));
    prevButton.setBounds(controlArea.removeFromLeft(buttonWidth));
    playPauseButton.setBounds(controlArea.removeFromLeft(buttonWidth));
    nextButton.setBounds(controlArea.removeFromLeft(buttonWidth));
    playlistButton.setBounds(controlArea.removeFromLeft(buttonWidth));

    if (isPlaylistVisible) {
        playlistBox.setBounds(area);
        playlistBox.setVisible(true);
    } else {
        playlistBox.setVisible(false);
    }
}

void MainComponent::buttonClicked(juce::Button *button)
{
    if (button == &playPauseButton) {
        isPlaying = !isPlaying;
        playPauseButton.setButtonText(isPlaying ? "Pause" : "Play");

        if (isPlaying && currentAudioSource == nullptr && currentAudioFile.existsAsFile()) {

            prepareToPlay(512, 44100.0);
        }
    } else if (button == &prevButton) {

    } else if (button == &nextButton) {

    } else if (button == &loopButton) {

    } else if (button == &playlistButton) {
        isPlaylistVisible = !isPlaylistVisible;
        playlistBox.setVisible(isPlaylistVisible);
        resized();
    }
}

void MainComponent::sliderValueChanged(juce::Slider *slider)
{
    if (slider == &progressSlider) {
    }
}

int MainComponent::getNumRows()
{
    return playlistItems.size();
}

void MainComponent::paintListBoxItem(int rowNumber, juce::Graphics &g, int width, int height, bool rowIsSelected)
{
    if (rowIsSelected)
        g.fillAll(juce::Colours::lightblue);

    g.setColour(juce::Colours::black);
    g.drawText(playlistItems[rowNumber], 0, 0, width, height, juce::Justification::centredLeft);
}

void MainComponent::listBoxItemClicked(int row, const juce::MouseEvent &)
{
    if (row >= 0 && row < playlistItems.size()) {
        currentRow = row;
        juce::String selectedTrack = playlistItems[row];
        titleLabel.setText(selectedTrack, juce::dontSendNotification);

        juce::File audioFile(selectedTrack);
        if (audioFile.existsAsFile()) {
            currentAudioFile = audioFile;
            LOGD("Selected audio file: %s", audioFile.getFullPathName().toStdString().c_str());

            isPlaying = false;
            playPauseButton.setButtonText("Play");
            currentAudioSource.reset();
            prepareToPlay(512, 44100.0);
        } else {
            LOGE("Selected file does not exist: %s", audioFile.getFullPathName().toStdString().c_str());
        }
    }
}
