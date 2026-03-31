#include "lists.hh"

namespace isim
{
  void print(std::vector<int> list)
  {
    std::cout << "[ ";
    for (int i : list)
      {
        if (i == list.back())
          std::cout << i << " ";
        else
          std::cout << i << ", ";
      }
    std::cout << "]" << std::endl;
  }

  void print(int* list, size_t size)
  {
    std::cout << "[ ";
    for (size_t i = 0; i < size; i++)
      {
        if (i == size - 1)
          std::cout << list[i] << " ";
        else
          std::cout << list[i] << ", ";
      }
    std::cout << "]" << std::endl;
  }

} // namespace isim