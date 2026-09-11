# make_ordered(...)

## Description

Make uid from string by ui-tree traversal order. 

## Behavior
- Determined by tree position and input string.
- **Not stable** across rebuilds if tree structure changes.

## Use-case

If you need just unique uid, but not stable.

## Example
```c++
auto uid = b.uid.make_ordered("button-uid");
```
```c++
auto uid_1 = b.uid.make_ordered("button-uid");
auto uid_2 = b.uid.make_ordered("button-uid");

// uid_1 == uid_2
```
```c++
auto uid_1 = b.uid.make_ordered("button-uid");

b.element.frame(...,[](builedr& b){
  auto uid_2 = b.uid.make_ordered("button-uid");
});

// uid_1 != uid_2
```
