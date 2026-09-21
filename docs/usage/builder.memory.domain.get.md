# get()

## Description
Get accessor to the storaged value.

## Behavior
- **Null** : can return null is object does not exist.
- **Return** : `iuic::erasure::visited::as_mutable`.

## Example

```c++
struct point {
  int x,y;
};

auto proxy = ...;

proxy.get().try_visit([](point&p){ ... });

```
