
## Ответственность 
// Прописать конкретнее
Используется для задания политик элемента.
## Пример
```c++
ctx.make([](context::builder &b) {
  b.element.frame([](context::builder &b){
    b.policy.hovered(policy::hovered::propagate);
  });
});
```
## Политики
### Hovered
- propagate 
- block
- none
### Event
- propagate
- block