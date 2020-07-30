#!/usr/bin/ruby

i = 0.0
midi = 0

print "const uint16_t velocityTab[] = {"

while midi <= 127

  if (midi < Math.log(i / 2600.0) * 51.0)
    print "#{i.to_i},"
    midi += 1
  end

  i += 1.0
end

print "};"

puts
