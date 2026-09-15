# frame(...)

## Description
Create a frame element.

## Behavior
- **Recursive construction** : processes nested children within the builder's scope.
- **Warning** : deep nesting may lead to **stack overflow**.
- **Implementation-defined optimization** : compiler behavior for inlining lambdas and short functions.

## Use-case
- grouping of elements.
- visual representation.

## Example

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
