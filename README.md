# LS Plus

A minimal alternative to the `ls` command which properly sorts the output alphabetically, puts a border between columns, separates files and directories, and shows only the most important information (to me). Written in C. Tested in a zsh shell on (GNU/)Linux.

## Use
Open your favourite terminal emulator and type in the `lsp` command.
```
lsp [option(s)]

-f		Only list regular [f]iles.
-d		Only list [d]irectories (including symbolic links).
-m		[M]ix together directories and files instead of separating them.
-r      Sort in [r]everse alphabetical order.
-a      Sort entries using the built-in C std library "[a]lphasort", which more or less just
		sorts by direct numerical value. However, it should take locale into account,
		so it may support non-ASCII characters better.
-s      [S]wap directory and file listing order, so directories precede files.
-o		For the sake of sorting, the first d[o]t in filenames is ignored.
-i		[I]gnore all files/directories that start with `.`.
-g		Ri[g]ht-align filenames.
```

You'll have to add the binary to your PATH,
or add an alias to your terminal emulator's config file,
if you'd like to use the program everywhere.

## Setup
### Dependencies

Uses the `test` and `stat` system commands.

### Build

Open a Unix-like terminal in the folder and run
```
make
```
or
```
gcc -o lsp lsplus.c -O3
```

### Alias example
The file will differ depending on your shell, but the syntax should be the same:
```
alias ls=~/lsplus/lsp
```
```
alias ls="~/lsplus/lsp -s"
```
Note that you can still supply arguments through the command line even if you
are executing the program through an alias.
## Example

![20240804_04h52m06s_grim](https://github.com/user-attachments/assets/813af2ed-330c-4ee4-a630-91ced341e22b)
