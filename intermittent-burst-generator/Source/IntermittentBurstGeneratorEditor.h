/*
  ==============================================================================

    IntermittentBurstGeneratorEditor.h
    Created: 3rd December 2024
    Author:  Sumedh Sopan Nagrale

  ==============================================================================
*/

#ifndef IntermittentBurstGeneratorEDITOR_H_INCLUDED
#define IntermittentBurstGeneratorEDITOR_H_INCLUDED

#include <EditorHeaders.h>


/**

 User interface for the "IntermittentBurstGenerator" node.

 @see IntermittentBurstGenerator

 */

class IntermittentBurstGeneratorEditor
    : public GenericEditor
    , public Label::Listener
    , public Button::Listener
{
public:
    IntermittentBurstGeneratorEditor(GenericProcessor* parentNode, bool useDefaultParameterEditors);

    void labelTextChanged(juce::Label* label) override;

    void buttonClicked(Button* button) override;

    void saveCustomParameters(XmlElement* xml) override;
    void loadCustomParameters(XmlElement* xml) override;

private:
    ScopedPointer<Label> inChanLabel;
    ScopedPointer<ComboBox> inChanBox;

    ScopedPointer<ComboBox> pinBox;

    ScopedPointer<UtilityButton> fileButton;
    //ScopedPointer<Label> file;
    ScopedPointer<Label> fileNameLabel;
    ScopedPointer<Label> pulsewidthLabel;
    ScopedPointer<Label> pulsewidthText;
    ScopedPointer<Label> ttlpulseText;
    ScopedPointer<Label> ttlShamDurationText;
    ScopedPointer<Label> ttlStimNumberText;
    ScopedPointer<Label> ttlpulseLabel;
    ScopedPointer<Label> ttlShamDurationLabel;
    ScopedPointer<Label> ttlStimNumberLabel;
    File lastFilePath;
};


#endif  // EVENTBROADCASTEREDITOR_H_INCLUDED
