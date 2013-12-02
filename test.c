//
// Read header to get raw samples is code from:
// http://yannesposito.com/Scratch/en/blog/2010-10-14-Fun-with-wav/
//

#include <stdio.h>
#include <stdlib.h>
#include <string.h> // for memcmp
#include <stdint.h> // for int16_t and int32_t

#include "goertzel.h"

#define SYMBOLS_PER_SECOND 10
#define DUTY_CYCLE 75 // 90% mark, 10% space

typedef struct {
    uint16_t count;
    float total;
    float samples[0];
} envelope_t;

static envelope_t *
initEnvelope(uint16_t count)
{
    int i;
    envelope_t *res;

    res = (envelope_t *)malloc(sizeof(envelope_t) + (count * sizeof(float)));
    if (res == NULL) {
        return NULL;
    }

    res->count = count;
    res->total = 0.0;
    for (i=0; i < count; i++) {
        res->samples[i] = 0.0;
    }

    return res;
}

static float
updateEnvelope(float sample, int pos, envelope_t *envelope)
{
    int index = pos % envelope->count;

    float mag = sample * sample;
    mag = 2.0 * mag;
    mag = sqrtf(mag);

    envelope->total = envelope->total - envelope->samples[index] + mag;
    envelope->samples[index] = mag;

    return envelope->total / envelope->count;
}

struct wavfile
{
    char    id[4];          // should always contain "RIFF"
    int32_t totallength;    // total file length minus 8
    char    wavefmt[8];     // should be "WAVEfmt "
    int32_t format;         // 16 for PCM format
    int16_t pcm;            // 1 for PCM format
    int16_t channels;       // channels
    int32_t frequency;      // sampling frequency
    int32_t bytes_per_second;
    int16_t bytes_by_capture;
    int16_t bits_per_sample;
    char    data[4];        // should always contain "data"
    int32_t bytes_in_data;
} __attribute__((__packed__));

int is_big_endian(void) {
    union {
        uint32_t i;
        char c[4];
    } bint = {0x01000000};
    return bint.c[0]==1;
}

