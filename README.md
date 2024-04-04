# vmrss

C implementation of ThePrimeagen's [https://github.com/theprimeagen/vmrss/](vmrss). Print a process'
Resident Set Size (VmRSS).

TODO: print child processes VmRSS info

## Building

Ensure `make` is installed and simply run `make`. All supported commands:
```
make [all]
make clean
make debug
```

NOTE: `make` defaults to using `tcc (Tiny C Compiler)` if it is installed. Otherwise it uses
`gcc (GNU Compiler Collection)`.

## License

Proudly free software, this work is licensed under the terms of the Mozilla Public License, v.2.0.
