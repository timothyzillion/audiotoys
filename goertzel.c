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

   float coeff; // precalc, from above

   // per sample
   int currentSample;
   float Q0, Q1, Q2;

   // per block
   bool updatedMagnitude;
   float squaredMagnitude;
} goertzel_state;

// Initialize a bank of states
void
goertzel_init(goertzel_state *states)
{
   int i;

   for (i = 0; states[i].blockSize != 0; i++) {
      float k, omega;

      states[i].currentSample = 0;
      states[i].Q0 = 0;
      states[i].Q1 = 0;
      states[i].Q2 = 0;
      states[i].squaredMagnitude = 0;

      // calculate the coeff;
      k = 0.5 * (float)states[i].blockSize * (float)states[i].targetFreq / (float)states[i].sampleRate;
      omega = (2.0 * M_PI * k) / ((float)states[i].blockSize);
      states[i].coeff = 2.0 * cos(omega);
   }
   return;
}

void
goertzel_update(goertzel_state *states, float sample)
{
   int i;

   for (i = 0; states[i].blockSize != 0; i++) {
      states[i].Q0 = states[i].coeff * states[i].Q1 + sample - states[i].Q2;
      states[i].Q2 = states[i].Q1;
      states[i].Q1 = states[i].Q0;

      states[i].currentSample++;
      if (states[i].currentSample == states[i].blockSize) {
        states[i].currentSample = 0;
        states[i].updatedMagnitude = true;

        // mag^2 = Q1^2 + Q1*Q2 - coeff*Q1*Q2
        states[i].squaredMagnitude =  states[i].Q1 * states[i].Q1;
        states[i].squaredMagnitude += states[i].Q1 * states[i].Q2;
        states[i].squaredMagnitude -= states[i].coeff * states[i].Q1 * states[i].Q2;
      } else {
        states[i].updatedMagnitude = false;
      }
   }
}
