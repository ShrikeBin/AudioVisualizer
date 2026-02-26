#define MINIAUDIO_IMPLEMENTATION
#include <miniaudio.h>

#include "config.h"

#include <stdio.h>
#include <termios.h>
#include <unistd.h>
#include <fcntl.h>
#include <stdlib.h>

// Called by the sound card whenever it needs more data
void data_callback(ma_device* pDevice, void* pOutput, const void* pInput, ma_uint32 frameCount) 
{
    ma_decoder* pDecoder = (ma_decoder*)pDevice->pUserData;
    if (pDecoder == NULL) return;

    // Read frames from WAV file to the output buffer
    ma_decoder_read_pcm_frames(pDecoder, pOutput, frameCount, NULL);
}

struct termios orig_termios;

void disableRawMode() 
{
    tcsetattr(STDIN_FILENO, TCSAFLUSH, &orig_termios);
}

void enableRawMode() 
{
    // Save current terminal state
    tcgetattr(STDIN_FILENO, &orig_termios);
    // Tell Linux to run disableRawMode when the program exits
    atexit(disableRawMode);

    struct termios raw = orig_termios;
    // ECHO: stop printing keys back to the screen
    // ICANON: stop waiting for the Enter key (read byte-by-byte)
    raw.c_lflag &= ~(ECHO | ICANON);

    tcsetattr(STDIN_FILENO, TCSAFLUSH, &raw);

    // Set input to non-blocking so getchar() doesn't freeze the timer
    int flags = fcntl(STDIN_FILENO, F_GETFL, 0);
    fcntl(STDIN_FILENO, F_SETFL, flags | O_NONBLOCK);
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

    ma_uint64 totalFrames;
    ma_decoder_get_length_in_pcm_frames(&decoder, &totalFrames);
    double duration = (double)totalFrames / decoder.outputSampleRate;

    printf("Playing: %s\n", argv[1]);
    printf("[Q] to stop.\n");
    printf("[J] to jump back 10 seconds.\n");
    printf("[K] to pause/resume playback.\n");
    printf("[L] to jump forward 10 seconds.\n\n");

    enableRawMode();

    while(1) 
    {
        char c;
        while (read(STDIN_FILENO, &c, 1) == 1) 
        { 
            ma_uint64 currentPos;
            ma_decoder_get_cursor_in_pcm_frames(&decoder, &currentPos);
            ma_uint64 step = decoder.outputSampleRate * 10;

            if (c == 'j') 
            {
                ma_uint64 newPos;
                if(currentPos > step) 
                {
                    newPos = currentPos - step;
                } 
                else
                {
                    newPos = 0;
                }
                ma_decoder_seek_to_pcm_frame(&decoder, newPos);
            } 
            else if (c == 'l') 
            {
                ma_uint64 newPos = currentPos + step;
                if (newPos >= totalFrames) 
                {
                    newPos = totalFrames; 
                }
                ma_decoder_seek_to_pcm_frame(&decoder, newPos);
            } 
            else if (c == 'k') 
            {
                if (ma_device_is_started(&device))
                {
                    ma_device_stop(&device);
                }
                else 
                {
                    ma_device_start(&device);
                }
            } 
            else if (c == 'q') 
            {
                goto cleanup;
            }
        }

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
            goto cleanup;
        }
        ma_sleep(50);
    }

cleanup:
    disableRawMode();
    ma_device_uninit(&device);
    ma_decoder_uninit(&decoder);

    return 0;
}