# self()

## Description

Returns current self style.

## Behavior
- Get sid from the current builder state.

## Use-case

Use when you don't have direct access to element-local context. 

## Example

```c++
void my_element_extension(builder& b) {
  auto sid = b.style.self();
}

```
