#include <iostream>
#include <string>
#include <vector>
#include <numbers>
#include <limits>
#include <cmath>
#include <cstdint>

#include <AL/al.h>
#include <AL/alc.h>

class MorseCodePlayer {
public:
	void PlayMorseCode(const std::string& morseCode) {
		auto audioDevice = alcOpenDevice(nullptr);
		if (!audioDevice) {
			std::cerr << "Failed to open OpenAL audio device" << std::endl;
			return;
		}

		auto audioContext = alcCreateContext(audioDevice, nullptr);
		if (!audioContext) {
			std::cerr << "Failed to create OpenAL audio context" << std::endl;
			alcCloseDevice(audioDevice);
			return;
		}

		if (!alcMakeContextCurrent(audioContext)) {
			std::cerr << "Failed to make OpenAL audio context current" << std::endl;
			alcDestroyContext(audioContext);
			alcCloseDevice(audioDevice);
			return;
		}

		for (unsigned char character : morseCode) {
			switch (character) {
			case '/':
				PlayTone(0.0f, 7 * dotDuration);
				break;
			case ' ':
				PlayTone(0.0f, 3 * dotDuration);
				break;
			case '.':
				PlayTone(toneFrequency, 1 * dotDuration);
				break;
			case '-':
				PlayTone(toneFrequency, 3 * dotDuration);
				break;
			default:
				break;
			}
		}

		alcMakeContextCurrent(nullptr);
		alcDestroyContext(audioContext);
		alcCloseDevice(audioDevice);
	}

private:
	void PlayTone(float frequency, std::int32_t duration) {
		alGenBuffers(1, &bufferId);

		const ALsizei sampleRate = 44'100;
		const ALsizei sampleCount = sampleRate * duration / 1'000;
		std::vector<ALshort> samples(sampleCount);
		for (ALsizei i = 0; i < sampleCount; ++i) {
			samples[i] = static_cast<ALshort>(std::numeric_limits<ALshort>::max() * std::sin(2.0f * std::numbers::pi * frequency * i / sampleRate));
		}

		alBufferData(bufferId, AL_FORMAT_MONO16, samples.data(), sampleCount * sizeof(ALshort), sampleRate);

		alGenSources(1, &sourceId);
		alSourcei(sourceId, AL_BUFFER, bufferId);
		alSourcePlay(sourceId);

		ALint sourceState;
		do {
			alGetSourcei(sourceId, AL_SOURCE_STATE, &sourceState);
		} while (sourceState == AL_PLAYING);

		alDeleteSources(1, &sourceId);
		alDeleteBuffers(1, &bufferId);
	}

	ALuint bufferId = 0u;
	ALuint sourceId = 0u;

	const std::uint32_t dotDuration = 100u;  // in milliseconds
	const float toneFrequency = 500.0f;      // in Hertz
};

int main() {
	MorseCodePlayer morseCodePlayer;
	morseCodePlayer.PlayMorseCode("... --- ...");
	return 0;
}
