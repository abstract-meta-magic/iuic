# iuic::context
Composite class for UI state.

- [make]
- [domain]
- [scheme]



```c++


void make_ui(iuic::scheme::builder&);
void eval_scheme(iuic::scheme::explorer&);

int main() {
  iuic::context ctx;
  
  
  ctx.make(make_ui);
  
  eval_scheme(ctx.scheme);

  return 0;
}

```
