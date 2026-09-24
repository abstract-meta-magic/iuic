# ranges

## Purpose
Provides methods for iterating through the tree of user interface elements.

## Context
```c++
iuic::scheme::explorer ex;
```
## Methods
### level\_order
- Iterates elements in level order (breadth-first, root to leaves).
- It does not skip off virtualized and discarded elements.
```c++
for(auto el : ex.ranges.level_order()) {
  ...
}
```
### clipzones
- Random access range for clipzones.
```c++
auto clipzones = ex.ranges.clipzones(); 

auto & clip = clipzones[4];

for(auto&& clip : clipzones) {
  ...
}
```
