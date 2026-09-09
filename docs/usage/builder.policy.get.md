# get(...)



Get policy by uid.


```c++
enum class my_poilyc {
  unset = 0,
  one,
  two
};

iuic::units::uid uid = ...;

auto policy = b.policy.get<my_policy>();
auto policy = b.policy.get<my_policy>(uid);

```

## Links
- [uid](./builder.uid.md)
