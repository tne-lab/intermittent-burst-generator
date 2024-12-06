/*
  ==============================================================================

    IntermittentBurstGeneratorEditor.h
    Created: 3rd December 2024
    Author:  Sumedh Sopan Nagrale

  ==============================================================================
*/

#include "IntermittentBurstGeneratorEditor.h"
#include "IntermittentBurstGenerator.h"


bool isHexString(const String& input) {
    for (int i = 0; i < input.length(); ++i) {
        char c = input[i];
        if (!isxdigit(c)) {
            return false;
        }
    }
    return !input.isEmpty();
}

bool isInteger(const String& input) {
    // Check if the string can be converted to an integer
    return input.containsOnly("0123456789");
}

IntermittentBurstGeneratorEditor::IntermittentBurstGeneratorEditor(GenericProcessor* parentNode, bool useDefaultParameterEditors)
    : GenericEditor(parentNode, useDefaultParameterEditors)

{
    desiredWidth = 235;

    const int TEXT_HT = 18;
    int xPos = 5;
    int yPos = 35;
    int yUpadate = 20;
    int XTextBox = 125;
    int WTextBox = 35;
    IntermittentBurstGenerator* p = (IntermittentBurstGenerator*) getProcessor();

    Label* fileDisplayText = new Label("Channel mapping file", "Channel map");
        fileDisplayText->setFont(Font("Small Text", 10, Font::plain));
        fileDisplayText->setBounds(xPos, yPos+5, 80, 8); // (110, 95, 100, 8)
        fileDisplayText->setColour(Label::textColourId, Colours::darkgrey);
        addAndMakeVisible(fileDisplayText);

    fileButton = new UtilityButton("F:", Font("Small Text", 10, Font::bold));
        fileButton->setRadius(5.0f);
        fileButton->setBounds(xPos + 80, yPos , 20, 15); // (115, 105, 20, 20);
        fileButton->addListener(this);
        addAndMakeVisible(fileButton);

    fileNameLabel = new Label("Selected file", "No file selected");
        fileNameLabel->setFont(Font("Small Text", 10, Font::plain));
        fileNameLabel->setBounds(xPos + 110, yPos, 100, 15); // (140, 105, 70, 20);
        fileNameLabel->setEditable(true);
        fileNameLabel->setEnabled(false);
        fileNameLabel->setColour(Label::backgroundColourId, Colours::lightgrey);
        fileNameLabel->addListener(this);
        addAndMakeVisible(fileNameLabel);
        yPos = yPos + yUpadate;
        xPos = xPos + 10;
    pulsewidthText = new Label("PWPulseText", "Pulse Width (ms):");
        pulsewidthText->setBounds(xPos, yPos, 140, 25);
        addAndMakeVisible(pulsewidthText);
    pulsewidthLabel = new Label("pulsewidthLabel", String(p->pulsewidth));
        pulsewidthLabel->setBounds(xPos + XTextBox, yPos, WTextBox, 18);
        pulsewidthLabel->setFont(Font("Default", 15, Font::plain));
        pulsewidthLabel->setColour(Label::textColourId, Colours::white);
        pulsewidthLabel->setColour(Label::backgroundColourId, Colours::grey);
        pulsewidthLabel->setTooltip("Minimum value should be greater than (1/Sampling rate)");
        pulsewidthLabel->setEditable(true);
        pulsewidthLabel->addListener(this);
        addAndMakeVisible(pulsewidthLabel);
        yPos = yPos + yUpadate;

    ttlpulseText = new Label("TTLPulseText", "TTL Pulse :");
        ttlpulseText->setBounds(xPos, yPos, 140, 25);
        ttlpulseText->setColour(Label::textColourId, Colours::black);
        //addAndMakeVisible(ttlpulseText);
        //yPos = yPos + yUpadate;
    ttlpulseLabel = new Label("ttlpulseLabel", String(p->ttlpulse));
        ttlpulseLabel->setBounds(xPos + XTextBox, yPos, WTextBox, TEXT_HT);
        ttlpulseLabel->setFont(Font("Default", 15, Font::plain));
        ttlpulseLabel->setColour(Label::textColourId, Colours::white);
        ttlpulseLabel->setColour(Label::backgroundColourId, Colours::grey);
        ttlpulseLabel->setEditable(true);
        ttlpulseLabel->addListener(this);
        //addAndMakeVisible(ttlpulseLabel);
       
    ttlShamDurationText = new Label("ttlShamDurationText", "Sham Duration (s) :");
        ttlShamDurationText->setBounds(xPos, yPos, 140, 25);
        ttlShamDurationText->setColour(Label::textColourId, Colours::black);
        addAndMakeVisible(ttlShamDurationText);

    ttlShamDurationLabel = new Label("ttlShamDurationLabel", String(p->shamDuration));
        ttlShamDurationLabel->setBounds(xPos + XTextBox, yPos, WTextBox, TEXT_HT);
        ttlShamDurationLabel->setFont(Font("Default", 15, Font::plain));
        ttlShamDurationLabel->setColour(Label::textColourId, Colours::white);
        ttlShamDurationLabel->setColour(Label::backgroundColourId, Colours::grey);
        ttlShamDurationLabel->setEditable(true);
        ttlShamDurationLabel->addListener(this);
        addAndMakeVisible(ttlShamDurationLabel);
    yPos = yPos + yUpadate;

    ttlStimNumberText = new Label("ttlStimNumberText", "Stim Pulse:");
        ttlStimNumberText->setBounds(xPos, yPos, 140, 25);
        ttlStimNumberText->setColour(Label::textColourId, Colours::black);
        addAndMakeVisible(ttlStimNumberText);
    ttlStimNumberLabel = new Label("ttlStimNumberLabel", String(p->stimPulseNum));
        ttlStimNumberLabel->setBounds(xPos + XTextBox, yPos, WTextBox, TEXT_HT);
        ttlStimNumberLabel->setFont(Font("Default", 15, Font::plain));
        ttlStimNumberLabel->setColour(Label::textColourId, Colours::white);
        ttlStimNumberLabel->setColour(Label::backgroundColourId, Colours::grey);
        ttlStimNumberLabel->setEditable(true);
        ttlStimNumberLabel->addListener(this);
        addAndMakeVisible(ttlStimNumberLabel);

}

