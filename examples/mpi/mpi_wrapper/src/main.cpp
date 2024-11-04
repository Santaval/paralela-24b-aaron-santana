// Copyright 2024 ECCI-UCR CC-BY-4
/**
Test Mpi wrapper

Process 0 read from args an action, and data from stdin. Then, send data to the
other process through the given action: broadcast, reduce, or others Mpi methods
*/

#include <cassert>
#include <cstring>
#include <iostream>
#include <new>
#include <sstream>

#include "Mpi.hpp"

const int BUFFER_SIZE = Mpi::DEFAULT_CAPACITY;

template <typename Type>
std::ostream& operator<<(std::ostream& out, const std::vector<Type>& values) {
  for (size_t index = 0; index < values.size(); ++index) {
    out << (index ? " " : "") << values[index];
  }
  return out;
}

template <typename Type>
void print(const Type& data, Mpi& mpi) {
  std::stringstream out;
  out << mpi.rank() << ": " << data << std::endl;
  std::cout << out.str();
}

template <typename Type>
void print(const Type* array, const int count, Mpi& mpi) {
  std::stringstream out;
  out << mpi.rank() << ':';
  for (int index = 0; index < count; ++index) {
    out << ' ' << array[index];
  }
  out << std::endl;
  std::cout << out.str();
}

template <typename Type>
int send_value(Mpi& mpi) {
  Type value = Type();
  if (mpi.rank() == 0) {
    std::cin >> value;
    for (int target = 1; target < mpi.size(); ++target) {
      mpi.send(value, target);
    }
  } else {
    mpi.receive(value, 0);
  }
  print(value, mpi);
  return 0;
}

template <typename Type>
int send_array(Mpi& mpi) {
  Type* array = new Type[BUFFER_SIZE]();
  int count = 0;
  if (mpi.rank() == 0) {
    while (std::cin >> array[count]) {
      ++count;
      assert(count <= BUFFER_SIZE);
    }
    for (int target = 1; target < mpi.size(); ++target) {
      mpi.send(array, count, target, 0);
    }
  } else {
    count = mpi.receive(array, BUFFER_SIZE, 0, 0);
  }
  print(array, count, mpi);
  delete [] array;
  return 0;
}

int send_text(Mpi& mpi) {
  char* text = new char[BUFFER_SIZE]();
  if (mpi.rank() == 0) {
    std::cin >> text;
    for (int target = 1; target < mpi.size(); ++target) {
      mpi.send(text, std::strlen(text) + 1, target, 0);
    }
  } else {
    mpi.receive(text, BUFFER_SIZE, 0, 0);
  }
  print(text, mpi);
  delete [] text;
  return 0;
}

template <typename Type>
int send_vector(Mpi& mpi) {
  std::vector<Type> values;
  if (mpi.rank() == 0) {
    Type value = Type();
    while (std::cin >> value) {
      values.push_back(value);
    }
    for (int target = 1; target < mpi.size(); ++target) {
      mpi.send(values, target);
    }
  } else {
    mpi.receive(values, BUFFER_SIZE, 0, 0);
  }
  print(values, mpi);
  return 0;
}

int send(const std::string& type, Mpi& mpi) {
  if (type == "1int") return send_value<int>(mpi);
  if (type == "1str") return send_value<std::string>(mpi);
  if (type == "1txt") return send_text(mpi);
  if (type == "aint") return send_array<int>(mpi);
  if (type == "vint") return send_vector<int>(mpi);
  throw std::runtime_error("unknown data type");
}

int main(int argc, char* argv[]) {
  try {
    Mpi mpi(argc, argv);
    if (argc == 3) {
      std::string action = argv[1], type = argv[2];
      if (action == "send") return send(type, mpi);
      // if (action == "bcst") return broadcast<Type>(type, mpi);
      throw std::runtime_error("unknown action");
    } else {
      std::cerr <<
          "usage: test {send|bcst} {1int|1txt|1str|aint|vint} < data.txt\n";
    }
  } catch (const std::exception& error) {
    std::cerr << "error: " << error.what() << std::endl;
  }
}

/*
bcst 1int 105
bcst 1txt present!
bcst 1str present!
bcst aint 84 -80 +88
bcst vint 5 3 1 -2 -4
*/
