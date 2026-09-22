# emit(...)

## Description
Emits an event on the given channel.

## Behavior
- **Protocol** : Behavior is determined by the **event** part of the **protocol** (see Links below).

## Use-case
- A reaction to the event is needed.
- Someone needs to respond to the event.

## Example

```c++

// see Links below 
namespace proto {
constexpr iuic::event::channel local;
struct key { ... };
}

b.event.emit<proto::local>([](proto::key){ ... });

```

## Links
- [event](./../concept/event.md)
- [protocol](./../concept/protocol.md)
