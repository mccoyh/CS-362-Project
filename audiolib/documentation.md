# audiolib

## `AudioData` Structure

### Members

- `SDL_AudioStream* stream;`
    - **Description**: A pointer to an SDL_AudioStream used for playing audio.

- `SDL_AudioSpec spec;`
    - **Description**: The specifications of the audio stream.

- `unit32_t duration;`
    - **Description**: The duration of the audio in milliseconds, generally with an added 150 ms buffer.

## Methods

### `bool exists(const std::string& name);`
- **name**: A string containing the name of a file.
- **Returns**: `true` if file exists; otherwise, `false`.

Checks if a file exists or not.

### `void convertWav(const std::string& input, const std::string& output);`
- **input**: A string containing the name of an input file.
- **output**: A string containing the name of a generated output file without the file extension suffix.

Creates a WAV file from an input.

### `AudioData playAudio(const char* input_wav);`
- **input_wav**: A string containing the name of an input file. Must be .wav format.
- **Returns**: An AudioData struct containing the current audio stream.

Creates and starts playing an audio stream.

### `void initSDL();`

Initializes SDL for usage, enables SDL audio and event modules.

### `void delay(uint32_t ms);`
- **ms**: A `uint32_t` containing a specified duration in milliseconds.

Creates a short time delay in the program.

### `void deleteStream(SDL_AudioStream *stream);`
- **stream**: A pointer to an SDL_AudioStream to be deleted.

Deletes an audio stream.

### `void quitSDL();`

Closes SDL audio and event modules and quits SDL.

### `void pauseAudio(SDL_AudioStream* stream);`
- **stream**: A pointer to an SDL_AudioStream to be paused.

Pauses playback of an audio stream.

### `void resumeAudio(SDL_AudioStream* stream);`
- **stream**: A pointer to an SDL_AudioStream to be resumed.

Resumes playback of an audio stream.

### `bool isPaused(SDL_AudioStream* stream);`
- **stream**: A pointer to an SDL_AudioStream.
- **Returns**: `true` if stream is paused; otherwise, `false`.

Checks if an audio stream is paused.

### `void changeSpeed(SDL_AudioStream* stream, float speed);`
- **stream**: A pointer to an SDL_AudioStream.
- **speed**: A float value containing the speed to play audio at. Must be between 0.01 and 100.

Changes the playback speed and pitch of an audio stream.

### `void changeVolume(SDL_AudioStream* stream, float volume);`
- **stream**: A pointer to an SDL_AudioStream.
- **volume**: A float value containing the volume to play audio at. 1 = no change, 0 = silence, 1< = louder output, 1> = quieter output.

Changes the volume of an audio stream.

### `void mute(SDL_AudioStream* stream);`
- **stream**: A pointer to an SDL_AudioStream.

Mutes an audio stream (set volume to 0).

## Example Usage

```cpp
Audio::initSDL();

Audio::convertWav("video.mp4", "audio");
Audio::AudioData audio = Audio::playAudio("audio.wav");
uint32_t duration = audio.duration;

while(duration > 0){
    Audio::delay(10);

    /*
        Do any checks, pause or resume audio, etc., here.
    */

    duration -= 10;
}

Audio::deleteStream(audio.stream);
Audio::quitSDL();
```

Further information regarding SDL can be found here:
[SDL3 documentation](https://wiki.libsdl.org/SDL3/FrontPage)