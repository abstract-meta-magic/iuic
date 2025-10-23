# context - это
Основной класс для работы с библиотекой.
## настройка 
## set_view_size
Используется для установки размера [[#Корневой элемент|корневого элемента]]
```c++
context ctx;

ctx.set_view_size(ui_size{800,600});
```
## примеры
```c++

import iuic.core;

using namespace iuic;

int main() {
  context ctx;
  
  ctx.set_view_size(ui_size{800,600});
  
  ctx.make([](context::builder &b) {
    // построение схемы
  });
}
```
-  [[ui_scheme|схема]]
- [[builder]]

# Корневой элемент