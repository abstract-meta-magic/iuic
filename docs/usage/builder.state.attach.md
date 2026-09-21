# attach(...)

## Description
Attach state to uid.

## Behavior
- **Default** : use self **uid**.
- **No-op** : if state already exist.

## Use-case
Unique behavior based on the presence of a state.

## Example

```c++

static constexpr iuic::state::decl state;

iuic::units::uid uid = ...;

b.state.attach(uid,state);
b.state.attach(state);

```

## Links
- [uid](./builder.uid.md)
