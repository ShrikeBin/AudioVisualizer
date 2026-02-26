#define MINIAUDIO_IMPLEMENTATION
#include <miniaudio.h>

#include "config.h"

#include <stdio.h>

// Called by the sound card whenever it needs more data
void data_callback(ma_device* pDevice, void* pOutput, const void* pInput, ma_uint32 frameCount) 
{
    ma_decoder* pDecoder = (ma_decoder*)pDevice->pUserData;
    if (pDecoder == NULL) return;

    // Read frames from WAV file to the output buffer
    ma_decoder_read_pcm_frames(pDecoder, pOutput, frameCount, NULL);
}

int main(int argc, char** argv) 
{
    if(argc < 2) 
    {
        printf("Usage: %s <filename.wav>\n", argv[0]);
        return -1;
    }

    ma_decoder decoder;
    ma_device_config deviceConfig;
    ma_device device;
    ma_result result;

    // Load the WAV
    result = ma_decoder_init_file(argv[1], NULL, &decoder);
    if(result != MA_SUCCESS) 
    {
        printf("Could not load file: %s\n", argv[1]);
        return -2;
    }

    // Setup playback device
    deviceConfig = ma_device_config_init(ma_device_type_playback);
    deviceConfig.playback.format   = decoder.outputFormat;
    deviceConfig.playback.channels = decoder.outputChannels;
    deviceConfig.sampleRate        = decoder.outputSampleRate;
    deviceConfig.dataCallback      = data_callback;
    deviceConfig.pUserData         = &decoder;

    if(ma_device_init(NULL, &deviceConfig, &device) != MA_SUCCESS) 
    {
        printf("Failed to open playback device.\n");
        ma_decoder_uninit(&decoder);
        return -3;
    }

    if(ma_device_start(&device) != MA_SUCCESS) 
    {
        printf("Failed to start device.\n");
        ma_device_uninit(&device);
        ma_decoder_uninit(&decoder);
        return -4;
    }

    // Get Total Length
    ma_uint64 totalFrames;
    ma_decoder_get_length_in_pcm_frames(&decoder, &totalFrames);
    double duration = (double)totalFrames / decoder.outputSampleRate;

    printf("Playing: %s\n", argv[1]);
    printf("Ctr + C to stop.\n\n");

    while(1) 
    {
        ma_uint64 cursor;
        ma_decoder_get_cursor_in_pcm_frames(&decoder, &cursor);
        
        double elapsed = (double)cursor / decoder.outputSampleRate;

        int cur_min = (int)elapsed / 60;
        int cur_sec = (int)elapsed % 60;
        int tot_min = (int)duration / 60;
        int tot_sec = (int)duration % 60;

        // Print timer
        printf("\rTime: [%02d:%02d / %02d:%02d] \033[K", 
                cur_min, cur_sec, tot_min, tot_sec);
        
        fflush(stdout); 

        if (cursor >= totalFrames) 
        {
            printf("\nFinished playback.\n");
            break; 
        }

        ma_sleep(100); 
    }

    // Cleanup (memory leaks i guess)
    ma_device_uninit(&device);
    ma_decoder_uninit(&decoder);

    return 0;
}