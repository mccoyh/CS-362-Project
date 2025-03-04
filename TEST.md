# Testing Instructions

All test executables are written to the `bin` directory after building.

Note: `PATH_TO_MEDIA` can be omitted if the executable doesn’t require external media or you choose to use built-in asset names.

| **Library**     | **Test Name**     | **Executable**   | **Description**                                                                 | **Run Command**                                      |
|-------------------|-------------------|------------------|---------------------------------------------------------------------------------|-----------------------------------------------------|
| **AudioToTxt**    | test_whisper      | `test_whisper.exe` | Generates a text file transcription of an audio or video file.                  | `./test_whisper.exe PATH_TO_MEDIA`                  |
| **audiolib**      | audioplayback      | `audioplayback.exe` | Plays a .wav format audio file for 10 seconds, then speeds it up to 2x for 10 seconds. | `./audioplayback.exe PATH_TO_MEDIA`                  |
|                   | convertwav         | `convertwav.exe`  | Converts any video or audio file to .wav format.                                 | `./convertwav.exe PATH_TO_MEDIA`                    |
| **avParser**      | avExtraction       | `avExtraction.exe` | Creates a window for playing video with a UI.                                    | `./avExtraction.exe PATH_TO_MEDIA`                  |
| **vulkanEngine**  | guiWidget          | `guiWidget.exe`   | Creates a window with a test widget.                                             | `./guiWidget.exe`                                   |
|                   | sfx                | `sfx.exe`         | Plays a video file with added effects.                                           | `./sfx.exe PATH_TO_MEDIA`                           |
|                   | videoDecode        | `videoDecode.exe` | Decodes and plays a video file.                                                 | `./videoDecode.exe PATH_TO_MEDIA`                   |
|                   | window             | `window.exe`      | Creates an empty window.                                                        | `./window.exe`           |
