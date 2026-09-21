# get(...)

## Description
Get policy by uid.

## Behavior
- **Default** : use self uid.
- **Unset** : If the policy is not set, then 'unset == 0' value is returned.

## Use-case
If you need behavior in relation to the specified policy.

## Example

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
