#include <mpi.h>

#include <cassert>
#include <iostream>
#include <math.h>

int main(int argc, char** argv)
{
  int rank, size;
  MPI_Init(&argc, &argv);
  MPI_Comm_size(MPI_COMM_WORLD, &size);
  MPI_Comm_rank(MPI_COMM_WORLD, &rank);

  assert(sqrt(size) == int(sqrt(size))); // make sure the size is an integer^2
  int dimen[2] = {int(sqrt(size)), int(sqrt(size))};
  int periods[2] = {1, 1};
  int rorder = 0;

  std::cout << "00" << '\n';
  MPI_Barrier(MPI_COMM_WORLD);
  
  MPI_Comm comm;
  MPI_Cart_create(MPI_COMM_WORLD, 2, dimen, periods, rorder, &comm);

  MPI_Comm subcomm[2];
  for (int i = 0; i < 2; i++) {
    int remain[2] = {0, 0};
    remain[i] = 1;
    MPI_Cart_sub(comm, remain, &subcomm[i]);
  }
  MPI_Comm comm_x = subcomm[0];
  MPI_Comm comm_y = subcomm[1];
  int mype_x, mype_y;
  MPI_Comm_rank(comm_x, &mype_x);
  MPI_Comm_rank(comm_y, &mype_y);

  std::cout << "11" << '\n';
  int* recvcount = new int[dimen[1]];
  int* rdispls = new int[dimen[1]];

  int num = 2;

  for (int i = 0; i < dimen[1]; i++) {
    recvcount[i] = num;
  }

  rdispls[0] = 0;
  for (int i = 1; i < dimen[1]; i++) {
    rdispls[i] = rdispls[i - 1] + recvcount[i - 1];
  }
  std::cout << "22" << '\n';
  int n1 = 2;
  int** send = new int*[n1];
  for (int i = 0; i < n1; i++) {
    send[i] = new int[num];
    for (int j = 0; j < num; j++)
      send[i][j] = j;
  }

  int num1 = 0;
  for (int i = 0; i < size; i++) {
    num1 = num1 + recvcount[i];
  }

  int* tmp = new int[num1];

  std::cout << "33" << '\n';
  MPI_Barrier(MPI_COMM_WORLD);

  for (int i = 0; i < n1; i++) {
    MPI_Allgatherv(send[i], num, MPI_INT, tmp, recvcount, rdispls, MPI_INT,
                   comm_y);
  }
  std::cout << "44" << '\n';
  MPI_Finalize();
}
