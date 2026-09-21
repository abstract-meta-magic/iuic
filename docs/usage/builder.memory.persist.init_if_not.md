# init_if_not(...)

## Description
Creates the object if it does not exist yet; otherwise refreshes its lifetime.

## Behavior
- **Allocate** : Slab allocation, only on first creation.
- **Chunk size** : 8, 16, 32, 64, 128 bytes.
- **Update** : On each call, if the type matches and the object is not outdated, the lifetime is updated.
- **Type mismatch** : No-op.

## Use-case
Use this to lazily create a persistent object on first access, or to refresh its lifetime on subsequent calls.

## Example

```c++

struct point {
  int x,y;
};

iuic::units::uid uid = ...;

auto persist = b.memory.persist<point>(uid);

persist.init_if_not([] {
  return point{4,4};
});

```

## Note
Rename to init_or_update ???
