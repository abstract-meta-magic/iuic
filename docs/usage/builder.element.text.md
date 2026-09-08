# text(...)

Make text element.

```c++

iuic::style::sid sid = ...;
iuic::units::uid uid = ...;
iuic::layout::text &layout = ...;
iuic::text::raw::token token = ...;
std::span<const iuic::text::raw::token> tokens = ...;

b.element.text(token,sid,layout);
b.element.text(token,uid,sid,layout);
b.element.text(tokens,sid,layout);
b.element.text(tokens,uid,sid,layout);

```
