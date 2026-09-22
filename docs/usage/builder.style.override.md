# [try_-]override(...)

## Description
Use for override style by sid.

## Behavior
- Gets access by sid to override style property

## Use-case
If needs dynamic changes in style.

## Example

```c++

iuic::style::sid sid = ...;

b.style.override(sid,[](iuic::style::shape&){ ... });
b.style.override(sid,[](iuic::style::decoration&){ ... });
b.style.override(sid,[](iuic::style::transform&){ ... });

b.style.try_override(sid,[](iuic::style::shape&){ ... });
b.style.try_override(sid,[](iuic::style::decoration&){ ... });
b.style.try_override(sid,[](iuic::style::transform&){ ... });

```

More info.
