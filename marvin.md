
# Marvin problems

For all of the following, I'm starting out with a `module purge`. The main issue I have is that this Cray is
just not behaving like a Cray should. I can (and do) work around all these issues by installing pretty much an
entire software stack from scratch using spack, but as a user I shouldn't have to do that.

I'm trying to build a simple MPI test -- it's available at https://github.com/germasch/mpi-test. It's using cmake 
as its buildsystem, but for the most part the problems here aren't related to that and would happen the same if
invoking the compiler by hand.

To reproduce:

```
git clone https://github.com/germasch/mpi-test
cd mpi-test
mkdir build
cd build
```

(After failed attempts, one may have to clean out the build directory again and start over.)

## Using the Cray programming environment on Marvin 

```
[kaig@ln-0001 build]$ module purge; module load PrgEnv-cray
Warning: CPU target module not loaded.
Library performance may be helped by loading
a CPU targeting module before loading PrgEnv-cray.
An example module is craype-sandybridge.

Loading PrgEnv-cray/1.0.6
  Loading requirement: craype/2.6.4 cce/9.1.3 cray-libsci/20.03.1 perftools-base/20.03.0
```

* Minor issue: It'd be nice if this automatically loaded craype-x86-rome as well.

## Adding cmake

I also need cmake

```
[kaig@ln-0001 build]$ module purge; module load craype-x86-rome PrgEnv-cray cmake
Loading PrgEnv-cray/1.0.6
  Loading requirement: craype/2.6.4 cce/9.1.3 cray-libsci/20.03.1 perftools-base/20.03.0
[kaig@ln-0001 build]$ cmake
cmake: /lib64/libstdc++.so.6: version `GLIBCXX_3.4.22' not found (required by cmake)
cmake: /lib64/libstdc++.so.6: version `GLIBCXX_3.4.20' not found (required by cmake)
cmake: /lib64/libstdc++.so.6: version `GLIBCXX_3.4.21' not found (required by cmake)
cmake: /lib64/libstdc++.so.6: version `CXXABI_1.3.9' not found (required by cmake)
```

