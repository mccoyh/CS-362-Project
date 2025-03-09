# Testing

A suite of tests has been established to ensure proper performance of all libraries that power Medos.
Since there is currently no simple and accurate way to automate AV playback and AI feature testing, all tests must be run manually.

## Running Tests
Each test corresponds to a specific library and its functionality. If a test requires an external media file, provide the file path as an argument (`PATH_TO_MEDIA`). If the test does not require external media, you can omit this argument or use built-in asset names.

All test executables are located in the `bin` directory after building the project.

## Notes
- Ensure all required dependencies are installed before running tests. Refer to [SETUP.md](SETUP.md) for setup instructions.
- For troubleshooting or further details, refer to the respective library documentation.

## Test Overview
| **Library**     | **Test Name**     | **Executable**   | **Description**                                                                 | **Run Command**                                      |
|-------------------|-------------------|------------------|---------------------------------------------------------------------------------|-----------------------------------------------------|
| **AudioToTxt**    | test_whisper      | `test_whisper.exe` | Generates a text file transcription of an audio or video file.                  | `./test_whisper.exe PATH_TO_MEDIA`                  |
| **audiolib**      | audioplayback      | `audioplayback.exe` | Plays a .wav format audio file for 10 seconds, then speeds it up to 2x for 10 seconds. | `./audioplayback.exe PATH_TO_MEDIA`                  |
|                   | convertwav         | `convertwav.exe`  | Converts any video or audio file to .wav format.                                 | `./convertwav.exe PATH_TO_MEDIA`                    |
| **avParser**      | avExtraction       | `avExtraction.exe` | Creates a window for playing video with a UI.                                    | `./avExtraction.exe PATH_TO_MEDIA`                  |
|                   | userinterface      | `ui_shortcuts.exe` | Displays UI interface for media player. Provides playback controls through keyboard shortcuts.    | `./ui_shortcuts.exe PATH_TO_MEDIA` |
| **vulkanEngine**  | guiWidget          | `guiWidget.exe`   | Creates a window with a test widget.                                             | `./guiWidget.exe`                                   |
|                   | sfx                | `sfx.exe`         | Plays a video file with added effects.                                           | `./sfx.exe PATH_TO_MEDIA`                           |
|                   | videoDecode        | `videoDecode.exe` | Decodes and plays a video file.                                                 | `./videoDecode.exe PATH_TO_MEDIA`                   |
|                   | window             | `window.exe`      | Creates an empty window.                                                        | `./window.exe`           |