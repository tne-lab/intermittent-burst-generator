/*
  ==============================================================================

    IntermittentBurstGeneratorEditor.h
    Created: 7th December 2024
    Author:  Sumedh Sopan Nagrale

  ==============================================================================
*/

#include "IntermittentBurstGeneratorEditor.h"
#include "IntermittentBurstGenerator.h"

bool isInteger(const String& input) {
    // Check if the string can be converted to an integer
    return input.containsOnly("0123456789");
}

IntermittentBurstGeneratorEditor::IntermittentBurstGeneratorEditor(GenericProcessor* parentNode, bool useDefaultParameterEditors)
    : GenericEditor(parentNode, useDefaultParameterEditors)

{
    desiredWidth = 215;

    const int HTextBox = 14;
    const int HFonts = 14;
    int xPos = 15;
    int yPos = 25;
    int yUpadate = 20;
    int XTextBox = 125;
    int WTextBox = 35;
    int wlabel = 60;
    int wlabelS = 140;
    IntermittentBurstGenerator* p = (IntermittentBurstGenerator*) getProcessor();

    // STIM IN
    stimInText = new Label("StimInText", "STIM IN:");
        stimInText->setFont(Font("Default", HFonts, Font::plain));
        stimInText->setBounds(xPos, yPos, wlabel, HTextBox);
        addAndMakeVisible(stimInText);

    stimInLabel = new Label("StimInLabel", String(p->stimEventChannelIn));
        stimInLabel->setBounds(xPos + wlabel, yPos, WTextBox, HTextBox);
        stimInLabel->setFont(Font("Default", HFonts, Font::plain));
        stimInLabel->setColour(Label::textColourId, Colours::white);
        stimInLabel->setColour(Label::backgroundColourId, Colours::grey);
        stimInLabel->setTooltip("Input stimulation event channel");
        stimInLabel->setEditable(true);
        stimInLabel->addListener(this);
        addAndMakeVisible(stimInLabel);

    // STIM OUT
    stimOutText = new Label("StimOutText", "OUT:");
    stimOutText->setFont(Font("Default", HFonts, Font::plain));
    stimOutText->setBounds(xPos + 100, yPos, wlabel, HTextBox);
    addAndMakeVisible(stimOutText);

    stimOutLabel = new Label("StimOutLabel", String(p->stimEventChannelOut));
        stimOutLabel->setBounds(xPos + 80 + wlabel, yPos, WTextBox, HTextBox);
        stimOutLabel->setFont(Font("Default", HFonts, Font::plain));
        stimOutLabel->setColour(Label::textColourId, Colours::white);
        stimOutLabel->setColour(Label::backgroundColourId, Colours::grey);
        stimOutLabel->setTooltip("Output stimulation event channel");
        stimOutLabel->setEditable(true);
        stimOutLabel->addListener(this);
        addAndMakeVisible(stimOutLabel);

    // Update position for the next group
    yPos += yUpadate;

    // SHAM IN
    shamInText = new Label("ShamInText", "SHAM IN:");
    shamInText->setFont(Font("Default", HFonts, Font::plain));
    shamInText->setBounds(xPos, yPos, wlabel, HTextBox);
    addAndMakeVisible(shamInText);

    shamInLabel = new Label("ShamInLabel", String(p->shamEventChannelIn));
        shamInLabel->setBounds(xPos + wlabel, yPos, WTextBox, HTextBox);
        shamInLabel->setFont(Font("Default", HFonts, Font::plain));
        shamInLabel->setColour(Label::textColourId, Colours::white);
        shamInLabel->setColour(Label::backgroundColourId, Colours::grey);
        shamInLabel->setTooltip("Input sham event channel");
        shamInLabel->setEditable(true);
        shamInLabel->addListener(this);
        addAndMakeVisible(shamInLabel);

    // SHAM OUT
    shamOutText = new Label("ShamOutText", "OUT:");
        shamOutText->setBounds(xPos + 100, yPos, wlabel, HTextBox);
        shamOutText->setFont(Font("Default", HFonts, Font::plain));
        addAndMakeVisible(shamOutText);

    shamOutLabel = new Label("ShamOutLabel", String(p->shamEventChannelOut));
        shamOutLabel->setBounds(xPos + 80 + wlabel, yPos, WTextBox, HTextBox);
        shamOutLabel->setFont(Font("Default", HFonts, Font::plain));
        shamOutLabel->setColour(Label::textColourId, Colours::white);
        shamOutLabel->setColour(Label::backgroundColourId, Colours::grey);
        shamOutLabel->setTooltip("Output stimulation event channel");
        shamOutLabel->setEditable(true);
        shamOutLabel->addListener(this);
        addAndMakeVisible(shamOutLabel);

    // Update position
    yPos += yUpadate;

    pulsewidthText = new Label("PWPulseText", "Pulse Width (ms):");
        pulsewidthText->setBounds(xPos, yPos, wlabelS, HTextBox);
        pulsewidthText->setFont(Font("Default", HFonts, Font::plain));
        addAndMakeVisible(pulsewidthText);
    pulsewidthLabel = new Label("pulsewidthLabel", String(p->pulsewidth));
        pulsewidthLabel->setBounds(xPos + XTextBox + 15, yPos, WTextBox, HTextBox);
        pulsewidthLabel->setFont(Font("Default", HFonts, Font::plain));
        pulsewidthLabel->setColour(Label::textColourId, Colours::white);
        pulsewidthLabel->setColour(Label::backgroundColourId, Colours::grey);
        pulsewidthLabel->setTooltip("Pulse Width in milliseconds.");
        pulsewidthLabel->setEditable(true);
        pulsewidthLabel->addListener(this);
        addAndMakeVisible(pulsewidthLabel);
        yPos = yPos + yUpadate;
       
    ttlShamDurationText = new Label("ttlShamDurationText", "Sham Duration (s):");
        ttlShamDurationText->setBounds(xPos, yPos, wlabelS, HTextBox);
        ttlShamDurationText->setFont(Font("Default", HFonts, Font::plain));
        ttlShamDurationText->setColour(Label::textColourId, Colours::black);
        addAndMakeVisible(ttlShamDurationText);

    ttlShamDurationLabel = new Label("ttlShamDurationLabel", String(p->shamDuration));
        ttlShamDurationLabel->setBounds(xPos + XTextBox + 15, yPos, WTextBox, HTextBox);
        ttlShamDurationLabel->setTooltip("Sham pulses for duration in seconds.");
        ttlShamDurationLabel->setFont(Font("Default", HFonts, Font::plain));
        ttlShamDurationLabel->setColour(Label::textColourId, Colours::white);
        ttlShamDurationLabel->setColour(Label::backgroundColourId, Colours::grey);
        ttlShamDurationLabel->setEditable(true);
        ttlShamDurationLabel->addListener(this);
        addAndMakeVisible(ttlShamDurationLabel);
    yPos = yPos + yUpadate;

    ttlStimNumberText = new Label("ttlStimNumberText", "Stim Pulse:");
        ttlStimNumberText->setBounds(xPos, yPos, wlabelS, HTextBox);
        ttlStimNumberText->setFont(Font("Default", HFonts, Font::plain));
        ttlStimNumberText->setColour(Label::textColourId, Colours::black);
        addAndMakeVisible(ttlStimNumberText);
    ttlStimNumberLabel = new Label("ttlStimNumberLabel", String(p->stimPulseNum));
        ttlStimNumberLabel->setBounds(xPos + XTextBox + 15, yPos, WTextBox, HTextBox);
        ttlStimNumberLabel->setFont(Font("Default", HFonts, Font::plain));
        ttlStimNumberLabel->setColour(Label::textColourId, Colours::white);
        ttlStimNumberLabel->setColour(Label::backgroundColourId, Colours::grey);
        ttlStimNumberLabel->setEditable(true);
        ttlStimNumberLabel->addListener(this);
        addAndMakeVisible(ttlStimNumberLabel);

}


