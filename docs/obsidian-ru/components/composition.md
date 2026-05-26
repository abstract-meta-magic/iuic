Вместо наследования была выбрана модель *композиций*. В которой *элементы* состоят из вложенных *элеметов*.

```c++

using builder = ...;

void my_component(builder& b) {
  static constexpr auto layout = ...;
  auto uid = ...;
  auto sid = ...;
  

  b.element.frame(uid,sid,layout,[](builder&b){
	auto sid = ...;
	
    b.element.frame(sid,layout);
  });
}

```