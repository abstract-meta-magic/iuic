# self()

## Description

Use to get self uid.

## Behavior
- Get uid from the current builder state.

## Use-case

Use when you don't have direct access to element-local context. 

## Example
```c++
void my_element_extension(builder& b) {
  auto uid = b.uid.self();
  ...
};
```
