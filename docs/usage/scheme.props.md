# props

## Purpose
Access to the element's properties.

## Context 
```c++
iuic::scheme::explorer ex;
...
for (auto el : ex.ranges.level_order()) {
  ...
};
```
## Properties
### area
Gets element area.
```c++
const iuic::units::ui::area& area = ex.props.area(el);
```
### order
Gets element order.
```c++
iuic::units::order order = ex.props.order(el);
```
### style
Gets element style.
```c++
iuic::style::value style = ex.props.style(el);
```
### policy
Gets element policy.
```c++
my_policy policy = ex.props.policy<my_policy>(el);
```
### uid
Gets element uid.
```c++
iuic::units::uid uid = ex.props.uid(el);
```
### clipzone
Gets element clipzone.
```c++
iuic::units::clipzone clip = ex.props.clipzone(el);
```
### text
Gets element text.
```c++
std::span<const iuic::text::present::token> tokens = ex.props.text(el);
```

## Links
- [ranges](./scheme.ranges.md)
