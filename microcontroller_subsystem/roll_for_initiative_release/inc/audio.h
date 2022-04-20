#ifndef AUDIO_H_
#define AUDIO_H_

// Note Values
#define F4 349.230
#define G4 392.000
#define A4 440.000

struct Tone {
    float freq;
    float next_psc;
}  startup_a[] = {
        {261.63, 2822},
        {466.16, 2822},
        {261.63, 2822},
        {466.16, 2822},
        {392.0, 2822},
        {466.16, 2822},
        {261.63, 8469},
    },
    startup_b[] = {
        {196.0, 2822},
        {174.61, 2822},
        {196.0, 2822},
        {174.61, 2822},
        {155.56, 2822},
        {174.61, 2822},
        {196.0, 8469},
    },
    startup_c[] = {
        {130.81, 2822},
        {130.81, 2822},
        {130.81, 2822},
        {233.08, 2822},
        {233.08, 2822},
        {233.08, 2822},
        {130.81, 8469},
    };

#endif