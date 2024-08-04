lsp: lsplus.c
	gcc -o lsp lsplus.c -O3

err: lsplus.c
	gcc -o lsp lsplus.c -Werror -Wpedantic -Wextra -g
