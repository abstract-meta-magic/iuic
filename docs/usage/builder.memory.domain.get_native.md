# get_native()

## Description
Gets accessor to the storaged value.

## Behavior
- **Null** : can return null if object does not exist.
- **Return** : `iuic::erasure::visited::as_mutable`.

## Example

```c++
struct IService { ... };
struct ServiceImpl : IService { ... };

auto proxy = ...;

proxy.get_native().try_visit([](ServiceImpl& s){ ... });

```