void IntermittentBurstGeneratorEditor::buttonClicked(Button* button)
{
    auto processor = static_cast<IntermittentBurstGenerator*>(getProcessor());
    if (button == fileButton)
    {
        String supportedFormats = "*.json";

        FileChooser chooseFileReaderFile("Please select a json file containing the markers mapping...",
            lastFilePath,
            supportedFormats);

        if (chooseFileReaderFile.browseForFileToOpen())
        {
            if (processor->setMapPath(chooseFileReaderFile.getResult().getFullPathName().toStdString()))
            {
                fileNameLabel->setText(chooseFileReaderFile.getResult().getFileName(), dontSendNotification);
            }
        }
    }
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
    else if (label == pulsewidthLabel)
    {
        String text = label->getText();
        float SamplingRate = processor->getSampleRate();
            if (float(text.getFloatValue()) <= (1.0f * 1000.0f/ SamplingRate)) // in milliseconds Making sure there are atleast two samples for the pulse
            {
                float MinimumValue = 1000.0f/ SamplingRate;
                pulsewidthLabel->setText(String(MinimumValue,3), dontSendNotification);
            }
            else 
            {
                processor->setParameter(IntermittentBurstGenerator::PULSEWIDTH, text.getIntValue());
            }
    }
}


void IntermittentBurstGeneratorEditor::saveCustomParameters(XmlElement* xml)
{
    auto processor = static_cast<IntermittentBurstGenerator*>(getProcessor());
    XmlElement* mainNode = xml->createNewChildElement("IntermittentBurstGenerator");
    mainNode->setAttribute("map_path", processor->channelMapPath);
}

void IntermittentBurstGeneratorEditor::loadCustomParameters(XmlElement* xml)
{
    forEachXmlChildElementWithTagName(*xml, xmlNode, "IntermittentBurstGenerator")
    {
        
        auto processor = static_cast<IntermittentBurstGenerator*>(getProcessor());

        if (xmlNode->hasAttribute("map_path"))
        {
            std::string path = xmlNode->getStringAttribute("map_path", "").toStdString();
            if (processor->setMapPath(path))
            {
                fileNameLabel->setText(path.substr(path.find_last_of("/\\") + 1), dontSendNotification);
            }
        }
    }
}
