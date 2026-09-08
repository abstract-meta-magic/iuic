# [try_-]override(...)

Use fore override style by sid.


```c++

iuic::style::sid sid = ...;

b.style.override(sid,[](iuic::style::shape&){ ... });
b.style.override(sid,[](iuic::style::decoration&){ ... });
b.style.override(sid,[](iuic::style::transform&){ ... });

b.style.try_override(sid,[](iuic::style::shape&){ ... });
b.style.try_override(sid,[](iuic::style::decoration&){ ... });
b.style.try_override(sid,[](iuic::style::transform&){ ... });

```

More info.
