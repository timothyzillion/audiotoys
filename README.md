audiotoys
=========

I've been thinking about some audio stuff recently; and it reminded me of some simple
stuff I'd written a long long time ago for generating/recognizing DTMF tones 
(a quick google search shows that there are maybe a lot of resources for this kind
of stuff now). I actually dug it up off of some old backup media.

The basic stuff is a quick&dirty implementation of the Goertzel Algorithm:
http://en.wikipedia.org/wiki/Goertzel_algorithm
(the intent is that this code would be suitable, especially the integer-only version,
for use by relatively small embedded systems).

Also to try out the basic stuff, I wrote a toy in ruby to generate tones into a .wav
file.