void IntermittentBurstGeneratorEditor::labelTextChanged(juce::Label* label)
{
    auto processor = static_cast<IntermittentBurstGenerator*>(getProcessor());
    if (label == ttlpulseLabel)
    {
        String text = label->getText();
        if (isInteger(text))
        {
            processor->setParameter(IntermittentBurstGenerator::TTLPULSE, text.getFloatValue());
        }
    }
    else if (label == ttlShamDurationLabel)
    {
        String text = label->getText();
        if (isInteger(text))
        {
            processor->setParameter(IntermittentBurstGenerator::SHAMDURATION, text.getFloatValue());
        }
    }
    else if (label == ttlStimNumberLabel)
    {
        String text = label->getText();
        if (isInteger(text))
        {
            processor->setParameter(IntermittentBurstGenerator::STIMPULSENUM, text.getFloatValue());
        }
    }
    else if (label == stimInLabel)
    {
        String text = label->getText();
        if (isInteger(text))
        {
            processor->setParameter(IntermittentBurstGenerator::STIMEVENTCHANNELIN, text.getFloatValue());
        }
    }
    else if (label == stimOutLabel)
    {
        String text = label->getText();
        if (isInteger(text))
        {
            processor->setParameter(IntermittentBurstGenerator::STIMEVENTCHANNELOUT, text.getFloatValue());
        }
    }
    else if (label == shamInLabel)
    {
        String text = label->getText();
        if (isInteger(text))
        {
            processor->setParameter(IntermittentBurstGenerator::SHAMEVENTCHANNELIN, text.getFloatValue());
        }
    }
    else if (label == shamOutLabel)
    {
        String text = label->getText();
        if (isInteger(text))
        {
            processor->setParameter(IntermittentBurstGenerator::SHAMEVENTCHANNELOUT, text.getFloatValue());
        }
    }
    else if (label == pulsewidthLabel)
    {
        String text = label->getText();
        processor->setParameter(IntermittentBurstGenerator::PULSEWIDTH, text.getIntValue());
    }
}

