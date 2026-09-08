# iuic::context
Composite class for UI state.

- [make]
- [eval]
- [domain]


```c++


void make_ui(iuic::scheme::builder&);
void eval_ui(iuic::scheme::explorer&);

int main() {
  iuic::context ctx;
  
  ctx.make(make_ui);
  // or 
  ctx.make([](iuic::scheme::builder& b){ ... });
  
  ctx.eval(eval_ui);
  //
  ctx.eval([](iuic::scheme::explorer &){ ... });
  
  ctx.domain<T>([](T&){ ... });

  return 0;
}

```
