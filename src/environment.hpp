#include "object.hpp"
#include <memory>
#include <type_traits>
#include <vector>

class Environment
{
public:
  std::vector<std::unique_ptr<Object>> objects;
  template <typename T,
            typename = std::enable_if_t<std::is_base_of_v<Object, T>>>
  void addObject(T obj)
  {
    objects.push_back(std::make_unique<T>(obj));
  }
};

typedef std::vector<std::unique_ptr<Object>> ObjectList;
