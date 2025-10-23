# element
Используется для создания элементов(согласно [[element]]).
## Примеры
```c++
ctx.make([](context::builder &b){
  // обычный элемент
  b.element.frame([](context::builder){
    // внутренние устройство
  });

  // текстовый элемент
  b.element.text(text::token{"ok"});
});
```