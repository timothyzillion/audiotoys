#include <math.h>
#include <stdbool.h>

// DTMF tones:
//   40ms
// rows:
// 697 Hz
// 770
// 852
// 941
//
// columns:
// 1209
// 1336
// 1477
// 1633

// Ideally you want your target-frequency to be a integer multiple of sampleRate/blockSize
typedef struct goertzel_state_s {
   // parameters
   int targetFreq; // Hz
   int sampleRate; // Hz
   int blockSize;  // # of samples. -> implies bin-width = sampleRate/blockSize, and latency.

   float k;
   float coeff; // precalc, from above

   // per sample
   int currentSample;
   float Q0, Q1, Q2;

   // per block
   bool updatedMagnitude;
   float squaredMagnitude;
} goertzel_state;

// Initialize a bank of states
extern void goertzel_init(goertzel_state *states);
extern void goertzel_update(goertzel_state *states, float sample);
