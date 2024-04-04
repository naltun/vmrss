# vmrss

Print a process' Resident Set Size (VmRSS) 🖥️🔬✨

C implementation of ThePrimeagen's [https://github.com/theprimeagen/vmrss/](vmrss)

TODO: print child processes' VmRSS info

## Building

Ensure `make` is installed and simply run `make`. All supported commands:
```
make [all]
make clean
make debug
```

NOTE: `make` defaults to using `tcc (Tiny C Compiler)` if it is installed. Otherwise it uses
`gcc (GNU Compiler Collection)`.

NOTE: This requires a Unix machine. It has only been tested on GNU/Linux (GNU userspace with the
Linux kernel).

## License

Proudly free software, this work is licensed under the terms of the Mozilla Public License, v.2.0.
