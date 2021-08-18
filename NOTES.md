Notes on XDP and eBPF Development
---------------------------------
A couple of notes which seem to make for smoother eBPF and XDP programs:

* Always use unsigned values and signed values consistently.
* When reading a length from a packet and using it as an offset into a packet,
  make sure to bound it via something like this:

```c:
  void *data = (void *)(long)ctx->data;
  const __u8 *data = data + 100;

  len = ((size_t)data[3] << 8) +
      (size_t)data[4];
  len = len & 0x20;
```

* Since eBPF programs are limited to 4K, logically breaking your program up
  into chunks and using tail calls is the best path forward.
* Use per-cpu maps to pass data across tail calls.
* Use per-cpu maps instead of the stack for variables.
* In general, use maps for debugging and tracing as you can write userspace
  programs to interpret the maps.
