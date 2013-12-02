require 'rubygems'
require 'wavefile'

include WaveFile

def sinusoid(linearFreq, sampleRate, angle)
  angularFreq = linearFreq * 2 * Math::PI
  Math.sin((angle * angularFreq / sampleRate))
end

def dtmf(symbol, sample_rate, sample)

  # in hertz.
  row = [ 0, 697, 770, 852, 941]
  column = [ 0, 1209, 1336, 1477, 1633]

  output = 0.0
  case symbol
  when 0
    output += sinusoid(row[1], sample_rate, sample)
    output += sinusoid(column[1], sample_rate, sample)
  when 1
    output += sinusoid(row[1], sample_rate, sample)
    output += sinusoid(column[2], sample_rate, sample)
  when 2
    output += sinusoid(row[1], sample_rate, sample)
    output += sinusoid(column[3], sample_rate, sample)
  when 3
    output += sinusoid(row[1], sample_rate, sample)
    output += sinusoid(column[4], sample_rate, sample)
  when 4
    output += sinusoid(row[2], sample_rate, sample)
    output += sinusoid(column[1], sample_rate, sample)
  when 5
    output += sinusoid(row[2], sample_rate, sample)
    output += sinusoid(column[2], sample_rate, sample)
  when 6
    output += sinusoid(row[2], sample_rate, sample)
    output += sinusoid(column[3], sample_rate, sample)
  when 7
    output += sinusoid(row[2], sample_rate, sample)
    output += sinusoid(column[4], sample_rate, sample)
  when 8
    output += sinusoid(row[3], sample_rate, sample)
    output += sinusoid(column[1], sample_rate, sample)
  when 9
    output += sinusoid(row[3], sample_rate, sample)
    output += sinusoid(column[2], sample_rate, sample)
  when 10
    output += sinusoid(row[3], sample_rate, sample)
    output += sinusoid(column[3], sample_rate, sample)
  when 11
    output += sinusoid(row[3], sample_rate, sample)
    output += sinusoid(column[4], sample_rate, sample)
  when 12
    output += sinusoid(row[4], sample_rate, sample)
    output += sinusoid(column[1], sample_rate, sample)
  when 13
    output += sinusoid(row[4], sample_rate, sample)
    output += sinusoid(column[2], sample_rate, sample)
  when 14
    output += sinusoid(row[4], sample_rate, sample)
    output += sinusoid(column[3], sample_rate, sample)
  when 15
    output += sinusoid(row[4], sample_rate, sample)
    output += sinusoid(column[4], sample_rate, sample)
  end

  # normalize
  output = output / 2.0 # Added two measurements, normalize.
  return output
end

data = "this is a test string."

data_nibbles = []

data.each_byte do |c|
  data_nibbles << ((c >> 4) & 0x0f)
  data_nibbles << (c & 0x0f)
end

sample_rate = ARGV[0].to_i
if sample_rate == 0
 sample_rate = 8000
end
#sample_rate = 44100

# symbol duration in milliseconds
# ITU DTMF are 40ms
symbols_per_second = 10
#symbols_per_second = 25
#symbols_per_second = 100

# how many samples per symbol ?
symbol_samples = sample_rate / symbols_per_second
puts "Sample-rate #{sample_rate} each symbol uses #{symbol_samples} samples: #{4*symbols_per_second}bps"

# marks are 9 times longer than spaces
mark_space_duty_cycle = 75
mark_samples = (symbol_samples * mark_space_duty_cycle) / 100
space_samples = symbol_samples - mark_samples

Writer.new("dtmf.wav", Format.new(:mono, :pcm_16, sample_rate)) do |writer|
  sample = 0
  symbolIndex = 0
  samples = []
  puts data_nibbles[symbolIndex]

  while (1)
    symbol = data_nibbles[symbolIndex]

    if (sample % symbol_samples < mark_samples)
      # mark
      value = dtmf(symbol, sample_rate, sample)
    else
      # space
      value = 0.0
    end

    samples << value

    sample += 1
    if sample % symbol_samples == 0
      symbolIndex += 1
      puts data_nibbles[symbolIndex]
      # clock = clock ^ 1 ... etc
      break if (symbolIndex == data_nibbles.length)
    end
  end

  buffer_format = Format.new(:mono, :float, sample_rate)
  buffer = Buffer.new(samples, buffer_format)
  writer.write(buffer)
end

