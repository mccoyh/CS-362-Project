# MediaParser Public Interface

## Constructor

### `MediaParser(const std::string& mediaFile)`
- **mediaFile**: The path to the media file to be parsed.

Initializes a new `MediaParser` instance with the specified media file.

## Destructor

### `~MediaParser()`
Cleans up resources when the `MediaParser` instance is destroyed.

## Methods

### `AVFrameData getCurrentFrame() const`
- **Returns**: The current video and audio data of the current frame.

### `double getFrameRate() const`
- **Returns**: The frame rate of the media.

### `uint32_t getTotalFrames() const`
- **Returns**: The total number of frames in the media file.

### `uint32_t getCurrentFrameIndex() const`
- **Returns**: The current frame index.

### `void loadNextFrame()`
Loads the next frame in the media.

### `void loadPreviousFrame()`
Loads the previous frame in the media.

### `void loadFrameAt(uint32_t targetFrame)`
- **targetFrame**: The index of the frame to load.

Loads a specific frame in the media by its index.

### `void update()`
Updates the parser's internal features.

### `void play()`
Starts playback in automatic mode.

### `void pause()`
Pauses playback.

### `void setManual(bool manual)`
- **manual**: `true` for manual control, `false` for automatic playback.

Enables or disables manual control over frame loading.

### `MediaState getState() const`
- **Returns**: The current state of the media.

Gets the current state of the media parser.

## `AVFrameData`

Contains the data of a single frame:

- **`std::shared_ptr<std::vector<uint8_t>> videoData`**: A shared pointer to the video data for the frame.
- **`std::shared_ptr<std::vector<uint8_t>> audioData`**: A shared pointer to the audio data for the frame.
- **`int frameWidth`**: The width of the video frame.
- **`int frameHeight`**: The height of the video frame.

## `MediaState` Enum

Represents the state of the media parser:

- **`AUTO_PLAYING`**: The media is playing automatically.
- **`PAUSED`**: The media is paused.
- **`MANUAL`**: The media is under manual control.

## Example Usage

```cpp
#include "MediaParser.h"

int main() {
    // Create a MediaParser object for a given media file.
    AVParser::MediaParser mediaParser("path_to_media_file");

    // Play the media automatically.
    mediaParser.play();

    // Get frame rate and total frames.
    double frameRate = mediaParser.getFrameRate();
    uint32_t totalFrames = mediaParser.getTotalFrames();

    // Load and display the first frame.
    mediaParser.loadFrameAt(0);
    AVParser::AVFrameData frameData = mediaParser.getCurrentFrame();
    
    // Display some basic frame info.
    std::cout << "Frame width: " << frameData.frameWidth << std::endl;
    std::cout << "Frame height: " << frameData.frameHeight << std::endl;

    // Load next frame.
    mediaParser.loadNextFrame();

    // Pause playback.
    mediaParser.pause();

    // Set manual control.
    mediaParser.setManual(true);
    mediaParser.loadFrameAt(100);  // Load the 100th frame manually.

    return 0;
}
```