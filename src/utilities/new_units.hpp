#pragma once

template<typename T>
struct prefix_holder {
  using value_type = T;
  T operator "" _M (long double value) {
    return value * 1'000'000;
  }
  T operator "" _K (long double value) {
    return value * 1'000;
  }
};

tempalte<typename T>
struct unit_holder {
  using value_type = T;

  T value() const { return m_value; }
private:
  T m_value;
};
