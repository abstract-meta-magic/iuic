# make_indexed(...)

## Description

Make static uid from number and anchor.


## Behavior
- Determinate by index and anchor.
- **Stable**: as long as the index and anchor remain unchanged.
- **Invalidation risk** : if two element swap indexs. 

## Use-case

Used for indexed elements.

## Example

```c++
static constexpr anchor anchor;

auto uid = b.uid.make_indexed(0,anchor);
```
```c++
static constexpr anchor anchor;

auto uid_1 = b.uid.make_indexed(0,anchor);
auto uid_2 = b.uid.make_indexed(0,anchor);

// uid_1 == uid_2
```
```c++
static constexpr anchor anchor;

auto uid_1 = b.uid.make_indexed(0,anchor);
auto uid_2 = b.uid.make_indexed(1,anchor);

// uid_1 != uid_2
```
```c++
auto uid_1 = b.uid.make_indexed(0,{}); // const & bind on stack
auto uid_2 = b.uid.make_indexed(1,{}); // const & bind on stack

// uid_1 != uid_2
```
