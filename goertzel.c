#include <math.h>
#include <stdbool.h>


#include "goertzel.h"

// Initialize a bank of states
void
goertzel_init(goertzel_state *states)
{
   int i;

   for (i = 0; states[i].blockSize != 0; i++) {
      float k;
      float omega;

      states[i].currentSample = 0;
      states[i].Q0 = 0;
      states[i].Q1 = 0;
      states[i].Q2 = 0;
      states[i].squaredMagnitude = 0;

      // calculate the coeff;
      k = (float)states[i].targetFreq / (float)states[i].sampleRate;
      omega = (2.0 * M_PI * k);
      states[i].k = k;
      states[i].coeff = 2.0 * cos(omega);
   }
   return;
}

void
goertzel_update(goertzel_state *states, float sample)
{
   int i;

   for (i = 0; states[i].blockSize != 0; i++) {
      states[i].Q0 = (states[i].coeff * states[i].Q1) + sample - states[i].Q2;
      states[i].Q2 = states[i].Q1;
      states[i].Q1 = states[i].Q0;

      states[i].currentSample++;
      if (states[i].currentSample == states[i].blockSize) {
        states[i].currentSample = 0;
        states[i].updatedMagnitude = true;

        // mag^2 = Q1^2 + Q2^2 - coeff*Q1*Q2
        states[i].squaredMagnitude =  (states[i].Q1 * states[i].Q1) + (states[i].Q2 * states[i].Q2) - (states[i].coeff * states[i].Q1 * states[i].Q2);

        states[i].Q0 = states[i].Q1 = states[i].Q2 = 0.0;
      } else {
        states[i].updatedMagnitude = false;
      }
   }
}
