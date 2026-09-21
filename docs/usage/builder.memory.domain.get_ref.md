# get()

## Description
Get reference to the storaged value.

## Behavior
- **Exeption** : if object does not exist.

## Example

```c++
struct point {
  int x,y;
};

auto proxy = ...;

point& p = proxy.get_ref();

```
