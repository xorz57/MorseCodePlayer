#if defined(_WIN32) || defined(__linux__)
#include <al.h>
#include <alc.h>
#else
#error "Unsupported Platform!"
#endif

#include <cmath>
#include <cstdint>
#include <cstdlib>
#include <iostream>
#include <limits>
#include <numbers>
#include <string>
#include <vector>

class MorseCodePlayer {
public:
    void Play(const std::string &morseCode) {
        auto audioDevice = alcOpenDevice(nullptr);
        if (!audioDevice) {
            std::cerr << "Failed to open OpenAL audio device!" << std::endl;
            std::exit(EXIT_FAILURE);
        }

        auto audioContext = alcCreateContext(audioDevice, nullptr);
        if (!audioContext) {
            std::cerr << "Failed to create OpenAL audio context!" << std::endl;
            alcCloseDevice(audioDevice);
            std::exit(EXIT_FAILURE);
        }

        if (!alcMakeContextCurrent(audioContext)) {
            std::cerr << "Failed to make OpenAL audio context current!" << std::endl;
            alcDestroyContext(audioContext);
            alcCloseDevice(audioDevice);
            std::exit(EXIT_FAILURE);
        }

        for (unsigned char character: morseCode) {
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
    void PlayTone(float frequency, std::uint32_t duration) {
        alGenBuffers(1, &mBufferID);

        const std::uint32_t sample_rate = 44'100;
        const std::uint32_t sample_count = sample_rate * duration / 1'000;
        std::vector<std::int16_t> samples(sample_count);
        for (std::uint32_t i = 0; i < sample_count; ++i) {
            samples[i] = static_cast<std::int16_t>(std::numeric_limits<std::int16_t>::max() * std::sin(2.0f * std::numbers::pi * frequency * i / sample_rate));
        }

        alBufferData(mBufferID, AL_FORMAT_MONO16, samples.data(), static_cast<std::int32_t>(sample_count * sizeof(std::int16_t)), sample_rate);

        alGenSources(1, &mSourceID);
        alSourcei(mSourceID, AL_BUFFER, static_cast<std::int32_t>(mBufferID));
        alSourcePlay(mSourceID);

        ALint sourceState;
        do {
            alGetSourcei(mSourceID, AL_SOURCE_STATE, &sourceState);
        } while (sourceState == AL_PLAYING);

        alDeleteSources(1, &mSourceID);
        alDeleteBuffers(1, &mBufferID);
    }

    std::uint32_t mBufferID = 0u;
    std::uint32_t mSourceID = 0u;

    const std::uint32_t dotDuration = 100u;
    const float toneFrequency = 500.0f;
};

int main() {
    MorseCodePlayer morseCodePlayer;
    morseCodePlayer.Play("... --- ...");
    return 0;
}
