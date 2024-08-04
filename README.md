# LS Plus

A minimal alternative to the `ls` command which properly sorts the output alphabetically, puts a border between columns, separates files and directories, and shows only the most important information (to me). Written in C. Tested on (GNU/)Linux.

## Use
Open your favourite terminal emulator and type in the `lsp` command.
```
lsp [option(s)]

  -f    Only list files.
  -d    Only list directories.
  -r    Reverse the order in which file names are sorted. Directories are still listed after files.
```

You'll have to add the binary to your PATH if you'd like to use it anywhere.

## Dependencies

Uses the `test` and `stat` system commands.

## Build

Open a Unix-like terminal in the folder and run
```
make
```
or
```
gcc -o lsp lsplus.c
```

## Example

![20240804_04h52m06s_grim](https://github.com/user-attachments/assets/813af2ed-330c-4ee4-a630-91ced341e22b)


