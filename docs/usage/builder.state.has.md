# has(...)

## Description
Check uid has state.

## Behavior
- **Default** : use self **uid**.

## Use-case
Unique behavior based on the presence of a state.

## Example

```c++
static constexpr iuic::state::decl state;

iuic::units::uid uid = ...;

if(b.state.has(state) || b.state.has(uid,state)) {
  ...
}

```

## Links
- [uid](./builder.uid.md)
