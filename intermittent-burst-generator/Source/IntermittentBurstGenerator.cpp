/*
  ==============================================================================

	IntermittentBurstGeneratorEditor.h
	Created: 7th December 2024
	Modified: 22nd May 2025 for Bug-- Pause & play caused the time to reset and counting for sham to restart
	Author:  Sumedh Sopan Nagrale

  ==============================================================================
*/

#include "IntermittentBurstGenerator.h"
#include "IntermittentBurstGeneratorEditor.h"

#ifdef __cplusplus
extern "C" {
#endif
}

// Constructor
IntermittentBurstGenerator::IntermittentBurstGenerator()
	: GenericProcessor("Intermittent Burst Generator"),
	eventChannel(0),                   // Initialize eventChannel to 0
	stimEventChannelIn(1),			   // Initialize stimulation event channel to 1
	stimEventChannelOut(1),			   // Initialize stimulation event channel to 1
	shamEventChannelIn(1),			   // Initialize sham event channel to 1
	shamEventChannelOut(1),			   // Initialize sham event channel to 1
	eventDurationSamp(0),              // Initialize eventDurationSamp to 0
	pinState(0),                       // Initialize pinState to 0
	eventCount(0),
	eventChannelPtr(nullptr),
	pulsewidth(5),
	ttlpulse(1),
	stimNoStimCondition(false),
	nextEventHappens(0),
	sampledInitialFlag(false),
	shamDuration(7),
	stimPulseNum(10)
{
	// Set the processor type
	setProcessorType(PROCESSOR_TYPE_SINK);

	// Initialize event-related metadata descriptors
	auto eventDescriptor = std::make_unique<MetaDataDescriptor>(
		MetaDataDescriptor::INT64,
		1,
		"Intermittent Burst Generator",
		"Time when Event was sent",
		"Event time"
	);
	eventMetaDataDescriptors.add(eventDescriptor.release());
}

// Parameter setter
void IntermittentBurstGenerator::setParameter(int parameterIndex, float newValue)
{
	switch (parameterIndex)
	{
	case TTLPULSE:
		ttlpulse = static_cast<int>(newValue);
		break;
	case PULSEWIDTH:
		pulsewidth = static_cast<float>(newValue);
		break;
	case SHAMDURATION:
		shamDuration = static_cast<float>(newValue);
		break;
	case STIMEVENTCHANNELIN:
		stimEventChannelIn = static_cast<int>(newValue);
		break;
	case STIMEVENTCHANNELOUT:
		stimEventChannelOut = static_cast<int>(newValue);
		break;
	case SHAMEVENTCHANNELIN:
		shamEventChannelIn = static_cast<int>(newValue);
		break;
	case SHAMEVENTCHANNELOUT:
		shamEventChannelOut = static_cast<int>(newValue);
		break;
	case STIMPULSENUM:
		stimPulseNum = static_cast<int>(newValue);
		break;
	}
}

// Editor creation
AudioProcessorEditor* IntermittentBurstGenerator::createEditor()
{
	editor = new IntermittentBurstGeneratorEditor(this, true);
	return editor;
}

// Pin state handling during audio processing
void IntermittentBurstGenerator::process(AudioSampleBuffer& continuousBuffer)
{
	totalSamplesProcessed += getNumSamples(0);
	checkForEvents(false);
}

