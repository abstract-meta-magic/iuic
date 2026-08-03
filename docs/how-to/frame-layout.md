Requires:
- [units]()
- [style]()


## Basic
```c++
import iuic.core;

namespace lib {
// for _px,_upx,_per,etc...
using namespace iuic::units::literals;

struct layout : iuic::layout::frame {

std::optional<iuic::units::ui::adaptive::size> 
measure(iuic::layout::measure::frame_utils utils) override {
  return {40_upx,40_upx};
}

bool arrange(iuic::layout::arrange::frame_utilse utils) override {
  return true;
}
}; 

}

```
## More

```c++
import iuic.core;

namespace lib {

iuic::units::ui::area get_child_area(auto ch,auto utilse);

struct container : iuic::layout::frame {

std::optional<iuic::units::ui::adaptive::size> 
measure(iuic::layout::measure::frame_utils utils) override {
  using namespace iuic::units::literals;
  return {100_per,100_per};
}

bool arrange(iuic::layout::arrange::frame_utilse utils) override {
  for(auto ch : utils.childs()) {
    iuic::units::ui::area area{get_child_area(ch,utils)};
	
	// place 

    utils.apply(ch,area);
  }
  return true;
}
}; 

}

```
