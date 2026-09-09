# tmp

Frame memory.
## For what

## Requires

## Lifetime

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
