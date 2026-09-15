# text(...)

## Description

Create a text element.

## Behavior
- **Endpoint** : In the hierarchy, it is always the final element.
- **Warning** : The **token** must have a stable address until the end of the **make** phase.
- **Note** : The token can be copied as needed.
- **Implementation-defined optimization** : Compiler behavior for inlining lambdas and short functions.

## Use-case
- text representation.

## Example

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

## Links
??
