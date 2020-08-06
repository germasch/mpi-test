#include <mpi.h>

#include <cassert>
#include <iostream>
#include <math.h>
#include <numeric>
#include <vector>

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
  const int num = 2;
  std::vector<int> recvcount(dimen[1], num);
  std::vector<int> rdispls(dimen[1]);

  rdispls[0] = 0;
  for (int i = 1; i < dimen[1]; i++) {
    rdispls[i] = rdispls[i - 1] + recvcount[i - 1];
  }
  std::cout << "22" << '\n';
  const int n1 = 2;
  std::vector<std::vector<int>> send(n1);
  for (int i = 0; i < n1; i++) {
    send[i].resize(num);
    for (int j = 0; j < num; j++)
      send[i][j] = j;
  }

  int n_recv = std::accumulate(recvcount.begin(), recvcount.end(), 0);

  int* tmp = new int[n_recv];

  std::cout << "33" << '\n';
  MPI_Barrier(MPI_COMM_WORLD);

  for (int i = 0; i < n1; i++) {
    MPI_Allgatherv(send[i].data(), num, MPI_INT, tmp, recvcount.data(), rdispls.data(),
                   MPI_INT, comm_y);
  }
  std::cout << "44" << '\n';
  MPI_Finalize();
}
