# state
## Ответственность 
// Прописать конкретнее
Используется для присвоения и проверки состояний по [[uid]].

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

b.state.transition(uid,idle,custom,[] -> transition_state (transition_utils u){
  // что-то происходит
});

```

>[!Error] Критическое UB.
>функция передаваемая в *transition* **обязана** являться **корутиной** согласно *std-cxx-20*, иначе это *UB*.  

>[!Error] Прерывание через смену состояния.
>Если во время выполнения **корутины**, до ее полного завершения завершения *(через co_return)*, произойдет смена состояния, то выполнение **корутины** прервется без какого-либо уведомления.
### transition-yield
Обычная приостановка с частичным уведомлением **scheduler**.
Возможные значения:
```c++
co_yield ok;
co_yield err;
co_yield yield;
co_yield null;
```
### transition-await
Принимает специальный **awaitor**.
>[!Warning] В разработке
### transition-return
Является новым или **нулевым** состоянием.
```c++

pseudo_state state = ...;

co_return state; 

// или

co_return pseudo_state::null();
```
В случае возврата **нулевого** состояния, происходит завершение **корутины**, иначе переход к новому состоянию.