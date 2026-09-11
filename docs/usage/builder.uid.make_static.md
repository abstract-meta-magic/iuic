# make_static(...)

## Description

Make static uid from string.

## Use-case

Used for static elements.

## Behavior
- Use string hash as uid.
- **Stable**

## Example

```c++
auto uid = b.uid.make_static("ui-button");
```
```c++
auto uid_1 = b.uid.make_static("ui-button");
auto uid_2 = b.uid.make_static("ui-button");

// uid_1 == uid_2

```
