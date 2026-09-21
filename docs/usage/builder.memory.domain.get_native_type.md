# get_native_type()

## Description
Gets native type.

## Behavior
- **Return** : `const iuic::erasure::type *`.

## Example

```c++

struct point {
  int x,y;
};

auto proxy = ...;

if(proxy.get_native_type() == iuic::erasure::type::from<point>()) {
  ...
};

```
