# state
Используется для задания и проверки состояний по [[uid]].

```c++
pseudo_state state = ...;

if(b.state.pseudo(uid) == state);

if(b.state.hovered(uid)) { ... }
```

## state transition
Обработка перехода к состоянию.

```c++

uid_t uid = ...;
pseudo_state idel = ...;
pseudo_state custom = ...;

b.state.transition(uid,idle,custom,[](transition_utils u){
  // что-то происходит
});

```