// Event handling
void IntermittentBurstGenerator::handleEvent(const EventChannel* channelInfo, const MidiMessage& event, int samplePosition)
{
	float sampleRate = getTotalDataChannels() > 0 ? getDataChannel(0)->getSampleRate() : 30000;
	static bool prevRecStatus = false; //to reset event count once when recording starts
	bool recordingStatus = CoreServices::getRecordingStatus();
	if (sampledInitialFlag == false) {
		nextEventHappens = sampleRate * shamDuration;
		sampledInitialFlag = true;
	}

	if (Event::getEventType(event) == EventChannel::TTL)
	{
		
		TTLEventPtr ttl = TTLEvent::deserializeFromMessage(event, channelInfo);
		juce::int64 currentTimeInSamples = totalSamplesProcessed;

		int nSamples = getNumSamples(0);
		// Making sure the events are pertaining to stimulation and sham and nothing else
		if (ttl->getState() && (ttl->getChannel() == stimEventChannelIn-1 || ttl->getChannel() == shamEventChannelIn-1))
		{
			// converting back to seconds and multiplying by sampling rate give samples
			int eventDurationSamplesIn = int(std::floor(sampleRate * (float(pulsewidth) / 1000.0f)));

            // resetting event count when recording starts
			if (recordingStatus == true && !prevRecStatus)
			{
				if (stimNoStimCondition == true) {
					std::cout << "---------------- Recording started: Stimulation reset ---------------" << std::endl;
				}
				if (stimNoStimCondition == false) {
					std::cout << "---------------- Recording started ------------------" << std::endl;
				}
				eventCount = 0;
				prevRecStatus = recordingStatus;
			} 
			if (recordingStatus == false && prevRecStatus == true)
			{
				//reset when recording is stopped
				prevRecStatus = false;
			}			
			if (stimNoStimCondition == true && ttl->getChannel() == stimEventChannelIn - 1)
			{

				// if the number of pulse that needs to be send is more than one do this other wise
				// for now this will be just one
				for (int pulseIndex = 0; pulseIndex < ttlpulse; ++pulseIndex) {
					// This is where we could create copies of the even by delaying it by certain amount.
					// The original event is 5 ms long in active duration
					juce::int64 adjustedTimestamp = event.getTimeStamp();// +(eventDurationSamplesIn) * (pulseIndex + 1);
					triggerEvent(getTimestamp(0), adjustedTimestamp, stimEventChannelOut, eventDurationSamplesIn, sampleRate); // for zero indexing within the function
					eventCount = eventCount + 1;
					logEventInfo("STIMULATION", stimEventChannelIn, stimEventChannelOut, sampleRate);
				}
				int TotalPulses = stimPulseNum; 
				if (eventCount >= TotalPulses)
				{
					std::cout << std::endl << "----------------" << "Sham Starts" << "----------------";
					stimNoStimCondition = false;
					eventCount = 0;
					nextEventHappens = currentTimeInSamples + (shamDuration * sampleRate);
				}
			}
			else
			{
				if (stimNoStimCondition == false)
				{
					juce::int64 eventOriginalTime = event.getTimeStamp();// This can be used to create delayed version of events +(eventDurationSamplesIn) * (pulseIndex + 1);
					triggerEvent(getTimestamp(0), eventOriginalTime, shamEventChannelOut, eventDurationSamplesIn, sampleRate); // for zero indexing within the function
					if (ttl->getChannel() == shamEventChannelIn - 1) {
						logEventInfo("SHAM", shamEventChannelIn, shamEventChannelOut, sampleRate);
					}
					else {
						std::cout << "##### Not triggered: Stim Event (Out: " << shamEventChannelOut
							<< " , In from: " << shamEventChannelIn << ",Current In:" << ttl->getChannel() + 1 << "), Event Trigger count: " << eventCount
							<< " Current time: " << currentTimeInSamples
							<< " Next time: " << nextEventHappens << std::endl;
					}
					// if time is greater than X seconds stim
					if (currentTimeInSamples >= nextEventHappens) {
						std::cout << std::endl << "----------------" << "Stimulation Starts" << "----------------";
						std::cout << "Reset @ " << currentTimeInSamples << " | Next Trigger @: " << nextEventHappens << std::endl;
						stimNoStimCondition = true;
						eventCount = 0;
					}
				}

			}
		}
	}
	
}

