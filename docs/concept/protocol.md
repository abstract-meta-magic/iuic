# Protocol abstraction.

## Discription
The common set of definitions for [backend](./backend.md)  and [frontend](./frontend.md).

## For what
Abstract bridge before [backend](./backend.md) and [frontend](./frontend.md).

## What is included in the set?
### State
...
```c++
constexpr iuic::state::decl hovered;
```
### Policy
...
```c++
struct hover {
  unset = 0,
  propagate,
  block,
};
```
### Event
#### Channel
#### Packager
- type
- pkgs
  


```c++

namespace my_lib::proto {

namespace policy {
struct hover {
  unset = 0,
  propagate,
  block,
};
};

namespace state {
constexpr iuic::state::decl hovered;

constexpr iuic::state::decl focused;

...
}

namespace event {
struct key { ... };
struct pointer { ... };

constexpr iuic::event::channel local;
constexpr iuic::event::channel global;
};
};


```
