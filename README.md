## Audio visualizer written in C

`work in progress -> see ROADMAP.md for details`

So far uses `miniaudio` and `kiss_ftt` but eventually will have their functions rewritten

requires files (not present in this repository):
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