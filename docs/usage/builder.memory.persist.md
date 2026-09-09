# persist


## For what

## Lifetime


```c++
struct point {
 int x,y;
};

iuic::units::uid uid = ...;

auto persist = b.memory.persist<point>(uid);

persist.init_if_not([&]() -> point {
  ...
});

persist.get().try_visit([](point&){ ... });


```


## Links
- [uid](./builder.uid.md)

Erasure ???