int main(int argc, char *argv[]) {
    char *filename=argv[1];
    FILE *wav = fopen(filename,"rb");
    struct wavfile header;

    if ( wav == NULL ) {
        fprintf(stderr,"Can't open input file %s\n", filename);
        exit(1);
    }

    // read header
    if ( fread(&header,sizeof(header),1,wav) < 1 ) {
        fprintf(stderr,"Can't read input file header %s\n", filename);
        exit(1);
    }

    // if wav file isn't the same endianness than the current environment
    // we quit
    if ( is_big_endian() ) {
        if (   memcmp( header.id,"RIFX", 4) != 0 ) {
            fprintf(stderr,"ERROR: %s is not a big endian wav file\n", filename);
            exit(1);
        }
    } else {
        if (   memcmp( header.id,"RIFF", 4) != 0 ) {
            fprintf(stderr,"ERROR: %s is not a little endian wav file\n", filename);
            exit(1);
        }
    }

    if (   memcmp( header.wavefmt, "WAVEfmt ", 8) != 0
           || memcmp( header.data, "data", 4) != 0
        ) {
        fprintf(stderr,"ERROR: Not wav format\n");
        exit(1);
    }
    if (header.format != 16) {
        fprintf(stderr,"\nERROR: not 16 bit wav format.");
        exit(1);
    }
    fprintf(stderr,"format: %d bits", header.format);
    if (header.format == 16) {
        fprintf(stderr,", PCM");
    } else {
        fprintf(stderr,", not PCM (%d)", header.format);
    }
    if (header.pcm == 1) {
        fprintf(stderr, " uncompressed" );
    } else {
        fprintf(stderr, " compressed" );
    }
    fprintf(stderr,", channel %d", header.pcm);
    fprintf(stderr,", freq %d", header.frequency );
    fprintf(stderr,", %d bytes per sec", header.bytes_per_second );
    fprintf(stderr,", %d bytes by capture", header.bytes_by_capture );
    fprintf(stderr,", %d bits per sample", header.bits_per_sample );
    fprintf(stderr,"\n" );

    if ( memcmp( header.data, "data", 4) != 0 ) {
        fprintf(stderr,"ERROR: Prrroblem?\n");
        exit(1);
    }
    fprintf(stderr,"wav format\n");

    // read data
    int16_t value;
    int i=0;

    int j;
    goertzel_state states[9];
//    int blockSize = 205; // 100 -> 80Hz per bin @8KHz sampleRate.
    int blockSize = 205; // 100 -> 80Hz per bin @8KHz sampleRate.

    memset(states, 0, sizeof(states));
    states[0].blockSize = blockSize;
    states[0].targetFreq = 697;
    states[0].sampleRate = header.frequency;

    states[1].blockSize = blockSize;
    states[1].targetFreq = 770;
    states[1].sampleRate = header.frequency;

    states[2].blockSize = blockSize;
    states[2].targetFreq = 852;
    states[2].sampleRate = header.frequency;

    states[3].blockSize = blockSize;
    states[3].targetFreq = 941;
    states[3].sampleRate = header.frequency;

    states[4].blockSize = blockSize;
    states[4].targetFreq = 1209;
    states[4].sampleRate = header.frequency;

    states[5].blockSize = blockSize;
    states[5].targetFreq = 1336;
    states[5].sampleRate = header.frequency;

    states[6].blockSize = blockSize;
    states[6].targetFreq = 1477;
    states[6].sampleRate = header.frequency;

    states[7].blockSize = blockSize;
    states[7].targetFreq = 1633;
    states[7].sampleRate = header.frequency;

    goertzel_init(states);

    printf("k[0]: %f coeff[0]: %f\n", states[0].k, states[0].coeff);
    printf("k[1]: %f coeff[1]: %f\n", states[1].k, states[1].coeff);
    printf("k[2]: %f coeff[2]: %f\n", states[2].k, states[2].coeff);
    printf("k[3]: %f coeff[3]: %f\n", states[3].k, states[3].coeff);

    printf("k[4]: %f coeff[4]: %f\n", states[4].k, states[4].coeff);
    printf("k[5]: %f coeff[5]: %f\n", states[5].k, states[5].coeff);
    printf("k[6]: %f coeff[6]: %f\n", states[6].k, states[6].coeff);
    printf("k[7]: %f coeff[7]: %f\n", states[7].k, states[7].coeff);

    float magnitudes[8];

    int currentOutput=-1;
    int updates_with_current_output=0;

    int symbolSamples = header.frequency / SYMBOLS_PER_SECOND;
    int symbolBlocks = symbolSamples / blockSize;

    int markSamples = (symbolSamples * DUTY_CYCLE) / 100;
    int spaceSamples = symbolSamples - markSamples;

    envelope_t *envelope = initEnvelope(spaceSamples);
    if (envelope == NULL) {
        printf("Allocation of integrator failed\n");
        exit(-1);
    }

    fprintf(stderr, "mark is %d samples long, spaceEnvelope has %d samples\n", markSamples, spaceSamples);

    int position = 0;
    int space = 0;
    bool mark=false;
    while(fread(&value,sizeof(value),1,wav)) {
        // got a new sample of value (16-bit)
        if (header.format == 16) {
            float sample = (float)value / 32768.0;

            float curEnvelope = updateEnvelope(sample, position, envelope);
            position++;

            goertzel_update(states, sample);

            bool update=false;
            for (j=0; states[j].blockSize != 0; j++) {
                if (states[j].updatedMagnitude) {
                    printf("Updated magnitude: sample: %d time: %f target %d %f\n", i, i/(float)header.frequency , states[j].targetFreq, states[j].squaredMagnitude);
                    update = true;
                    magnitudes[j] = states[j].squaredMagnitude;
                }
            }

            printf("position: %d envelope: %f\n", position, curEnvelope);

            if (update) {
                if (mark) {
                    // waiting for a space
                    if (curEnvelope < 0.1) {
                        mark = false;
                    }
                    // don't bother decoding.
                    continue;
                } else {
                    // waiting for mark.
                    if (curEnvelope > 0.55) {
                        mark = true;
                    } else {
                        continue;
                    }
                }

                int row=0, column=4;
                for (j=1; j < 4; j++) {
                    if (magnitudes[j] > magnitudes[row]) {
                        row = j;
                    }
                }
                for (j=5; j < 8; j++) {
                    if (magnitudes[j] > magnitudes[column]) {
                        column = j;
                    }
                }
                column -= 4;

                int newOutput = row<<2 | column;
                printf("row %d col %d ----> %d\n", row, column, newOutput);
            }
        }
        i++;
    }
    exit(0);
}
