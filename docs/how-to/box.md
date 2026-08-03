Requires:
- [frame-layout]()
- [style]()


```c++
import iuic.core;

using builder = iuic::scheme::builder;

void box(builder &b) {
  static constexpr box_layout layout{...};
  static constexpr iuic::style::decl style{...};
  
  
  b.element.frame(b.style.make(style),layout);
}

```
