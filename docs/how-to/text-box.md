```c++

using builder = iuic::scheme::builder;

void text_box(builder &b) {
  static constexpr tex_box_layout layout{...};
  static constexpr iuic::style::decl style{...};

  static iuic::text::raw::token token{...};
  
  b.element.text(token,b.style.make(style),layout);
}

// or

void text_box(builder &b) {
  static constexpr tex_box_layout layout{...};
  static constexpr iuic::style::decl style{...};

  auto token = b.memory.tmp(iuic::text::raw::token token{...});
  
  b.element.text(token,b.style.make(style),layout);
}


```
