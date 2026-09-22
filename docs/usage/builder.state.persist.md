# persist(...)

## Description
Update lifetime of states.

## Behavior
- **No-op** : if states by uid doesn't exist.

## Use-case
If long‑lived states are required.

## Example

```c++

iuic::units::uid uid = ...;

b.state.persist(uid);

```
