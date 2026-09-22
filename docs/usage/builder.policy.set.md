# set(...)

## Description
Set policy by uid.

## Behavior
- **Default** : use self uid.
- **Replace** : If the policy has already been assigned, the value is replaced.

## Use-case
If you need behavior in relation to the specified policy.

## Example

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
