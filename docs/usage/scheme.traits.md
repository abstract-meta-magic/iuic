# props

## Purpose
Access to the element's traits.

## Context
```c++
iuic::scheme::explorer ex;
...
for (auto el : ex.ranges.level_order()) {
  ...
};
```
## Traits
### has\_text
Element has **text**?
```c++
if (ex.traits.has_text(el)) {
  ...
}
```
### is\_virtualized
Is the element **virtualized**?
```c++
if (ex.traits.is_virtualized(el)) {
  ...
}
```
### is_discarded 
Is the element **discarded**?
```c++
if (ex.traits.is_discarded(el)) {
  ...
}
```

## Links
