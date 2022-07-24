# Eratosthenes sieve

Parallel implementation of the erastothenes sieve algorithm, using OpenMPI.

## Build and execute

To build the binary, just run `make`. The binary can be found at
`build/eratosthenes-sieve`. To run the binary sequentially, follow the format:

```
./build/eratosthenes-sieve <right-limit> <mode>
```

`<right-limit>` is the maximum prime number that must be calculated. `<mode>`
can be one of:

- `l` -- print the list of primes up until `<right-limit>`.
- `t` -- print the execution time.
- `a` -- both `l` and `t`. That is, print list of primes and execution time.

For exemple, to print all prime numbers up to 100,000, run:

```
./build/eratosthenes-sieve 100000 l
```

### MPI

To run with MPI:

```
mpiexec -n <NUM_PROCESSES> ./build/eratosthenes-sieve <righ-limit> <mode>
```
