## Audio visualizer written in C

So far uses `miniaudio` and `kiss_ftt` but eventually will have their functions rewritten

requires files:
```
lib/
├── kiss_fft.c
├── _kiss_fft_guts.h
├── kiss_fft.h
├── kiss_fft_log.h
└── miniaudio.h
```

### Compile and Run

```sh
make
./visualizer <filename.wav>
```