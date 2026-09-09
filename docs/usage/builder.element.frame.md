# frame(...)


Make frame element.


```c++

iuic::layout::frame layout;
iuic::style::sid sid;
iuic::units::uid uid;

b.element.frame(sid,layout);

b.element.frame(uid,sid,layout);

b.element.frame(uid,sid,layout,[](iuic::scheme::builder&){ ... });

```

## Links
- [uid](./builder.uid.md)
- [sid](./builder.style.md)
