
# event
Используется для прикрепления реакций на события к элементам.
## Ограничения
- событием может быть указателем на функцию или лямбду без захвата, которые принимают [[объект события]].
- захват объектов происходит через [[storage#ключ|ключи]]. Только один общий объект, и только один текстовый буфер.
## Пример
```c++
ctx.make([](context::builder &b){

  b.element.frame([](context::builder b&){
    // требуеться для локальных лобытий
    b.policy.hovered(policy::hovered::propagate);
	
    b.event([](event::local::key e){
	  // что-то происходит
    });
  });
});
```

>[!WARNING]
> События зависят от [[builder-policy|политик]] **hovered** и **event**.
## Локальные
Срабатывают только если элемент имеет статус **hovered**.
```c++
b.event([](event::local::key e) { ... });
```
## Глобальные
Срабатывают *независимо* от статуса **hovered**.
```c++
b.event([](event::global::key e) { ... });
```
# Захват объекта
Возможен только через получение ключей и передачи их в **события**.
```c++
object_registry_key ork = ...; // он же ork_t
text_registry_key trk = ...;   // он же trk_t 

b.event([](event::*::key e){ ... },trk);
b.event([](event::*::key e){ ... },ork);
b.event([](event::*::key e){ ... },ork,trk);
```
# event_utils
- event-storage
- event-state