// Trigger Event required to send the events
void IntermittentBurstGenerator::triggerEvent(juce::int64 bufferstartTs, int EventStratTime, int eventChannelNum, 
	int eventDurationSamplesIn, float samplingRate)
{
	// Create metadata array
	// Requires associated metadata, needs to updated when you are sending this.
	MetaDataValueArray metadataArray;
		{
			int metadataIndex = 0;
			MetaDataValue* currentEventTime = new MetaDataValue(*eventMetaDataDescriptors[metadataIndex++]);
			currentEventTime->setValue(bufferstartTs + EventStratTime);
			metadataArray.add(currentEventTime);
		}

	// Setup event channel
		eventChannel = eventChannelNum;
		int currentEventChannel = eventChannel - 1; // Adjust for 0-based indexing

	// TTL starts
		// EventTime should be larger if we have not missed to show it
		int sampleNumberOn = std::max(EventStratTime, 0);
		juce::int64 eventTimestamp = bufferstartTs + sampleNumberOn;

	// Time for pulse width of TTL pulse
		int eventDurationSamples = eventDurationSamplesIn;

	// TTL ends
		int sampleNumberOff = sampleNumberOn + eventDurationSamples;
		juce::int64 eventTimestampOff = bufferstartTs + sampleNumberOff;

	// Create and add "On" event
		juce::uint8 ttlDataOn = 1 << currentEventChannel; // Turn on the event channel
		TTLEventPtr eventOn = TTLEvent::createTTLEvent(eventChannelPtr, eventTimestamp, &ttlDataOn, sizeof(juce::uint8), metadataArray, currentEventChannel);
		// this event is triggered at the same time as the original event.
		addEvent(eventChannelPtr, eventOn, sampleNumberOn);

	// Calculate duration and create "Off" event
		juce::uint8 ttlDataOff = 0; // Turn off all the event channel
		TTLEventPtr eventOff = TTLEvent::createTTLEvent(eventChannelPtr, eventTimestampOff, &ttlDataOff, sizeof(juce::uint8), metadataArray, currentEventChannel);
		addEvent(eventChannelPtr, eventOff, sampleNumberOff);

	// To see if things are working right
		std::cout << std::endl<< "[Trigged @ - CH" << eventChannel <<" ] for ";

	
}

// Create Event Channels is required for the creation of the channel
void IntermittentBurstGenerator::createEventChannels()
{
	// Retrieve the input data channel
	const DataChannel* inputChannel = getDataChannel(0);
	if (!inputChannel)
	{
		eventChannelPtr = nullptr;
		return;
	}

	// Create and configure the event channel
	float sampleRate = inputChannel->getSampleRate();
	EventChannel* eventChannel = new EventChannel(EventChannel::TTL, 8, 1, sampleRate, this);
	eventChannel->setName("IBG Event Output");
	eventChannel->setDescription("Triggers when an event is received for two TTL pulses.");
	eventChannel->setIdentifier("Event.IBG");

	// Add metadata for the source channel
	// Identify that the plugin has been responsible for creation of this events.
	MetaDataDescriptor sourceChanDesc(
		MetaDataDescriptor::UINT16, 3, "Source Channel",
		"Index at its source, Source processor ID, and Sub Processor index of the channel that triggers this event",
		"source.channel.identifier.full");

	uint16 sourceInfo[3] = {
		inputChannel->getSourceIndex(),
		inputChannel->getSourceNodeID(),
		inputChannel->getSubProcessorIdx() };

	MetaDataValue sourceChanVal(sourceChanDesc);
	sourceChanVal.setValue(static_cast<const uint16*>(sourceInfo));
	eventChannel->addMetaData(sourceChanDesc, sourceChanVal);

	// Add event-related metadata descriptors
	for (const auto& descriptor : eventMetaDataDescriptors)
	{
		eventChannel->addEventMetaData(descriptor);
	}

	// Add the created event channel to the array
	eventChannelPtr = eventChannelArray.add(eventChannel);
}

void IntermittentBurstGenerator::logEventInfo(const std::string& type, int inChannel, int outChannel, float sampleRate)
{
	float currentTimeSec = static_cast<float>(totalSamplesProcessed) / sampleRate;
	float nextTimeSec = static_cast<float>(nextEventHappens) / sampleRate;

	std::cout << type << " Event | In: " << inChannel
		<< ", Out: " << outChannel
		<< ", Count: " << eventCount
		<< ", Time: " << juce::String(currentTimeSec, 0).toStdString() << "s"
		<< ", Next Stim after: " << juce::String(nextTimeSec, 0).toStdString();

}

