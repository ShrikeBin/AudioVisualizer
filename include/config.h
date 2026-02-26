#ifndef CONFIG_H
#define CONFIG_H

// General configuration
/*
    N: Number of samples per FFT (must be a power of 2)
    FPS: Frames per second for visualization updates
    SMOOTHING: Smoothing factor for jumping bars (0.0 to 1.0)
    MAX_HEIGHT: Maximum height of the visualizer bars in the terminal
    SENSITIVITY: Multiplier for bar heights

    BLOCKS: Unicode characters for different bar heights

    BASS_BINS: Frequency bins for bass frequencies
    MID_BINS: Frequency bins for mid frequencies
    HIGH_BINS: Frequency bins for high frequencies
*/

int N = 4096;
int FPS = 60;
double SMOOTHING = 0.8;
int MAX_HEIGHT = 15;
double SENSITIVITY = 3.0;

char* BLOCKS[] = {" ", " ", "▂", "▃", "▄", "▅", "▆", "▇", "█"};

int BASS_BINS[] = 
{
    20, 30, 40, 50, 
    60, 70, 80, 90, 
    100, 110, 120, 130, 
    140, 150, 160, 170, 
    180, 190, 200
};
int MID_BINS[] = 
{
    250, 300, 350, 400, 
    450, 500, 550, 600, 
    650, 700, 750, 800, 
    850, 900, 950, 1000
};
int HIGH_BINS[] = 
{
    1500, 2000, 2500, 3000, 
    3500, 4000, 4500, 5000, 
    5500, 6000, 6500, 7000, 
    7500, 8000, 8500, 9000, 
    9500, 10000, 10500, 11000, 
    11500, 12000, 12500, 13000, 
    13500, 14000, 14500, 15000, 
    15500, 16000, 16500, 17000, 
    17500, 18000
};

#endif // CONFIG_H