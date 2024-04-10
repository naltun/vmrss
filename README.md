# vmrss

Print parent, child process Resident Set Size (VmRSS) 🖥️🔬✨

C implementation of ThePrimeagen's [https://github.com/theprimeagen/vmrss/](vmrss)

TODO:
```
[ ] add monitor mode
[x] print child processes' VmRSS
[ ] refactor code
```

## Building

Ensure `make` is installed and simply run `make`. All supported commands:
```
make [all]  # Defaults to tcc (Tiny C Compiler), otherwise uses gcc
make clean
make debug  # Defaults to gcc for compatibility with gdb
make install[-system]  # /usr/local/bin/
make install-local     # $HOME/.local/bin/
```

NOTE: This requires a Unix machine and has only been tested on GNU/Linux (GNU userspace with the
Linux kernel).

## License

Proudly free software, this work is licensed under the terms of the Mozilla Public License, v.2.0.
