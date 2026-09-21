# tmp(...)

## Purpose 
Temporary memory.

## Behavior
- **Allocation** : Linear allocation.
- **Overflow** : throws `std::bad_alloc` when out of memory.
- **Size IS** : Fixed. By default is 4MB. Configurable.
- **Overloads** : copy, move, emplace. 

## Lifetime
- 1 tick

## Use-case
Use this when you need a temporary object whose address will be valid until the end of the current tick.

## Example

```c++

struct point {
  int x,y;
};

point p1{4,8};

point& p2 = b.memory.tmp(p1);         // copy 
point& p3 = b.memory.tmp(point{4,8}); // move
point& p4 = b.memory.tmp<point>(4,8); // pl new
point& p5 = b.memory.tmp<point>();    // pl new

```
