// Copyright 2024 ECCI-UCR CC-BY-4
#pragma once

#include <stdexcept>
#include <mpi.h>

class Mpi {

    protected:
        int process_number = -1;
        int process_count = -1;
        std::string hostname;
        int process_hostname_length = -1;
    
    public:
        class Error : public std::runtime_error {
            public:
                Error(const std::string& message) : std::runtime_error(message) {}
                Error(const std::string& message, const Mpi& mpi) : std::runtime_error(
                    message + " on process " + std::to_string(mpi.getRank()) + " of " 
                        + std::to_string(mpi.getProcessCount()) + " on " + mpi.getHostname() + ":" + message
                ) {}
                Error(const std::string& message, const Mpi& mpi, const int threadNumber) : std::runtime_error(
                    message + " on process " + std::to_string(mpi.getRank()) + " of "
                        + std::to_string(mpi.getProcessCount()) + " on " + mpi.getHostname() + " thread "
                        + std::to_string(threadNumber) + ":" + message
                ) {}

        };
    
    public:
        Mpi(int& argc, char**& argv) {
            if (MPI_Init(&argc, &argv) != MPI_SUCCESS) {
                throw std::runtime_error("Error initializing MPI");
            }

            char process_hostname[MPI_MAX_PROCESSOR_NAME] = { '\0' };
            
            if (MPI_Comm_rank(MPI_COMM_WORLD, &this->process_number) != MPI_SUCCESS) {
                throw Error("Error getting process number");
            }
            if (MPI_Comm_size(MPI_COMM_WORLD, &this->process_count) != MPI_SUCCESS) {
                throw Error("Error getting process count");
            }
            if (MPI_Get_processor_name(process_hostname, &this->process_hostname_length) != MPI_SUCCESS) {
                throw Error("Error getting hostname");
            }
            this->hostname = process_hostname;
        }

        ~Mpi() {
            MPI_Finalize();
        }

        inline int getProcessNumber() const {
            return this->process_number;
        }

        inline int getRank() const {
            return this->getProcessNumber();
        }

        inline int getProcessCount() const {
            return this->process_count;
        }

        inline int getSize() const {
            return this->getProcessCount();
        }

        inline const std::string& getHostname() const {
            return this->hostname;
        }

};
