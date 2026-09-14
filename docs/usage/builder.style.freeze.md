# freeze(...)

## Description
Prohibition on style override.

## Behavior
- Sets the freeze flag. 
- style::decl by **default** has freeze flag.

## Use-case
If it is necessary to prevent subsequent changes.

## Example

```c++

iuic::style::sid sid = ...;

b.style.freeze(sid);

```
