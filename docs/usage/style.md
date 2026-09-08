# iuic::style::delc
Class for stilization elements.
Work in progress.

- [shape]
  - [size]
  - [border]
- [decoration]
- [transform]
- [advence]



```c++

constexpr auto my_style = [](iuic::style::decl style){
  using namespace iuic::units::literals;
  style.shape.size.width = 40_upx;
  style.shape.size.height = 40_upx;
  return style;
}({});


```
