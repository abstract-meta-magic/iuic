# get()

## Description
Gets access to the object.

## Behavior
- **Return** : `iuic::erasure::visited::as_mutable` - type-erasure handle that support `try_visit`.
- **Null** : Returns invalid handle if the object does not exist or the type mismatch.

## Use-case
Use when you need custom behavior depending on the object's state.

## Example

```c++

struct point {
  int x,y;
};

iuic::units::uid uid = ...;

auto persist = b.memory.persist<point>(uid);

...

persist.get().try_visit([](point& p){
  ...
});

```
