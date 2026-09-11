# make_stable(...)

## Description

Make stable uid from string by ui-tree root path and local order.

## Behavior
- Determined by root-to-element path,local order and string.
- **Stable** : guaranteed by design.
- **Collision-resistant**: relies on the uniqueness of the full path + string combination.
- **Trade-off**: higher computational cost due to path traversal and hashing.

## Use-case

If you need stable uid.

## Example

 ```c++
 auto uid = b.uid.make_stable("button-uid");
 ```
 ```c++
 auto uid_1 = b.uid.make_stable("button-uid");
 auto uid_2 = b.uid.make_stable("button-uid");
 
 // uid_1 == uid_2
 ```
 ```c++
 auto uid_1 = b.uid.make_stable("button-uid");
 
 b.element.frame(...,[](builder& ){
   auto uid_2 = b.uid.make_stable("button-uid");
 });
 
// uid_1 != uid_2
 ```
