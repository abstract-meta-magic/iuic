# domain

## For what

## Lifetime


```c++
struct point {
  int x,y;
};


auto proxy = b.memory.domain<point>();

if (not proxy.has_value()) {
  proxy.try_set(...);
  proxy.try_emplace(...);
}

auto erasure = proxy.get(); 

erasure.try_visit([](point&){ ... });

point& p   = proxy.get_ref();


```
