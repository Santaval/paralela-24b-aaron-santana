// Copyright 2024 ECCI-UCR CC-BY-4
#pragma once

#include <mpi.h>
#include <stdexcept>
#include <string>
#include <vector>

class Mpi {
 public:
  static const int DEFAULT_CAPACITY = 1024;

 protected:
  int processNumber = -1;
  int processCount = -1;
  std::string hostname;

 public:
  class Error : public std::runtime_error {
   public:
    explicit Error(const std::string& message)
      : std::runtime_error(message) {
    }
    Error(const std::string& message, const Mpi& mpi)
      : std::runtime_error(mpi.getHostname() + ':' +
        std::to_string(mpi.getProcessNumber()) + ':' + message) {
    }
    Error(const std::string& message, const Mpi& mpi, const int threadNumber)
      : std::runtime_error(mpi.getHostname() + ':' +
        std::to_string(mpi.getProcessNumber()) + '.' +
        std::to_string(threadNumber) + ':' + message) {
    }
  };

 public:
  Mpi(int& argc, char**& argv) {
    if (MPI_Init(&argc, &argv) == MPI_SUCCESS) {
      if (MPI_Comm_rank(MPI_COMM_WORLD, &this->processNumber) != MPI_SUCCESS) {
        throw Error("could not get MPI rank");
      }
      if (MPI_Comm_size(MPI_COMM_WORLD, &this->processCount) != MPI_SUCCESS) {
        throw Error("could not get MPI size");
      }
      char buffer[MPI_MAX_PROCESSOR_NAME] = { '\0' };
      int hostname_len = -1;
      if (MPI_Get_processor_name(buffer, &hostname_len) != MPI_SUCCESS) {
        throw Error("could not get MPI hostname");
      }
      this->hostname = buffer;
    } else {
      throw Error("could not init MPI");
    }
  }
  ~Mpi() {
    MPI_Finalize();
  }
  inline int getProcessNumber() const {
    return this->processNumber;
  }
  inline int rank() const {
    return this->getProcessNumber();
  }
  inline int getProcessCount() const {
    return this->processCount;
  }
  inline int size() const {
    return this->getProcessCount();
  }
  inline const std::string& getHostname() const {
    return this->hostname;
  }

 public:
  static inline MPI_Datatype map(bool) { return MPI_C_BOOL; }
  static inline MPI_Datatype map(char) { return MPI_CHAR; }
  static inline MPI_Datatype map(unsigned char) { return MPI_UNSIGNED_CHAR; }
  static inline MPI_Datatype map(short) { return MPI_SHORT; }
  static inline MPI_Datatype map(unsigned short) { return MPI_UNSIGNED_SHORT; }
  static inline MPI_Datatype map(int) { return MPI_INT; }
  static inline MPI_Datatype map(unsigned) { return MPI_UNSIGNED; }
  static inline MPI_Datatype map(long) { return MPI_LONG; }
  static inline MPI_Datatype map(unsigned long) { return MPI_UNSIGNED_LONG; }
  static inline MPI_Datatype map(long long) { return MPI_LONG_LONG; }
  static inline MPI_Datatype map(unsigned long long) {
    return MPI_UNSIGNED_LONG_LONG;
  }
  static inline MPI_Datatype map(float) { return MPI_FLOAT; }
  static inline MPI_Datatype map(double) { return MPI_DOUBLE; }
  static inline MPI_Datatype map(long double) { return MPI_LONG_DOUBLE; }

 public:  // Send
  /// Send a scalar value to another process
  template <typename Type>
  void send(const Type& value, const int toProcess, const int tag = 0) {
    if (MPI_Send(&value, /*count*/ 1, Mpi::map(value), toProcess, tag,
        MPI_COMM_WORLD) != MPI_SUCCESS) {
      throw Mpi::Error("could not send value", *this);
    }
  }
  /// Send an array of count values to another process
  template <typename Type>
  void send(const Type* values, const int count, const int toProcess,
      const int tag = 0) {
    if (MPI_Send(values, count, Mpi::map(Type()), toProcess, tag,
        MPI_COMM_WORLD) != MPI_SUCCESS) {
      throw Mpi::Error("could not send array", *this);
    }
  }
  /// Send an array of values to another process
  template <typename Type>
  void send(const std::vector<Type>& values, const int toProcess,
      const int tag = 0) {
    if (MPI_Send(values.data(), values.size(), Mpi::map(Type()), toProcess, tag,
        MPI_COMM_WORLD) != MPI_SUCCESS) {
      throw Mpi::Error("could not send vector", *this);
    }
  }
  /// Send a text to another process
  void send(const std::string& text, const int toProcess, const int tag = 0) {
    if (MPI_Send(text.data(), text.length() + 1, MPI_CHAR, toProcess, tag,
        MPI_COMM_WORLD) != MPI_SUCCESS) {
      throw Mpi::Error("could not send string", *this);
    }
  }

 public:  // Receive
  /// Wait until it receives a scalar value from other process
  template <typename Type>
  int receive(Type& value, const int fromProcess,
      const int tag = MPI_ANY_TAG, int* source = nullptr) {
    return this->receive(&value, 1, fromProcess, tag, source);
  }
  /// Wait until it receives at most capacity values from another process
  /// Return effective count of elements read
  template <typename Type>
  int receive(Type* values, const int capacity, const int fromProcess,
      const int tag = MPI_ANY_TAG, int* source = nullptr) {
    MPI_Status status;
    if (MPI_Recv(values, capacity, Mpi::map(Type()), fromProcess, tag,
        MPI_COMM_WORLD, &status) != MPI_SUCCESS) {
      throw Mpi::Error("could not receive array", *this);
    }
    if (source) {
      *source = status.MPI_SOURCE;
      // if (MPI_Status_get_source(&status, source) != MPI_SUCCESS) {
      //   throw Mpi::Error("could not get count", *this);
      // }
    }
    int count = -1;
    if (MPI_Get_count(&status, Mpi::map(Type()), &count) != MPI_SUCCESS) {
      throw Mpi::Error("could not get count from status", *this);
    }
    return count;
  }
  /// Wait until it receives at most capacity values from another process
  template <typename Type>
  int receive(std::vector<Type>& values, const int capacity,
      const int fromProcess, const int tag = MPI_ANY_TAG,
      int* source = nullptr) {
    values.resize(capacity);
    const int count = this->receive(values.data(), capacity, fromProcess, tag,
        source);
    values.resize(count);
    return count;
  }
  /// Wait until it receives a text of at most length chars from another process
  int receive(std::string& text, const int fromProcess,
      const int capacity = DEFAULT_CAPACITY, const int tag = MPI_ANY_TAG,
      int* source = nullptr) {
    std::vector<char> buffer(capacity, '\0');
    this->receive(buffer, capacity, fromProcess, tag, source);
    text = buffer.data();
    return text.length();
  }
};
