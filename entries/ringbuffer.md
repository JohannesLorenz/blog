# Ringbuffer where you can write until the read position

Most ringbuffer implementations I've seen, e.g. the one of
[Jack](https://jackaudio.org), let you only write until "read position minus
one" (minus in a cyclic sense). This means that the write capacity will never
exceed "buffer size minus one". The reason is clear: If the write position
ever hits the read position, then you can not tell whether the write capacity
is now "size" or "0" - at least not by just looking at both positions.
You could add a boolean "write hit read", but this will only complicate
code and add branches to your code which can cause slowdowns by branch
misprediction. Also, if you have more than just "read" and "write" positions
(e.g. "write requested", "written", "read"), this can get really complex.

For my [wavetable implementation](https://github.com/zynaddsubfx/zynaddsubfx/pull/61)
of [ZynAddSubFX](https://zynaddsubfx.sourceforge.io/), I felt I needed a
special ringbuffer. The reason was that this code often used ringbuffers of
size "1", where at least one element should have already been written and be
ready to be read (or peaked). This would not work with the "normal" ringbuffer
approach, since the maximum write capacity would be "size minus one", i.e.
"zero".

The solution? I wrote a ringbuffer where the internal read/write positions
have double the range of the buffer size (e.g. for a buffer size of 8, they
run from 0 to 15). To get the buffer position from the internal read/write
position, you just remove the highest bit (e.g. 3 and 3+8=11 point both to
the fourth buffer element). Assuming the ringbuffer size is a power of two,
the read/write spaces are computed as

```
read_space  =         (2*size + write_pos - read_pos) % (size*2)
write_space = size - read_space
              size - ((2*size + write_pos - read_pos) % (size*2))
```

As long as you make sure that the write position never advances
more than "size" elements ahead of the read position, the second subtrahend
of the write space formula can be in range `[0, size]`, and so can be the
write space.

Precomputing some of those variables when resizing the buffer, this can be
optimized to

```
// initialization when resizing buffer
size_x2      = size * 2
size_x2_mask = size * 2 - 1
// ...
// computing read space at any time
read_space  =        (write_pos - read_pos + size_x2) & (size_x2_mask)
write_space = size - (write_pos - read_pos + size_x2) & (size_x2_mask)
```

Click [here](../code/ringbuffer.cpp) for a complete implementation.

Compare this algorithm to the "normal" ringbuffer implementation. Isn't this
even more beautiful than messing around with "minus one" all the time?
