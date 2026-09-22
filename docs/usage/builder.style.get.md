# get()

## Description
Get style.

## Behavior
- **Returns** : `iuic::style::value` by sid. 

## Use-case
If needs access to style.

## Example

```c++

iuic::style::sid sid = ...;

iuic::style::value style = b.style.get(sid);

```
