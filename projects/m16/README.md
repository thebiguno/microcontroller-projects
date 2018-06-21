https://esdblog.org/how-to-drive-4096-rgb-leds-each-24bit-rgb-brightness-with-one-stm32-microcontroller-without-much-cpu-load/

4-bits per pixel, expanding to 16-bit per pixel for gamma adjustment
see http://www.batsocks.co.uk/readme/p_art_bcm.htm
0     0
1    17/32767 = 0.0005%
2   116
3   362
4   809
5  1512
6  2519
7  3878
8  5637
9  7839
a 10529
b 13749
c 17542
d 21949
e 27011
f 32767

64x32 matrix w/ 1/16 scan rate
2048 rgb leds, 16 * 128 rgb rows

3072 byte frame buffer ?
dma buffer: [x:x:x:b1:g1:r1:ck:b0:g0:r0:oe:st:d:c:b:a] ?
2 rgb leds per dma value
64 dma values per row
16384 16-bit dma values per frame

