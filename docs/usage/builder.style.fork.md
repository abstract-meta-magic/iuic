# fork(...)

## Description
Use to create fork by sid.

## Behavior
- create new sid and link with old sid properties.

## Use-case
If needs branches in styles.

## Example

```c++
iuic::style::sid sid = ...;

iuic::style::sid new_sid = b.style.fork(sid);

```
