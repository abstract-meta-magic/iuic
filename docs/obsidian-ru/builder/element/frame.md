Используется для создания *рамки*,или в привычном понимании *коробки*.
```c++
builder.element.frame(uid,sid,layout);
```
Можно опустить [[uid]]. В таком случае будет использован [[uid]] *родительского элемента*.
```
builder.element.frame(sid,layout);
```
Может иметь вложенные *элементы*.
```c++
builedr.element.frame(uid,sid,layout,[](builder& inner_builder) {
   inner_builder.elment.frame(...);
})
```
- [[uid]]
- [[style/sid|sid]]
- [[layout/frame|layout]]
- [[components/composition|inner]]