require 'rubygems'
require 'wavefile'

include WaveFile

data = "this is a test string."

# bitwise MSB first
data_bits = []
data.each_byte do |c|
   8.times do |i|
      bitmask = (1 << (7-i))
      bit = (bitmask & c == bitmask) ? 1 : 0 
      # put the bit at the end of the array
      data_bits.push bit
   end
end

# sample-rate in HZ

sample_rate = ARGV[0].to_i
if sample_rate == 0
 sample_rate = 8000
end
#sample_rate = 44100

# symbol duration in milliseconds
# ITU DTMF are 40ms
symbols_per_second = 25
symbols_per_second = 100

# how many samples per symbol ?
symbol_samples = sample_rate / symbols_per_second
puts "Sample-rate #{sample_rate} each symbol uses #{symbol_samples} samples"

# in hertz.
mark_frequency = 697
mark_angular_frequency = mark_frequency * 2 * Math::PI

space_frequency = 1209
space_angular_frequency = space_frequency * 2 * Math::PI

total_samples = data_bits.length * symbol_samples

Writer.new("fsk.wav", Format.new(:mono, :pcm_16, sample_rate)) do |writer|

  sample = 0
  phase = 0
  data_bits.each do |bit|
    angularFreq = bit == 1 ? mark_angular_frequency : space_angular_frequency

    samples = []
    symbol_samples.times do
      samples <<  Math.sin((sample * angularFreq / sample_rate) - phase)
      sample += 1
    end

    phase = (sample * angularFreq / sample_rate) + phase
    phase = phase % (2 * Math::PI)

    buffer_format = Format.new(:mono, :float, sample_rate)
    buffer = Buffer.new(samples, buffer_format)
    writer.write(buffer)
  end
end
