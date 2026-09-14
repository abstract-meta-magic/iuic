# make(...)

## Description
Use for define/get style by **sid**;

## Behavior
- if style is not **maked**, copy style property
- if style already **maked**, just return sid

## Use-case
Create new style.

## Example

```c++
iuic::style::decl style;

auto sid = b.style.make(style);
auto sid = b.style.make(&style);

```
