#ifndef AUDIO_H_
#define AUDIO_H_

// Note Values
#define F4 349.230
#define G4 392.000
#define A4 440.000

struct Tone {
    float freq;
    float len;
}  INTRO[] = {
        {A4, 4.0},
        {G4, 1.0},
        {A4, 2.0},
        {F4, 3.0},
};

#endif
