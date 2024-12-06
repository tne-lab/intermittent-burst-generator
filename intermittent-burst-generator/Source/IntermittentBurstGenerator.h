/*
  ==============================================================================

    IntermittentBurstGeneratorEditor.h
    Created: 3rd December 2024
    Author:  Sumedh Sopan Nagrale

  ==============================================================================
*/

#ifndef IntermittentBurstGenerator_H_INCLUDED
#define IntermittentBurstGenerator_H_INCLUDED

#include <ProcessorHeaders.h>  // Include necessary processor headers

class IntermittentBurstGenerator : public GenericProcessor
{
    friend class IntermittentBurstGeneratorEditor;  // Allow IntermittentBurstGeneratorEditor to access private members

public:
    // Constructor
    IntermittentBurstGenerator();

    // Overridden methods from GenericProcessor
    void createEventChannels() override;
    AudioProcessorEditor* createEditor() override;
    void process(AudioSampleBuffer& continuousBuffer) override;
    void handleEvent(const EventChannel* channelInfo, const MidiMessage& event, int samplePosition = 0) override;
    void setParameter(int parameterIndex, float newValue) override;

    //public method
    void triggerEvent(juce::int64 bufferstartTs, int EventStratTime, int eventChannelNum, 
        int eventDurationSamplesIn, float samplingRate);


    // Public members
    EventChannel* eventChannelPtr;
    MetaDataDescriptorArray eventMetaDataDescriptors;
    TTLEventPtr turnoffEvent;  // Holds a turnoff event to be added later

    bool setMapPath(std::string filePath);
    std::string channelMapPath;

    enum Parameter
    {
        TTLPULSE,
        PULSEWIDTH,
        SHAMDURATION,
        STIMPULSENUM
    };

private:
    // Private methods
    //void sendState();
    std::string trim(const std::string str);
    int ttlpulse;
    float pulsewidth;
    float shamDuration;
    float stimPulseNum;

    // Private members
    int eventChannel;
    
    int eventDurationSamp;
    uint8_t pinState;
    int eventCount;
    bool stimNoStimCondition;
    int nextEventHappens;
    bool sampledInitialFlag;
    // Maps for channel and pin mappings
    std::map<int, int> channelDelayMap;  // Maps incoming TTL channels to delay numbers (1-8)
    std::unordered_map<int, uint64_t> pinTurnOffSamples;  // Maps pin numbers to timestamps for turn-off

};

#endif  // IntermittentBurstGenerator_H_INCLUDED
