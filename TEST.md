# Testing Instructions

All test executables are written to the `bin` directory after building.

## AudioToTxt

### test_whisper

Executable: `test_whisper.exe`
Generates a text file transcription of an audio or video file.

To run executable:
```bash
./test_whisper.exe PATH_TO_MEDIA
```

Note: `PATH_TO_MEDIA` can be omitted if the executable doesn’t require external media or you choose to use built-in asset names.

This is for testing whether audio transcriptions can be created.

## audiolib

### audioplayback

Executable: `audioplayback.exe`
Plays a .wav format audio file for 10 seconds, then it speeds up the audio to 2x speed for another 10 seconds before stopping.

To run executable:
```bash
./audioplayback.exe PATH_TO_MEDIA
```

Note: `PATH_TO_MEDIA` can be omitted if the executable doesn’t require external media or you choose to use built-in asset names.

This is used for testing audio playing functionality, as well as variable playback speed functionality for audio.

### convertwav

Executable: `convertwav.exe`
Converts any video or audio file* to .wav format.

To run executable:
```bash
./convertwav.exe PATH_TO_MEDIA
```

Note: `PATH_TO_MEDIA` can be omitted if the executable doesn’t require external media or you choose to use built-in asset names.

*Any file type supported by ffmpeg.

This is used for testing audio conversion capability needed for the audio library.

## avParser

### avExtraction

Executable: `avExtraction.exe`
Creates a window for playing video with UI.

To run executable:
```bash
./avExtraction.exe PATH_TO_MEDIA
```

Note: `PATH_TO_MEDIA` can be omitted if the executable doesn’t require external media or you choose to use built-in asset names.

## vulkanEngine

### guiWidget

Executable: `guiWidget.exe`
Creates a window with a test widget.

To run executable:
```bash
./guiWidget.exe
```
This is to test whether an ImGui widget can be created in a window.

### sfx

Executable: `sfx.exe`
Plays a video file with added effects.

To run executable:
```bash
./sfx.exe PATH_TO_MEDIA
```

Note: `PATH_TO_MEDIA` can be omitted if the executable doesn’t require external media or you choose to use built-in asset names.

This is to test the special effects functionality for video.

### videoDecode

Executable: `videoDecode.exe`
Decodes and plays a video file.

To run executable:
```bash
./videoDecode.exe PATH_TO_MEDIA
```

Note: `PATH_TO_MEDIA` can be omitted if the executable doesn’t require external media or you choose to use built-in asset names.

This is to test whether a video file can be played.

### window

Executable: `window.exe`
Creates an empty window.

To run executable:
```bash
./window.exe
```

This is for testing whether a window can be created.