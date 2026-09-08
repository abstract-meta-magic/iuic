# Protocol abstraction.
Abstract bridge before [backend](./backend.md) and [frontend](./frontend.md).




```c++

namespace my_lib::proto {

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
