# state

## Purpose
Access to the element's state.

## Context
```c++
static constexpr iuic::state::decl my_state;
iuic::scheme::explorer ex;
...
for (auto el : ex.ranges.level_order()) {
  ...
};
```
## Methods
### has
Check whether the element has a state.
```c++
if (ex.state.has(el,my_state)) {
  ...
}
```
### attach
Attach state to the element's
```c++
ex.state.attach(el,my_state);
```
### detach
Detach state from the element's
```c++
ex.state.detach(el,my_state);
```

## Links
- [ranges](./scheme.ranges.md)
- [builder state](./builder.state.md)
