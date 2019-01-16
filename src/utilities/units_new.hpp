
#include <limits>
#include <string>

template<typename T>
T checked_add(T a, T b, T min_val = std::numeric_limits<T>::min, T max_val = std::numeric_limits<T>::max)
{
  if ((a > 0 && b > max_val - a) || (a < 0 && b < min_val - a))
  {
    throw std::runtime_error(std::to_string(a) + " + " + std::to_string(b) + " overflowed");
  }
  return a + b;
}

class unit {
public:
  std::string name();
  std::string suffix_name();

  static unit parse(std::string const& input);

  static unit delete_me()
  {
    data_t d;
    d.meter = 4;
    unit u(d);
    return u;
  }

  bool valid_add(unit const& other)
  {
    return data == other.data;
  }

  unit multiply_result(unit const& other)
  {
    return unit(data + other.data);
  }

private:
  struct data_t
  {
    data_t()
    {
      // default member initialization is only in c++20 for bitfields
      meter = 0;
      kilogram = 0;
      second = 0;
      ampere = 0;
      kelvin = 0;
      mole = 0;
      candela = 0;
    }

    int meter: 4;
    int kilogram: 4;
    int second: 4;
    int ampere: 4;
    int kelvin: 4;
    int mole: 4;
    int candela: 4;

    data_t operator+(data_t const& other)
    {
      data_t ret;
      #define ASSIGN(type) ret.type = checked_add(type, other.type, -8, 7)
      ASSIGN(meter);
      ASSIGN(kilogram);
      ASSIGN(second);
      ASSIGN(ampere);
      ASSIGN(kelvin);
      ASSIGN(mole);
      ASSIGN(candela);
      #undef ASSIGN
      return ret;
    }

    bool operator==(data_t const& other)
    {
      return false; // todo
    }

  };

  unit(data_t new_data) : data(new_data) {}
  data_t data;
};
static_assert(sizeof(unit) == 4, "Unit is too big!");
