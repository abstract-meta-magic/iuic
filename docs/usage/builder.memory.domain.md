# domain

## Purpose 
Stable memory tied to a context.

## Behavior
- **Contained type** : the type by which access is granted.
- **Native type** : stored type.
- **Uniqueness** : the contained type is used.
- **Registration** : Reserves or gets a `slot` in an open-address map. No memory is allocated at this point.
- **Access** : All access to the stored object goes through the proxy (see below).
- **Polymorphism** :
  - Supports storing descendants of the contained type.
  - Supports downcast from contained type to native type.
  - Does not support casting to intermediate types yet.

## Lifetime
- As long as the context is alive.

## Use-case
- Use for global data within the context.
- Use for dependency injection.

## Proxy
### Purpose 
Provides control operations over a domain object.

### Members
- [has_value](./builder.memory.domain.has_value.md)
- [try_set](./builder.memory.domain.try_set.md)
- [try_emplace](./builder.memory.domain.try_emplace.md)
- [get](./builder.memory.domain.get.md)
- [get_ref](./builder.memory.domain.get_ref.md)
- [get_native_type](./builder.memory.domain.get_native_type.md)
- [get_native](./builder.memory.domain.get_native.md)
- [reset](./builder.memory.domain.reset.md)

## Example

```c++
struct point {
  int x,y;
};

auto domain = b.memory.domain<point>();

```
