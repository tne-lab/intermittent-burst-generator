/*
  ==============================================================================

    IntermittentBurstGeneratorEditor.h
    Created: 7th December 2024
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

    enum Parameter
    {
        TTLPULSE,
        PULSEWIDTH,
        SHAMDURATION,
        STIMPULSENUM,
        STIMEVENTCHANNELIN,
        STIMEVENTCHANNELOUT,
        SHAMEVENTCHANNELIN,
        SHAMEVENTCHANNELOUT
    };

private:
    // Private methods
    //void sendState();
    std::string trim(const std::string str);
    int ttlpulse;
    float pulsewidth;
    float shamDuration;
    float stimPulseNum;
    int stimEventChannelIn; // Channel used to track stimulation events
    int stimEventChannelOut; // Channel used to track stimulation events
    int shamEventChannelIn; // Channel used to track sham events
    int shamEventChannelOut; // Channel used to track sham events

    // Private members
    int eventChannel;
    int eventDurationSamp;
    uint8_t pinState;
    int eventCount;
    bool stimNoStimCondition;
    int nextEventHappens;
    bool sampledInitialFlag;

};

#endif  // IntermittentBurstGenerator_H_INCLUDED
