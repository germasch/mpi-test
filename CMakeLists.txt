
cmake_minimum_required(VERSION 3.16)

project(MPITest)

set(MPI_CXX_SKIP_MPICXX ON)
find_package(MPI REQUIRED)

add_executable(mpi-test mpi-test.cxx)
target_link_libraries(mpi-test PRIVATE MPI::MPI_CXX)
