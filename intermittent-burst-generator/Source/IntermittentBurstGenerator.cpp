/*
  ==============================================================================

	IntermittentBurstGeneratorEditor.h
	Created: 3rd December 2024
	Author:  Sumedh Sopan Nagrale

  ==============================================================================
*/

#include "IntermittentBurstGenerator.h"
#include "IntermittentBurstGeneratorEditor.h"

#ifdef __cplusplus
extern "C" {
#endif
}
#include <fstream>
#include <sstream>
#include <bitset>

// Constructor
IntermittentBurstGenerator::IntermittentBurstGenerator()
	: GenericProcessor("Intermittent Burst Generator"),
	eventChannel(0),                   // Initialize eventChannel to 0
	eventDurationSamp(0),              // Initialize eventDurationSamp to 0
	pinState(0),                       // Initialize pinState to 0
	channelDelayMap(),                   // Default initialize the map
	pinTurnOffSamples(),
	eventCount(0),
	eventChannelPtr(nullptr),
	pulsewidth(0.1),
	ttlpulse(1),
	stimNoStimCondition(false),
	nextEventHappens(0),
	sampledInitialFlag(false),
	shamDuration(7),
	stimPulseNum(7)
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
	case STIMPULSENUM:
		stimPulseNum = static_cast<int>(newValue);
		break;
	}
}

// Binary string conversion
std::string toBinaryString(uint8_t byte) {
	return std::bitset<8>(byte).to_string();
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
	checkForEvents(false);
}

// Event handling
void IntermittentBurstGenerator::handleEvent(const EventChannel* channelInfo, const MidiMessage& event, int samplePosition)
{
	float sampleRate = getTotalDataChannels() > 0 ? getDataChannel(0)->getSampleRate() : 30000;
	if (sampledInitialFlag == false) {
		nextEventHappens = sampleRate * shamDuration;
		sampledInitialFlag = true;
	}

	if (Event::getEventType(event) == EventChannel::TTL)
	{
		
		TTLEventPtr ttl = TTLEvent::deserializeFromMessage(event, channelInfo);
		auto it = channelDelayMap.find(ttl->getChannel());
		
		//std::cout << "Event at: " << it->first << it->second << std::endl;
		juce::int64 startTs = getTimestamp(0);
		int nSamples = getNumSamples(0);

		// To see if things are working right


		if (it != channelDelayMap.end() && ttl->getState())
		{
			int delayAtEventChannel = it->second;
			
			if (stimNoStimCondition == true)
			{


				// converting back to seconds and multiplying by sampling rate give samples
				int eventDurationSamplesIn = int(std::floor(sampleRate * (float(pulsewidth) / 1000.0f)));

				for (int pulseIndex = 0; pulseIndex < ttlpulse; ++pulseIndex) {
					juce::int64 adjustedTimestamp = event.getTimeStamp() + (eventDurationSamplesIn) * (pulseIndex + 1);
					triggerEvent(getTimestamp(0), adjustedTimestamp, delayAtEventChannel, eventDurationSamplesIn, sampleRate);
					eventCount = eventCount + 1;
					std::cout << "Event at" << eventChannel << ", Event Trigger count: " << eventCount << " Current time:" << startTs << " Next time:" << nextEventHappens << std::endl;
				}
				int TotalPulses = stimPulseNum; 
				if (eventCount >= TotalPulses)
				{
					stimNoStimCondition = false;
					eventCount = 0;
					nextEventHappens = startTs + (shamDuration * sampleRate);
				}

			}
			else
			{
				std::cout << "No trigger Event at" << eventChannel << ", Event Trigger count: " << eventCount << " Current time:" << startTs << " Next time:" << nextEventHappens << std::endl;
				// Do not stim
				// if time is greater than X seconds stim
				if (startTs >= nextEventHappens) {
					std::cout << "Reset at " << startTs << " Next time:" << nextEventHappens << std::endl;
					stimNoStimCondition = true;
					eventCount = 0;
				}

			}
		}
	}
	
}

// Channel-pin mapping
// Adapted from parallel port
bool IntermittentBurstGenerator::setMapPath(std::string filePath)
{
	if (filePath.empty())
	{
		return false;
	}
	channelDelayMap.clear();
	std::ifstream file(filePath);

	if (!file)
	{
		std::cout << "Cannot open channel-pin config file " << filePath << std::endl;
		CoreServices::sendStatusMessage("Cannot open channel-pin config file");
		return false;
	}

	channelMapPath = filePath;

	// Perform a naive JSON parsing
	try
	{
		std::stringstream ss;
		ss << file.rdbuf();
		file.close();

		// remove curly braces
		std::string str = ss.str();
		str.erase(std::remove(str.begin(), str.end(), '{'), str.end());
		str.erase(std::remove(str.begin(), str.end(), '}'), str.end());

		// split by comma
		std::vector<std::string> pairs;
		size_t pos = 0;
		std::string token;
		while ((pos = str.find(',')) != std::string::npos)
		{
			pairs.push_back(str.substr(0, pos));
			str.erase(0, pos + 1);
		}
		pairs.push_back(str);

		for (const auto& pair : pairs)
		{
			size_t colonPos = pair.find(':');
			if (colonPos != std::string::npos)
			{
				std::string channelStr = trim(pair.substr(0, colonPos));
				int channel = std::stoi(channelStr) - 1;
				int delay = std::stoi(trim(pair.substr(colonPos + 1)));

				if (channel < 0 || channel > 7)
				{
					std::cerr << "Channel number out of range in mapping: " << pair << std::endl;
					continue;
				}

				std::cout << "Event Delay setting map entry -- Channel:" << channel << ". Delay in the event:" << delay << std::endl;
				channelDelayMap[channel] = delay;
			}
		}
	}
	catch (const std::runtime_error& re)
	{
		std::cout << "Failed to read channel to pin mapping file with runtime error: " << re.what() << std::endl;
		return false;
	}
	catch (const std::exception& ex)
	{
		std::cout << "Failed to read channel to pin mapping file with exception: " << ex.what() << std::endl;
		return false;
	}

	return true;
}

// String trimming helper
inline std::string IntermittentBurstGenerator::trim(const std::string const_str)
{
	std::string str = const_str;
	str.erase(str.find_last_not_of(" \n\r\t\"") + 1);
	str.erase(0, str.find_first_not_of(" \n\r\t\""));
	return str;
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
			MetaDataValue* crossingPointValue = new MetaDataValue(*eventMetaDataDescriptors[metadataIndex++]);
			crossingPointValue->setValue(bufferstartTs + EventStratTime);
			metadataArray.add(crossingPointValue);
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
		std::cout << "At Event "<< eventChannel << " Sample Rate: " << samplingRate
			<< ", Event 'On' triggered at sample: " << sampleNumberOn
			<< ", Event 'Off' triggered at sample: " << sampleNumberOff
			<< ", Duration (milliseconds): " << 1000.0f * (float(sampleNumberOff) - float(sampleNumberOn)) / samplingRate
			<< std::endl;
	
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
	eventChannel->setName("Event Delay output");
	eventChannel->setDescription("Triggers when an event is received for two TTL pulses.");
	eventChannel->setIdentifier("Event.delay");

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