/// @copyright 2020 ECCI, Universidad de Costa Rica. All rights reserved
/// This code is released under the GNU Public License version 3
/// @author Jeisson Hidalgo-Céspedes <jeisson.hidalgo@ucr.ac.cr>

#include "ProducerTest.hpp"
#include "Log.hpp"
#include "Util.hpp"

ProducerTest::ProducerTest(const size_t whoAmI, const size_t& packageCount, const int& productorDelay, const size_t& producerCount,
  const size_t& consumerCount, size_t& createdPackages, std::mutex& canAccessCreatedPackages)
  :whoAmI(whoAmI), packageCount(packageCount), productorDelay(productorDelay), producerCount(producerCount), consumerCount(consumerCount), createdPackages(createdPackages), canAccessCreatedPackages(canAccessCreatedPackages) {
}

int ProducerTest::run() {
  // Produce each asked message
  size_t currentPackage = 0;
  while (true) {
    // Protect the access to the shared counter of created packages
    this->canAccessCreatedPackages.lock();
    // Check if we have produced all the messages
    size_t currentPackage = ++this->createdPackages;
    // Release the access to the shared counter of created packages
    this->canAccessCreatedPackages.unlock();
    if (currentPackage > this->packageCount) {
      break;
    }
    // Create a message
    NetworkMessage message = this->createMessage(currentPackage);
    // Push the message to the queue
    this->produce(message);
    ++this->myPackageCount;
    // Increment the counter of created packages
  }
  
  if (currentPackage == this->packageCount + this->producerCount) {
    this->produce(NetworkMessage());
  }
  
  // Report production is done
  Log::append(Log::INFO, "Producer", std::to_string(this->myPackageCount)
    + " menssages sent");
  return EXIT_SUCCESS;
}

NetworkMessage ProducerTest::createMessage(size_t index) const {
  // Source is always 1: this producer
  const uint16_t source = this->whoAmI + 1;
  // Target is selected by random
  const uint16_t target = 1 + Util::random(0
    , static_cast<int>(this->consumerCount));
  // IMPORTANT: This simulation uses sleep() to mimics the process of
  // producing a message. However, you must NEVER use sleep() for real projects
  Util::sleepFor(this->productorDelay);
  // Create and return a copy of the network message
  return NetworkMessage(target, source, index);
}
