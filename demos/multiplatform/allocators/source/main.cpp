#include <algorithm>

#include "utility/containers/vector.hpp"
#include "utility/containers/string.hpp"
#include "utility/containers/list.hpp"
#include "utility/containers/deque.hpp"
#include "utility/log.hpp"

int main()
{
  sjsu::Vector<int, 10> vec(sjsu::Vector<int, 10>::allocator_type{});
  LOG_INFO("Starting Allocator Demonstration...");

  LOG_INFO("============ std::vector ============ \n");
  // Creating vector that uses a stack based memory arena
  vec.reserve(decltype(vec)::allocator_type::size);
  vec.push_back(1);
  vec.push_back(2);
  vec.push_back(3);
  vec.push_back(4);
  vec.push_back(5);
  vec.push_back(6);
  vec.push_back(7);
  vec.push_back(8);
  vec.push_back(9);
  vec.push_back(10);
  // Uncomment this line below and watch as the program detects you going over
  // the limit, and prints a stack trace.
  //
  // vec.push_back(11);

  LOG_INFO("List of elements in vector:");

  for (auto element : vec)
  {
    printf("%d, ", element);
  }
  puts("");

  LOG_INFO("============ std::basic_string ============ \n");

  sjsu::String<128> str(sjsu::String<128>::allocator_type{});
  str.reserve(decltype(str)::allocator_type::size - 1);

  str = "hello, world";
  LOG_INFO("str = %.128s", str.c_str());
  str.append(". Appending more text to this string!");
  LOG_INFO("str = %.128s", str.c_str());
  str.append(" More more more text!!!");
  LOG_INFO("str = %.128s", str.c_str());
  LOG_INFO("Inserting 5x'-' at the start of the string");
  str.insert(0, 5, '-');

  LOG_INFO("str = %.128s", str.c_str());

  str += "...testing...";
  LOG_INFO("str = %.128s", str.c_str());
  str = "...testing...";
  LOG_INFO("str = %.128s", str.c_str());

  LOG_INFO("============ std::deque ============ \n");

  // Create a deque containing integers
  sjsu::Deque<int, 8> deque({ 7, 5, 16, 8 },
                            sjsu::Deque<int, 8>::allocator_type{});

  // Add an integer to the beginning and end of the deque
  deque.push_front(13);
  deque.push_back(25);
  deque.push_front(13);
  deque.push_back(25);

  LOG_INFO("List of elements in deque:");
  for (int element : deque)
  {
    printf("%d, ", element);
  }
  puts("");

  LOG_INFO("============ std::list ============ \n");

  sjsu::List<int, 11> list({ 7, 5, 16, 8 },
                           sjsu::List<int, 11>::allocator_type{});

  // Add an integer to the front of the list
  list.push_front(25);
  // Add an integer to the back of the list
  list.push_back(13);
  // Insert an integer before 16 by searching
  auto it = std::find(list.begin(), list.end(), 16);
  if (it != list.end())
  {
    list.insert(it, 42);
  }

  list.push_back(55);

  LOG_INFO("List of elements in list:");
  for (int element : list)
  {
    printf("%d, ", element);
  }
  puts("");

  return 0;
}
