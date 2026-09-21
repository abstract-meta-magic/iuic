# detach(...)

## Description
Check uid has state.

## Behavior
- **Default** : use self **uid**.
- **No-op** : if state doesn't exist.

## Use-case
Unique behavior based on the presence of a state.

## Example

```c++

static constexpr iuic::state::decl state;
iuic::units::uid uid = ...;

b.state.detach(uid,state);
b.state.detach(state);

```

## Links
- [uid](./builder.uid.md)
