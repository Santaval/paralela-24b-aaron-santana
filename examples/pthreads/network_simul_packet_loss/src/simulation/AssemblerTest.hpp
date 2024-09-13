/// @copyright 2020 ECCI, Universidad de Costa Rica. All rights reserved
/// This code is released under the GNU Public License version 3
/// @author Jeisson Hidalgo-Céspedes <jeisson.hidalgo@ucr.ac.cr>

#ifndef ASSEMBLERTEST_HPP
#define ASSEMBLERTEST_HPP

#include "Assembler.hpp"
#include "NetworkMessage.hpp"

/**
 * @brief A ConsumerTest class example
 */
class AssemblerTest : public Assembler<NetworkMessage, NetworkMessage> {
  DISABLE_COPY(AssemblerTest);

 protected:
  /// Delay of consumer to consume a package, negative for max random
  int packageLossPercent = 0;
  /// Number of messages received
  size_t ConsumerCount = 0;

 public:
  /// Constructor
  explicit AssemblerTest(double packageLossPercent, size_t consumerCount);
  /// Consume the messages in its own execution thread
  int run() override;
  /// Override this method to process any data extracted from the queue
  void consume(NetworkMessage data) override;
};

#endif  // CONSUMERTEST_HPP
