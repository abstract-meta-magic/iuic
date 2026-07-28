namespace {
struct dummy_layout : iuic::layout::frame {
  std::optional<iuic::units::ui::adaptive::size>
  measure(iuic::layout::measure::frame_utils utils) const noexcept override {
    return iuic::units::ui::adaptive::size{iuic::units::upixel{0},
                                           iuic::units::upixel{0}};
  };

  bool
  arrange(iuic::layout::arrange::frame_utils utils) const noexcept override {
    auto chs = utils.childs();

    for (auto ch : chs) {
      utils.apply_element(ch, {});
    }

    return true;
  };
};

} // namespace
