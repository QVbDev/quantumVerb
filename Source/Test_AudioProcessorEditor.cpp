/*
==============================================================================

Test_AudioProcessorEditor.cpp

==============================================================================
*/

#include "catch.hpp"
#include "PluginEditor.h"

#include "../JuceLibraryCode/modules/juce_audio_plugin_client/Standalone/juce_StandaloneFilterWindow.h"


/**
* How to write tests with Catch:
* https://github.com/catchorg/Catch2/blob/2bbba4f5444b7a90fcba92562426c14b11e87b76/docs/tutorial.md#writing-tests
*/

class MockAudioProcessorEditor : public juce::Component, private juce::Value::Listener,
    juce::Button::Listener,
    juce::ComponentListener
{
public:
    MockAudioProcessorEditor(reverb::AudioProcessor& processor)
        : owner(processor), notification(this),
        editor((processor.createEditorIfNeeded())),
        shouldShowNotification(false)
    {
        if (editor != nullptr)
        {
            editor->addComponentListener(this);
            componentMovedOrResized(*editor, false, true);

            addAndMakeVisible(editor);
        }

        addChildComponent(notification);
    }

    ~MockAudioProcessorEditor()
    {
        if (editor != nullptr)
        {
            editor->removeComponentListener(this);
            owner.editorBeingDeleted(editor);
            editor = nullptr;
        }
    }

    void resized() override
    {
        auto r = getLocalBounds();

        if (shouldShowNotification)
            notification.setBounds(r.removeFromTop(NotificationArea::height));

        editor->setBounds(r);
    }

private:
    //==============================================================================
    class NotificationArea : public Component
    {
    public:
        enum { height = 30 };

        NotificationArea(juce::Button::Listener* settingsButtonListener)
            : notification("notification", "Audio input is muted to avoid feedback loop"),
            settingsButton("Settings...")
        {
            setOpaque(true);

            notification.setColour(juce::Label::textColourId, juce::Colours::black);

            settingsButton.addListener(settingsButtonListener);

            addAndMakeVisible(notification);
            addAndMakeVisible(settingsButton);
        }

        void paint(juce::Graphics& g) override
        {
            auto r = getLocalBounds();

            g.setColour(juce::Colours::darkgoldenrod);
            g.fillRect(r.removeFromBottom(1));

            g.setColour(juce::Colours::lightgoldenrodyellow);
            g.fillRect(r);
        }

        void resized() override
        {
            auto r = getLocalBounds().reduced(5);

            settingsButton.setBounds(r.removeFromRight(70));
            notification.setBounds(r);
        }
    private:
        juce::Label notification;
        juce::TextButton settingsButton;
    };

    //==============================================================================
    void componentMovedOrResized(Component&, bool, bool wasResized) override
    {
        if (wasResized && editor != nullptr)
            setSize(editor->getWidth(),
                editor->getHeight() + (shouldShowNotification ? NotificationArea::height : 0));
    }

    //==============================================================================
    reverb::AudioProcessor& owner;
    NotificationArea notification;
    juce::ScopedPointer<reverb::AudioProcessorEditor> editor;
    bool shouldShowNotification;
};

//inline StandalonePluginHolder* StandalonePluginHolder::getInstance()
//{
//#if JucePlugin_Enable_IAA || JucePlugin_Build_Standalone
//    if (PluginHostType::getPluginLoadedAs() == AudioProcessor::wrapperType_Standalone)
//    {
//        auto& desktop = Desktop::getInstance();
//        const int numTopLevelWindows = desktop.getNumComponents();
//
//        for (int i = 0; i < numTopLevelWindows; ++i)
//            if (auto window = dynamic_cast<StandaloneFilterWindow*> (desktop.getComponent(i)))
//                return window->getPluginHolder();
//    }
//#endif
//
//    return nullptr;
//}

juce::Component* recursivelyFindComponentWithID(juce::Component* currentBase, const juce::String& componentID)
{
    if (currentBase->getComponentID() == componentID) {
        return currentBase;
    }

    int numChildren = currentBase->getNumChildComponents();

    for (int componentIndex = 0; componentIndex < numChildren; ++componentIndex)
    {
        auto c = currentBase->getChildComponent(componentIndex);
        auto result = recursivelyFindComponentWithID(c, componentID);

        if (result)
            return result;
    }

    return nullptr;
}

bool clickButton(juce::Component * rootComponent, const juce::String& buttonComponentId)
{
    if (!rootComponent) {
        return false;
    }

    auto component = recursivelyFindComponentWithID(rootComponent, buttonComponentId);

    auto button = dynamic_cast<juce::Button*>(component);

    if (!button) {
        return false;
    }

    button->triggerClick();
    return true;
}

TEST_CASE("Audio Processor Editor is tested", "[AudioProcessorEditor]") {
    const juce::String APPLICATION_NAME = "QuantumReverb GUI Testing";
    //JUCE_TRY
    //{
    //    // Loop until a quit message is received.
    //    juce::MessageManager::getInstance()->runDispatchLoop();
    //}
    //JUCE_CATCH_EXCEPTION

	reverb::AudioProcessor processor;
    juce::MessageManager::getInstance();
    juce::AudioProcessorEditor* editor = processor.createEditor();
    // reverb::AudioProcessorEditor* editor = dynamic_cast<reverb::AudioProcessorEditor*>(processor.createEditor());
    // reverb::AudioProcessorEditor editor(processor);
    REQUIRE(processor.hasEditor() == true);
    REQUIRE(editor->isOnDesktop() == false);
    editor->setOpaque(true);
    editor->setVisible(true);
    editor->setTopLeftPosition(0, 0);
    editor->addToDesktop(0);
    editor->setAlwaysOnTop(true);
    editor->toFront(true);
    REQUIRE(editor->isOnDesktop() == true);
    // editor->setOpaque(true);
    // editor->setVisible(true);
    // editor->setTopLeftPosition(0, 0);
    // editor->addToDesktop(0);
    // editor->setAlwaysOnTop(true);
    // juce::Colour(0xffeedd)
    juce::DialogWindow::showModalDialog("Quantum Reverb GUI Testing", editor, nullptr, juce::Colour(0xffeedd), true, false, false);
    juce::TopLevelWindow* mainWindow = juce::TopLevelWindow::getActiveTopLevelWindow();
    mainWindow->setVisible(true);

    juce::PropertiesFile::Options options;
    options.applicationName = APPLICATION_NAME;
    options.filenameSuffix = ".settings";
    options.osxLibrarySubFolder = "Application Support";
    options.folderName = "";

    juce::ApplicationProperties appProperties;
    appProperties.setStorageParameters(options);
    // Other option:
    juce::PropertiesFile* userProperties = new juce::PropertiesFile(options);
    //juce::ScopedPointer<juce::StandaloneFilterWindow> window = new juce::StandaloneFilterWindow(APPLICATION_NAME,
    //    juce::LookAndFeel::getDefaultLookAndFeel().findColour(juce::ResizableWindow::backgroundColourId),
    //    userProperties, false, {}, nullptr);
	//reverb::AudioProcessorEditor editor(processor);
    //editor.setOpaque(true);
    //editor.setVisible(true);
    //editor.setTopLeftPosition(0, 0);
    //editor.addToDesktop(0);
    //editor.setAlwaysOnTop(true);
    juce::Time::waitForMillisecondCounter(juce::Time::getMillisecondCounter() + 2000);
}
