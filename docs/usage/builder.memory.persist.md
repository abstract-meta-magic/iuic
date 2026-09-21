# persist

## Purpose 
Stable memory between ticks.

## Behavior
- **Uniqueness** : the uid is used.
- **Registration** : Reserves or gets a `slot` in an open-address map. No memory is allocated at this point.
- **Access** : All access to the stored object goes through the proxy (see below).

## Lifetime
- 3 tick

## Use-case
Use this when you need an object whose address remains valid for several ticks.

## Proxy 
### Purpose
Provides control operations over a reserved slot.

### Members
- [init_if_not](./builder.memory.persist.init_if_not.md)
- [get](./builder.memory.persist.get.md)

## Example
```c++

iuic::units::uid uid = ...;

struct point { 
  int x,y;
};

auto persist = builder.memory.persist<point>(uid);
```
