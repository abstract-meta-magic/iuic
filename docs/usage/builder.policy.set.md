# set(...)


Set policy by uid. 

```c++

enum class my_policy {
  unset = 0,
  one,
  two
};

iuic::units::uid uid = ...;

b.policy.set(my_policy::one);
b.policy.set(uid,my_policy::one);

```

## Links
- [uid](./builder.uid.md)