* Issue: cmake is built against a different version of libstdc++. (This is a UNH-specific problem, since I believe cmake and its modulefile were installed by UNH. The rest of what I'm trying to do here are problems with Cray modules.)

I can work around this by adding `module load gcc/8.2.0`, but currenty I'm trying to use the Cray compilers,
so I'd preferably not risk messing it up by loading a gcc module.

```
[kaig@ln-0001 build]$ module purge; module load craype-x86-rome PrgEnv-cray cmake gcc/8.2.0
Loading PrgEnv-cray/1.0.6
  Loading requirement: craype/2.6.4 cce/9.1.3 cray-libsci/20.03.1 perftools-base/20.03.0
[kaig@ln-0001 build]$ cmake ..
-- The C compiler identification is Clang 9.0.0
-- The CXX compiler identification is Clang 9.0.0
-- Cray Programming Environment 2.6.4 C
-- Detecting C compiler ABI info
-- Detecting C compiler ABI info - done
-- Check for working C compiler: /opt/cray/pe/craype/2.6.4/bin/cc - skipped
-- Detecting C compile features
-- Detecting C compile features - done
-- Cray Programming Environment 2.6.4 CXX
-- Detecting CXX compiler ABI info
-- Detecting CXX compiler ABI info - done
-- Check for working CXX compiler: /opt/cray/pe/craype/2.6.4/bin/CC - skipped
-- Detecting CXX compile features
-- Detecting CXX compile features - done
-- Could NOT find MPI_C (missing: MPI_C_LIB_NAMES MPI_C_HEADER_DIR MPI_C_WORKS)
-- Could NOT find MPI_CXX (missing: MPI_CXX_LIB_NAMES MPI_CXX_HEADER_DIR MPI_CXX_WORKS)
CMake Error at /mnt/lustre/software/cmake/share/cmake-3.20/Modules/FindPackageHandleStandardArgs.cmake:230 (message):
  Could NOT find MPI (missing: MPI_C_FOUND MPI_CXX_FOUND)
Call Stack (most recent call first):
  /mnt/lustre/software/cmake/share/cmake-3.20/Modules/FindPackageHandleStandardArgs.cmake:594 (_FPHSA_FAILURE_MESSAGE)
  /mnt/lustre/software/cmake/share/cmake-3.20/Modules/FindMPI.cmake:1741 (find_package_handle_standard_args)
  CMakeLists.txt:9 (find_package)
```

The first part is good, compilers are detected properly. 

## Making MPI available

* Issue: MPI is not available by default in this programming environment

Workaround: try adding `cray-mvapich2_nogpu` by hand:

```
[kaig@ln-0001 build]$ module purge; module load craype-x86-rome PrgEnv-cray cmake gcc/8.2.0 cray-mvapich2_nogpu
Loading PrgEnv-cray/1.0.6
  Loading requirement: craype/2.6.4 cce/9.1.3 cray-libsci/20.03.1 perftools-base/20.03.0
[kaig@ln-0001 build]$ cmake ..
-- The C compiler identification is unknown
-- The CXX compiler identification is unknown
-- Detecting C compiler ABI info
-- Detecting C compiler ABI info - failed
-- Check for working C compiler: /opt/cray/pe/craype/2.6.4/bin/cc
-- Check for working C compiler: /opt/cray/pe/craype/2.6.4/bin/cc - broken
CMake Error at /mnt/lustre/software/cmake/share/cmake-3.20/Modules/CMakeTestCCompiler.cmake:66 (message):
  The C compiler

    "/opt/cray/pe/craype/2.6.4/bin/cc"

  is not able to compile a simple test program.

  It fails with the following output:

    Change Dir: /mnt/lustre/germaschewski/kaig/src/mpi-test/build/CMakeFiles/CMakeTmp

    Run Build Command(s):/usr/bin/gmake -f Makefile cmTC_38ef1/fast && /usr/bin/gmake  -f CMakeFiles/cmTC_38ef1.dir/build.make CMakeFiles/cmTC_38ef1.dir/build
    gmake[1]: Entering directory `/mnt/lustre/germaschewski/kaig/src/mpi-test/build/CMakeFiles/CMakeTmp'
    Building C object CMakeFiles/cmTC_38ef1.dir/testCCompiler.c.o
    /opt/cray/pe/craype/2.6.4/bin/cc    -o CMakeFiles/cmTC_38ef1.dir/testCCompiler.c.o -c /mnt/lustre/germaschewski/kaig/src/mpi-test/build/CMakeFiles/CMakeTmp/testCCompiler.c
    Error invoking pkg-config!
    Package libsci was not found in the pkg-config search path.
    Perhaps you should add the directory containing `libsci.pc'
    to the PKG_CONFIG_PATH environment variable
    No package 'libsci' found
    Package libsci was not found in the pkg-config search path.
    Perhaps you should add the directory containing `libsci.pc'
    to the PKG_CONFIG_PATH environment variable
    No package 'libsci' found
    gmake[1]: *** [CMakeFiles/cmTC_38ef1.dir/testCCompiler.c.o] Error 1
    gmake[1]: Leaving directory `/mnt/lustre/germaschewski/kaig/src/mpi-test/build/CMakeFiles/CMakeTmp'
    gmake: *** [cmTC_38ef1/fast] Error 2
```

* Issue: Well, this made it worse, the compilers stopped working :(
    
Adding cray-libsci into the mix, I finally get the code to configure and build:

```
[kaig@ln-0001 build]$ module purge; module load craype-x86-rome cray-libsci PrgEnv-cray cmake gcc/8.2.0 cray-mvapich2_nogpu
Loading PrgEnv-cray/1.0.6
  Loading requirement: craype/2.6.4 cce/9.1.3 perftools-base/20.03.0
[kaig@ln-0001 build]$ cmake ..
-- The C compiler identification is Clang 9.0.0
-- The CXX compiler identification is Clang 9.0.0
-- Cray Programming Environment 2.6.4 C
-- Detecting C compiler ABI info
-- Detecting C compiler ABI info - done
-- Check for working C compiler: /opt/cray/pe/craype/2.6.4/bin/cc - skipped
-- Detecting C compile features
-- Detecting C compile features - done
-- Cray Programming Environment 2.6.4 CXX
-- Detecting CXX compiler ABI info
-- Detecting CXX compiler ABI info - done
-- Check for working CXX compiler: /opt/cray/pe/craype/2.6.4/bin/CC - skipped
-- Detecting CXX compile features
-- Detecting CXX compile features - done
-- Found MPI_C: /opt/cray/pe/craype/2.6.4/bin/cc (found version "3.1")
-- Found MPI_CXX: /opt/cray/pe/craype/2.6.4/bin/CC (found version "3.1")
-- Found MPI: TRUE (found version "3.1")
-- Configuring done
-- Generating done
-- Build files have been written to: /mnt/lustre/germaschewski/kaig/src/mpi-test/build
[kaig@ln-0001 build]$ make
[ 25%] Building CXX object CMakeFiles/mpi-test.dir/mpi-test.cxx.o
[ 50%] Linking CXX executable mpi-test
[ 50%] Built target mpi-test
[ 75%] Building CXX object CMakeFiles/mpi-hello.dir/mpi-hello.cxx.o
[100%] Linking CXX executable mpi-hello
[100%] Built target mpi-hello
```

## Trying to run the executables

* Issue: the compiled executable does not run:

```
[kaig@ln-0001 build]$ ./mpi-hello
./mpi-hello: error while loading shared libraries: libibmad.so.12: cannot open shared object file: No such file or directory
```

Trying to run it through slurm doesn't help:
```
[kaig@ln-0001 build]$ module load slurm
[kaig@ln-0001 build]$ srun ./mpi-hello
srun: job 4901 queued and waiting for resources
srun: job 4901 has been allocated resources
/mnt/lustre/germaschewski/kaig/src/mpi-test/build/./mpi-hello: error while loading shared libraries: libibmad.so.12: cannot open shared object file: No such file or directory
srun: error: cn-0013: task 0: Exited with exit code 127
```

## Trying an alternate environment

* Issue: At this point I might have tried to use `PrgEnv-gnu` instead, but it does not exist on this machine.

## How I would like it to behave

This on "spock", a new Cray at Oak Ridge. (It's actually really rather cutting edge and the environment there itself is
under development, but that isn't really relevant)

After logging in:

```
Last login: Wed Jun 30 20:52:55 2021 from c-71-235-183-218.hsd1.nh.comcast.net
module list
[kaig1@login1 ~]$ module list

Currently Loaded Modules:
  1) cce/11.0.4              5) craype-network-ofi                      9) cray-mpich/8.1.4       13) DefApps/default
  2) craype/2.7.6            6) cray-dsmml/0.1.4                       10) cray-libsci/21.04.1.1  14) PrgEnv-cray/8.0.0
  3) craype-x86-rome         7) perftools-base/21.02.0                 11) cray-pmi/6.0.10
  4) libfabric/1.11.0.3.74   8) xpmem/2.2.40-2.1_2.7__g3cf3325.shasta  12) cray-pmi-lib/6.0.10
```

Having a complete environment already loaded is kinda nice ;)

Everything works out of the box:
```
[kaig1@login1 ~]$ git clone https://github.com/germasch/mpi-test
Cloning into 'mpi-test'...
remote: Enumerating objects: 46, done.
remote: Counting objects: 100% (46/46), done.
remote: Compressing objects: 100% (22/22), done.
remote: Total 46 (delta 23), reused 45 (delta 22), pack-reused 0
Receiving objects: 100% (46/46), 7.55 KiB | 967.00 KiB/s, done.
Resolving deltas: 100% (23/23), done.
[kaig1@login1 ~]$ cd mpi-test
[kaig1@login1 mpi-test]$ mkdir build
[kaig1@login1 mpi-test]$ cd build
[kaig1@login1 build]$ cmake ..
-- The C compiler identification is Clang 11.0.0
-- The CXX compiler identification is Clang 11.0.0
-- Check for working C compiler: /opt/cray/pe/craype/2.7.6/bin/cc
-- Check for working C compiler: /opt/cray/pe/craype/2.7.6/bin/cc - works
-- Detecting C compiler ABI info
-- Detecting C compiler ABI info - done
-- Detecting C compile features
-- Detecting C compile features - done
-- Check for working CXX compiler: /opt/cray/pe/craype/2.7.6/bin/CC
-- Check for working CXX compiler: /opt/cray/pe/craype/2.7.6/bin/CC - works
-- Detecting CXX compiler ABI info
-- Detecting CXX compiler ABI info - done
-- Detecting CXX compile features
-- Detecting CXX compile features - done
-- Found MPI_C: /opt/cray/pe/craype/2.7.6/bin/cc (found version "3.1")
-- Found MPI_CXX: /opt/cray/pe/craype/2.7.6/bin/CC (found version "3.1")
-- Found MPI: TRUE (found version "3.1")
-- Configuring done
-- Generating done
-- Build files have been written to: /ccs/home/kaig1/mpi-test/build
[kaig1@login1 build]$ make
Scanning dependencies of target mpi-test
[ 25%] Building CXX object CMakeFiles/mpi-test.dir/mpi-test.cxx.o
[ 50%] Linking CXX executable mpi-test
[ 50%] Built target mpi-test
Scanning dependencies of target mpi-hello
[ 75%] Building CXX object CMakeFiles/mpi-hello.dir/mpi-hello.cxx.o
[100%] Linking CXX executable mpi-hello
[100%] Built target mpi-hello
```

Running works as well:
```
[kaig1@login1 build]$ ./mpi-hello
Hi there 0/1
[kaig1@login1 build]$ srun -n 2 -A FUS123 -t 00:10 -p ecp ./mpi-hello
Hi there 1/2
Hi there 0/2```
```

`PrgEnv-gnu` is also available, and works just the same as above, other than the compiler id changing correspondingly, so I'm not repeating the details here.









