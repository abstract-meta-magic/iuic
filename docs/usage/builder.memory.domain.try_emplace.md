# try_emplace(...)

## Description
Constructs object if does not exist.

## Behavior
- **Allocation** : uses global `operator new`.
- **Existing object** : no-op if the slot is already occupied.
- **Return** : true if object construct.

## Example

```c++

struct IService {
...
};

auto proxy = b.memory.domain<IService>();

struct ServiceImpl : IService {
...
};

proxy.try_emplace<ServiceImpl>();

```
