
## About
This c++ library uses FFmpeg, Whisper ASR cpp, and Vulkan SDK to create subtitles for videos. The library works with any whisper model and provides a human readable subtitle file with timestamps in frames. 

## Notes
When running whisper for the first time with vulkan, the vulkan shaders are compiled extending the runtime for the first use. 

## Functions
### transcribeAudio
#### Parameters
- model_file : const std::string  
    The location of a whisper model. 
- audio_file : const std::string  
    The location of a formatted audio file (see whisper documentation for format).
- output_srt : const std::string  
    The location to save the generated subtitles. 
#### Returns
- Generated subtitles are saved to the output_srt file location. 
- int : Returns 1 if success full and -1 otherwise. 


### getSubtitleForFrame
This function searches the subtitle file for subtitles at a specific frame. The file must be formatted as generated in the transcribeAudio function. 
#### Parameters
 - frame : int  
    The frame to find subtitles at.
 - subtitle_file : const std::string 
    The location of the subtitle text file.

#### Returns
 - str : The subtitle text
