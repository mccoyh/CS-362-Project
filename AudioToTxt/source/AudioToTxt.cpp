#include "AudioToTxt.h"
#include <iostream>
#include <fstream>
#include <vector>
#include <sstream>
#include <cstdlib>

#include "whisper.h"

namespace Captions{
    constexpr float AUDIO_NORM = 32768.0f; //max size of a 16 bit signed int to normalize audio between -1 and 1

    int transcribeAudio(const std::string& model_path, const std::string& audio_file, const std::string& output_srt){
        std::cout << "in function  " << std::endl;
        whisper_full_params params = whisper_full_default_params(WHISPER_SAMPLING_GREEDY);
        whisper_context* ctx = whisper_init_from_file(model_path.c_str());
        if (!ctx){
            std::cerr << "Failed to initialize Whisper model from file: " << model_path << std::endl;
            return -1;
        }
        std::vector<float> pcm_data; // Store extracted audio samples

        // Load the extracted audio file
        std::ifstream input(audio_file, std::ios::binary);
        if (!input.is_open()){
            std::cerr << "Failed to open audio file: " << audio_file << std::endl;
            whisper_free(ctx);
            return -1;
        }
        input.seekg(0, std::ios::end);
        size_t size = input.tellg();
        input.seekg(0, std::ios::beg);

        // Resize the vector based on the size of the input
        pcm_data.resize(size / sizeof(short));  // sizeof(short) because it's 16-bit PCM

        // Read PCM data
        std::vector<short> pcm_short_data(pcm_data.size());
        input.read(reinterpret_cast<char*>(pcm_short_data.data()), size);
        input.close();

        // Convert PCM data (16-bit signed) to floating point
        for (size_t i = 0; i < pcm_short_data.size(); ++i){
            pcm_data[i] = pcm_short_data[i] / AUDIO_NORM; // Convert to float in the range of -1.0 to 1.0
        }

        // Transcribe audio
        int result = whisper_full(ctx, params, pcm_data.data(), pcm_data.size());
        if (result != 0){
            std::cerr << "Whisper transcription failed." << std::endl;
            whisper_free(ctx);
            return -1;
        }
        
        // Save subtitles
        std::ofstream srt_out(output_srt);
        if (!srt_out.is_open()){
            std::cerr << "Failed to open subtitle file: " << output_srt << std::endl;
            whisper_free(ctx);
            return -1;
        }
        std::cout << "reaches write  " << std::endl;
        for (int i = 0; i < whisper_full_n_segments(ctx); i++){
            std::cout << "segment: " << i << std::endl;
            srt_out << i + 1 << "\n";
            srt_out << whisper_full_get_segment_t0(ctx, i) << " --> "
                    << whisper_full_get_segment_t1(ctx, i) << "\n";
            srt_out << whisper_full_get_segment_text(ctx, i) << "\n\n";
        }

        srt_out.close();
        whisper_free(ctx);
        return 0;
    }
    
    // Find the subtitle for a given frame by parsing the file directly
    std::string getSubtitleForFrame(int frame_number, const std::string& filename){
        std::ifstream file(filename);
        if (!file.is_open()){
            std::cerr << "Failed to open file: " << filename << std::endl;
            return "[Error: File not found]";
        }
    
        std::string line;
        int start_frame, end_frame;
        std::string text;
    
        while (std::getline(file, line)){
            std::istringstream iss(line);
            if (iss >> start_frame){  // Read subtitle index (ignored)
                if (std::getline(file, line)){  // Read frame range line
                    std::istringstream time_stream(line);
                    std::string arrow; // To capture '-->'
                    if (time_stream >> start_frame >> arrow >> end_frame){
                        text.clear();
                        while (std::getline(file, line) && !line.empty()){
                            text += (text.empty() ? "" : " ") + line;
                        }
                        // Check if the frame number is within this range
                        if (frame_number >= start_frame && frame_number <= end_frame){
                            return text;
                        }
                    }
                }
            }
        }
    
        return "";
    }
    CaptionCache::CaptionCache(const std::string& caption_srt){
        loadCaptions(caption_srt);
    }


    void CaptionCache::loadCaptions(const std::string& caption_srt){
        std::ifstream file(caption_srt);
        if (!file){
            std::cerr << "Error: Could not open file: " << caption_srt << std::endl;
            return;
        }
        std::string line;
        int index;
        int startFrame, endFrame;
        std::string text, frameRange;

        while(std::getline(file, line)){
            if (line.empty()){
                continue;
            }
            //Read subtitle number
            std::istringstream(line) >> index;
            //Read Subtitle frame range
            if(!std::getline(file, frameRange)){
                break;
            }
            std::istringstream frame_stream(frameRange);
            std::string arrow;
            if(frame_stream >> startFrame >> arrow >> endFrame){
                text.clear();
                while (std::getline(file, line) && !line.empty()){
                    text += (text.empty() ? "" : " ") + line;
                }
            }
            captionMap[startFrame] = Subtitle(startFrame, endFrame, text);
        }
        numCaptions = index;
    }

    std::string CaptionCache::getCaptionAtFrame(int frame){
        // Binary search: find the subtitle with the largest start Frame <= frame
        auto it = captionMap.lower_bound(frame);

        // Check if there is a valid caption and if the frame is inside the caption's range
        if (it != captionMap.begin()){
            --it;  // Go to the previous subtitle (largest startFrame <= frame)

            if (frame >= it->second.startFrame && frame <= it->second.endFrame){
                return it->second.text;
            }
        }

        return "";  // No valid caption for the frame
    }

    int CaptionCache::getNumCaptions(){
        return numCaptions;
    }

} //captions namespace