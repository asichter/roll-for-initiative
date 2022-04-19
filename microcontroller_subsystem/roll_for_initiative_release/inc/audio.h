#ifndef AUDIO_H_
#define AUDIO_H_

// Note Values
float F4 = 349.230;
float G4 = 392.000;
float A4 = 440.000;


struct Tone {
    float freq;
    float len;
}  intro[] = {
        {A4, 4},
        {G4, 1},
        {A4, 2},
        {F4, 3},
};

#endif
