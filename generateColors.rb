#!/usr/bin/ruby

# require 'color_math'
# https://martin.ankerl.com/2009/12/09/how-to-create-random-colors-programmatically/
def hsv_to_rgb(h, s, v)
  h_i = (h*6).to_i
  f = h*6 - h_i
  p = v * (1 - s)
  q = v * (1 - f*s)
  t = v * (1 - (1 - f) * s)
  r, g, b = v, t, p if h_i==0
  r, g, b = q, v, p if h_i==1
  r, g, b = p, v, t if h_i==2
  r, g, b = p, q, v if h_i==3
  r, g, b = t, p, v if h_i==4
  r, g, b = v, p, q if h_i==5
  [(r*255).to_i, (g*255).to_i, (b*255).to_i]
end

hueColors = [
  11 , # Orange
  22 , # Light Orange
  45 , # Warm Yellow
  62 , # Yellow
  90 , # Lime
  112, # Green
  135, # Mint
  174, # Cyan
  202, # Turquoise
  225, # Blue
  247, # Plum
  259, # Violet
  270, # Purple
  304, # Magenta
  343, # Fuchia
  0  , # Red
]

# 8 bright, 16 colors, 3 components
print "const PROGMEM uint8_t colors[8*16*3] = {\n"

print "// 0: OFF \n"
hueColors.each do |hue|
  print "  0x00,0x00,0x00,"
end
print "\n"

print "// 1-6: max saturation & value: 1/6 .. 6/6\n"
[1,2,3,4,5,6].each do |bright|
  hueColors.each do |hue|
    r,g,b = hsv_to_rgb(hue/360.0, 1.0, bright/6.0)
    print "  0x#{"%02X" % g},0x#{"%02X" % r},0x#{"%02X" % b},"
  end
  print "\n"
end

print "// 7: max value & saturation: 0.8 \n"
hueColors.each do |hue|
  r,g,b = hsv_to_rgb(hue/360.0, 0.8, 1.0)
  print "  0x#{"%02X" % g},0x#{"%02X" % r},0x#{"%02X" % b},"
end
print "\n"

print "};\n"
