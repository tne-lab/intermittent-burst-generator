/*
  ==============================================================================

    IntermittentBurstGeneratorEditor.h
    Created: 7th December 2024
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
{
public:
    IntermittentBurstGeneratorEditor(GenericProcessor* parentNode, bool useDefaultParameterEditors);

    void labelTextChanged(juce::Label* label) override;

private:
    ScopedPointer<Label> stimInLabel;
    ScopedPointer<Label> stimInText;
    ScopedPointer<Label> shamInLabel;
    ScopedPointer<Label> shamInText;

    ScopedPointer<Label> stimOutLabel;
    ScopedPointer<Label> stimOutText;
    ScopedPointer<Label> shamOutLabel;
    ScopedPointer<Label> shamOutText;

    ScopedPointer<Label> pulsewidthLabel;
    ScopedPointer<Label> pulsewidthText;
    ScopedPointer<Label> ttlpulseText;
    ScopedPointer<Label> ttlShamDurationText;
    ScopedPointer<Label> ttlStimNumberText;
    ScopedPointer<Label> ttlpulseLabel;
    ScopedPointer<Label> ttlShamDurationLabel;
    ScopedPointer<Label> ttlStimNumberLabel;

};


#endif  // EVENTBROADCASTEREDITOR_H_INCLUDED
