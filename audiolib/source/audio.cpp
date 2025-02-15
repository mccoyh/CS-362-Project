#include "audio.h"

//checks if file exists
inline bool exists (const std::string& name) {
    struct stat buffer;   
    return (stat (name.c_str(), &buffer) == 0); 
}

//converts mp4 to mp3, pulls audio from video
void convert_mp4_mp3(std::string input_mp4){
    if(exists(input_mp4)){
        //checks for any existing output.mp3 files that already exist
        int num = 0;
        while(exists("output" + num + ".mp3")){
            num++
        }
        system("ffmpeg -i " + input_mp4 + " output" + num + ".mp3")
    }
}

void play_audio(std::string input_mp3){
    av_register_all();

    AVFormatContext *formatContext = avformat_alloc_context();
    if (avformat_open_input(&formatContext, input_mp3, NULL, NULL) != 0) {
        throw std::runtime_error("failed to open input file");
    }

    int audioStreamIndex = -1;
    for (int i = 0; i < formatContext->nb_streams; i++) {
        if (formatContext->streams[i]->codecpar->codec_type == AVMEDIA_TYPE_AUDIO) {
            audioStreamIndex = i;
            break;
        }
    }

    AVCodecContext *codecContext = avcodec_alloc_context3(NULL);
    avcodec_parameters_to_context(codecContext, formatContext->streams[audioStreamIndex]->codecpar);
    AVCodec *codec = avcodec_find_decoder(codecContext->codec_id);
    avcodec_open2(codecContext, codec, NULL);

    SDL_AudioSpec desiredSpec, obtainedSpec;
    desiredSpec.freq = codecContext->sample_rate;
    desiredSpec.format = AUDIO_S16SYS;
    desiredSpec.channels = codecContext->channels;
    desiredSpec.samples = 1024;
    SDL_AudioDeviceID audioDevice = SDL_OpenAudioDevice(NULL, 0, &desiredSpec, &obtainedSpec, 0);
    SDL_PauseAudioDevice(audioDevice, 0); // Start playback
}
