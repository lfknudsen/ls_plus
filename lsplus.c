/*
 * Author: github.com/lfknudsen
 */

#include <dirent.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static int is_link(char* dname){
	if (dname[0] == '.' && (dname[1] == '.' || dname[1] == '\0'))
	{
		return 1;
	}
	return 0;
}

static int filter(char* a)
{
	char* ta = malloc(strlen("test -f \"") + strlen(a) + 1 + 1);
	sprintf(ta, "test -f \"%s\"", a);
	if (!system(ta))
	{
		free(ta);
		return 1;
	}

	sprintf(ta, "test -d \"%s\"", a);
	if (!system(ta) && !is_link(a))
	{
		free(ta);
		return 2;
	}

	free(ta);
	return 0;
}

static int is_const_link(const char* dname){
	if (dname[0] == '.' && (dname[1] == '.' || dname[1] == '\0'))
	{
		return 1;
	}
	return 0;
}

static int is_file_hide_dots(const struct dirent* a)
{
	char* ta = malloc(strlen("test -f \"") + strlen(a->d_name) + 1 + 1);
	sprintf(ta, "test -f \"%s\"", a->d_name);
	if (!system(ta) && a->d_name[0] != '.')
	{
		free(ta);
		return 1;
	}

	free(ta);
	return 0;
}

static int is_dir_hide_dots(const struct dirent* a)
{
	char* ta = malloc(strlen("test -d \"") + strlen(a->d_name) + 1 + 1);
	sprintf(ta, "test -d \"%s\"", a->d_name);
	if (!system(ta) && a->d_name[0] != '.')
	{
		free(ta);
		return 1;
	}

	free(ta);
	return 0;
}

static int is_file(const struct dirent* a)
{
	char* ta = malloc(strlen("test -f \"") + strlen(a->d_name) + 1 + 1);
	sprintf(ta, "test -f \"%s\"", a->d_name);
	if (!system(ta))
	{
		free(ta);
		return 1;
	}

	free(ta);
	return 0;
}

static int is_dir(const struct dirent* a)
{
	char* ta = malloc(strlen("test -d \"") + strlen(a->d_name) + 1 + 1);
	sprintf(ta, "test -d \"%s\"", a->d_name);
	if (!system(ta) && !is_const_link(a->d_name))
	{
		free(ta);
		return 1;
	}

	free(ta);
	return 0;
}

static int is_not_link(const struct dirent* a)
{
	if (!is_const_link(a->d_name))
	{
		char* ta = malloc(strlen("test -d \"") + strlen(a->d_name) + 1 + 1);
		sprintf(ta, "test -f \"%s\"", a->d_name);
		if (!system(ta))
		{
			free(ta);
			return 1;
		}
		sprintf(ta, "test -d \"%s\"", a->d_name);
		if (!system(ta))
		{
			free(ta);
			return 1;
		}
		free(ta);
	}
	return 0;
}

static int is_not_dot(const struct dirent* a)
{
	return (a->d_name[0] != '.');
}

static int is_number(char c)
{
	return (c >= 48 && c <= 58);
}
static int is_lowercase(char c)
{
	return (c >= 97 && c <= 122);
}
static int is_uppercase(char c)
{
	return (c >= 64 && c <= 90);
}

static int asort(const struct dirent** a, const struct dirent** b)
{
	int i = 0;
	char ac = (*a)->d_name[0];
	char bc = (*b)->d_name[0];
	while (ac != '\0' && bc != '\0')
	{
		if (is_uppercase(ac))
		{
			if (is_uppercase(bc))
			{
				if (ac < bc)
					return -1;
				else if (ac > bc)
					return 1;
				i ++;
				ac = (*a)->d_name[i];
				bc = (*b)->d_name[i];
				continue;
			}
			else if (is_lowercase(bc))
			{
				if (ac < bc - 32)
					return -1;
				else if (ac > bc - 32)
					return 1;
				i ++;
				ac = (*a)->d_name[i];
				bc = (*b)->d_name[i];
				continue;
			}
			else if (is_number(bc))
				return 1;
		}
		else if (is_lowercase(ac))
		{
			if (is_lowercase(bc))
			{
				if (ac < bc)
					return -1;
				else if (ac > bc)
					return 1;
				i ++;
				ac = (*a)->d_name[i];
				bc = (*b)->d_name[i];
				continue;
			}
			else if (is_uppercase(bc))
			{
				if (ac < bc + 32)
					return -1;
				else if (ac > bc + 32)
					return 1;
				i ++;
				ac = (*a)->d_name[i];
				bc = (*b)->d_name[i];
				continue;
			}
			else if (is_number(bc))
				return 1;
		}
		else if (is_number(ac))
		{
			if (is_number(bc))
			{
				if (ac < bc)
					return -1;
				if (ac > bc)
					return 1;
				i ++;
				ac = (*a)->d_name[i];
				bc = (*b)->d_name[i];
				continue;
			}
		}
		if (ac < bc) return -1;
		else if (ac > bc) return 1;
		i ++;
		ac = (*a)->d_name[i];
		bc = (*b)->d_name[i];
	}
	if (ac == '\0' && bc != '\0')
		return -1;
	if (ac != '\0' && bc == '\0')
		return 1;
	return 0;
}

static int reverse_sort(const struct dirent** a, const struct dirent** b)
{
	int result = alphasort(a,b);
	if (result < 0)
		return 1;
	return -1;
}

static int reverse_asort(const struct dirent** a, const struct dirent** b)
{
	int result = asort(a,b);
	if (result < 0)
		return 1;
	return -1;
}

static int asort_mix(const struct dirent** a, const struct dirent** b)
{
	int ai = 0;
	int bi = 0;
	char ac = (*a)->d_name[0];
	char bc = (*b)->d_name[0];
	if (ac == '.')
	{
		ac = (*a)->d_name[1];
		ai ++;
	}
	if (bc == '.')
	{
		bc = (*b)->d_name[1];
		bi ++;
	}
	while (ac != '\0' && bc != '\0')
	{
		if (is_uppercase(ac))
		{
			if (is_uppercase(bc))
			{
				if (ac < bc)
					return -1;
				else if (ac > bc)
					return 1;
				ai ++;
				bi ++;
				ac = (*a)->d_name[ai];
				bc = (*b)->d_name[bi];
				continue;
			}
			else if (is_lowercase(bc))
			{
				if (ac < bc - 32)
					return -1;
				else if (ac > bc - 32)
					return 1;
				ai ++;
				bi ++;
				ac = (*a)->d_name[ai];
				bc = (*b)->d_name[bi];
				continue;
			}
			else if (is_number(bc))
				return 1;
		}
		else if (is_lowercase(ac))
		{
			if (is_lowercase(bc))
			{
				if (ac < bc)
					return -1;
				else if (ac > bc)
					return 1;
				ai ++;
				bi ++;
				ac = (*a)->d_name[ai];
				bc = (*b)->d_name[bi];
				continue;
			}
			else if (is_uppercase(bc))
			{
				if (ac < bc + 32)
					return -1;
				else if (ac > bc + 32)
					return 1;
				ai ++;
				bi ++;
				ac = (*a)->d_name[ai];
				bc = (*b)->d_name[bi];
				continue;
			}
			else if (is_number(bc))
				return 1;
		}
		else if (is_number(ac))
		{
			if (is_number(bc))
			{
				if (ac < bc)
					return -1;
				if (ac > bc)
					return 1;
				ai ++;
				bi ++;
				ac = (*a)->d_name[ai];
				bc = (*b)->d_name[bi];
				continue;
			}
		}
		if (ac < bc)
			return -1;
		else if (ac > bc)
			return 1;
		ai ++;
		bi ++;
		ac = (*a)->d_name[ai];
		bc = (*b)->d_name[bi];
	}
	if (ac == '\0' && bc != '\0')
		return -1;
	if (ac != '\0' && bc == '\0')
		return 1;
	return 0;
}

static int reverse_asort_mix(const struct dirent** a, const struct dirent** b)
{
	int result = asort_mix(a,b);
	if (result < 0)
		return 1;
	return -1;
}

static void print_all(struct dirent** files, int n, const size_t stat_sz, const char* size_align,
					  const int right_align, const char* cmd_after, const char* colour_dir,
					  const char* colour_reg)
{
	size_t test_len = strlen("test -d \"");
	for (int i = 0; i < n; i++)
	{
		size_t name_len = strlen(files[i]->d_name);
		char* cmd = malloc(stat_sz + name_len + 1);
		sprintf(cmd, "stat --printf=\"%%A  █ %%%ss \" \"%s\"", size_align, files[i]->d_name);
		if (!system(cmd))
		{
			cmd = realloc(cmd, test_len + 2 + name_len);
			sprintf(cmd, "test -d \"%s\"", files[i]->d_name);
			const char* colour;
			if (!system(cmd))
				colour = colour_dir;
			else
				colour = colour_reg;
			if (right_align)
				printf("%s%s%20s\x1b[m\n", cmd_after, colour, files[i]->d_name);
			else
				printf("%s%s%s\x1b[m\n", cmd_after, colour, files[i]->d_name);
		}
		free(files[i]);
		free(cmd);
	}
}


static void print_dirents(struct dirent** files, int n, const size_t stat_sz, const char* size_align,
				   const int right_align, const char* cmd_after, const char* colour)
{
	for (int i = 0; i < n; i++)
	{
		char* cmd = malloc(stat_sz + strlen(files[i]->d_name) + 1);
		sprintf(cmd, "stat --printf=\"%%A  █ %%%ss \" \"%s\"", size_align, files[i]->d_name);
		if (!system(cmd))
		{
			if (right_align)
				printf("%s%s%20s\x1b[m\n", cmd_after, colour, files[i]->d_name);
			else
				printf("%s%s%s\x1b[m\n", cmd_after, colour, files[i]->d_name);
		}
		free(files[i]);
		free(cmd);
	}
}

static int scan_directory(struct dirent*** files, char seeking,
						 char use_reverse_sort, char use_alphasort, char hide_dotfiles,
						 char mix_dotfiles)
{
	int (*fn_filter) (const struct dirent*);
	if (seeking == 0)
	{
		if (hide_dotfiles)
			fn_filter = &is_file_hide_dots;
		else
			fn_filter = &is_file;
	}
	else if (seeking == 1)
	{
		if (hide_dotfiles)
			fn_filter = &is_dir_hide_dots;
		else
			fn_filter = &is_dir;
	}
	else
	{
		if (hide_dotfiles)
			fn_filter = &is_not_dot;
		else
			fn_filter = &is_not_link;
	}

	int (*fn_sort) (const struct dirent**, const struct dirent**);
	if (use_reverse_sort)
		if (use_alphasort)
			fn_sort = &reverse_sort;
		else if (mix_dotfiles)
			fn_sort = &reverse_asort_mix;
		else
			fn_sort = &reverse_asort;
	else
		if (use_alphasort)
			fn_sort = &alphasort;
		else if (mix_dotfiles)
			fn_sort = &asort_mix;
		else
			fn_sort = &asort;

	return scandir(".", files, fn_filter, fn_sort);
}

char clow(char a)
{
	if (a >= 65 && a <= 90)
		return a + 32;
	return a;
}

char cup(char a)
{
	if (a >= 97 && a <= 122)
		return a - 32;
	return a;
}

char* slow(char* in, char* out)
{
	out = malloc(strlen(in));
	for (size_t i = 0; i < strlen(in); i++)
	{
		if (in[i] >= 65 && in[i] <= 90)
			out[i] = in[i] + 32;
	}
	return out;
}

char* sup(char* in, char* out)
{
	out = malloc(strlen(in));
	for (size_t i = 0; i < strlen(in); i++)
	{
		if (in[i] >= 97 && in[i] <= 122)
			out[i] = in[i] - 32;
	}
	return out;
}

void print_help()
{
	printf("Small alternative to ls.\n");
	printf("Usage:\n");
	printf("lsp [option(s)]\n");
	printf("  -h    Print help information and exit.\n");
	printf("  -f    Only list regular [f]iles.\n");
	printf("  -d    Only list [d]irectories (incl. symbolic links).\n");
	printf("  -m    [Mix] together directories and files instead of separating them.\n");
	printf("  -a    Sort entries using the built-in C std library \"[a]lphasort\",\
 which more or less just sorts by direct numerical value\
 (e.g. 'Z' comes before 'a' to it). However, it should\
 take locale into account, so it may support non-ASCII\
 characters better.\n");
	printf("  -s    [S]wap directory and file listing order, so directories precede files.\n");
	printf("  -o	For the sake of sorting, the first d[o]t in filenames is ignored.\n");
	printf("  -i	[I]gnore all files/directories that start with `.`.\n");
	printf("  -g	Ri[g]ht-align filenames.\n");
}

/*
* Very minimal alternative to ls.
* Print a list of files and/or directories in the current directory,
* along with permissions and size (in bytes).
* Directory names are coloured.
*
* Usage:
lsp [option(s)]

-f		Only list regular [f]iles.
-d		Only list [d]irectories (including symbolic links).
-m		[M]ix together directories and files instead of separating them.
-r      Sort in [r]everse alphabetical order.
-a      Sort entries using the built-in C std library "[a]lphasort", which more or less just
		sorts by direct numerical value (e.g. 'Z' comes before 'a' to it). However, it should
		take locale into account, so it may support non-ASCII characters better.
-s      [S]wap directory and file listing order, so directories precede files.
-o		For the sake of sorting, the first d[o]t in filenames is ignored.
-i		[I]gnore all files/directories that start with `.`.
-g		Ri[g]ht-align filenames.
*/
int main(int argc, char** argv)
{
	char narrow_type		= 0;
	char mix_types			= 0;
	char use_reverse_sort	= 0;
	char use_alphasort		= 0;
	char swap_print_order	= 0;
	char mix_dotfiles		= 0;
	char hide_dotfiles		= 0;
	char right_align		= 0;
	char print_help_msg		= 0;

	for (int i = 1; i < argc; i++)
	{
		if (argv[i][0] == '-')
		{
			int idx = 1;
			char c = argv[i][idx];
			while (c != '\0')
			{
				if		(c == 'f') { narrow_type		=  1; }
				else if (c == 'd') { narrow_type		= -1; }
				else if (c == 'm') { mix_types			=  1; }
				else if (c == 'r') { use_reverse_sort	=  1; }
				else if (c == 'a') { use_alphasort		=  1; }
				else if (c == 's') { swap_print_order	=  1; }
				else if (c == 'o') { mix_dotfiles		=  1; }
				else if (c == 'i') { hide_dotfiles		=  1; }
				else if (c == 'g') { right_align		=  1; }
				else if (c == 'h') { print_help_msg		=  1; }
				idx ++;
				c = argv[i][idx];
			}
		}
	}

	if (print_help_msg)
	{
		print_help();
		return 0;
	}
	// printf("Narrow type: %d, Mix types: %d, Reverse sort: %d,\nAlphasort: %d, Swap: %d, Mix dotfiles: %d,\nHide dotfiles: %d, Right align: %d\n",
	// narrow_type, mix_types, use_reverse_sort, use_alphasort, swap_print_order, mix_dotfiles, hide_dotfiles, right_align);

	struct dirent** files;
	int n = 0;
	struct dirent** dirs;
	int dn = 0;

	if (narrow_type == 0 && mix_types)
	{
		n = scan_directory(&files, -1, use_reverse_sort, use_alphasort, hide_dotfiles, mix_dotfiles);
		if (n <= 0)
			return 0;
	}
	else
	{
		if (narrow_type >= 0)
		{
			n = scan_directory(&files, 0, use_reverse_sort, use_alphasort, hide_dotfiles, mix_dotfiles);
			if (n < 0)
			{
				printf("Could not scan files. Scandir() returned error code %d.\n", errno);
				return 1;
			}
		}

		if (narrow_type <= 0)
		{
			dn = scan_directory(&dirs, 1, use_reverse_sort, use_alphasort, hide_dotfiles, mix_dotfiles);
			if (dn < 0)
			{
				printf("Could not scan files. Scandir() returned error code %d.\n", errno);
				return 1;
			}
		}

		if (n <= 0 && dn <= 0)
			return 0;
	}

	const char* permissions = "Permissions";
	const char* size		= "Size";
	const char* name		= "Name";
	const char* c_header	= "\x1b[33m";
	const char* c_regular	= "\x1b[m";
	const char* c_directory = "\x1b[32m";
	const char* stat_a		= "stat --printf=\"%%%A  █  %%%";
	const char* stat_b		= "s \" \"";
	const char* cmd_after	= " █  ";
	const int size_align_t  = 10 - strlen(size);
	const char*	size_align  = "10";

	const size_t a_sz		= strlen(stat_a);
	const size_t b_sz		= strlen(stat_b);
	const size_t size_align_sz	= strlen(size_align);
	const size_t stat_sz	= a_sz + b_sz + size_align_sz + 1;
	char* stat				= malloc(stat_sz);
	memcpy(stat, stat_a, a_sz);
	memcpy(stat + a_sz, size_align, size_align_sz);
	memcpy(stat + a_sz + size_align_sz, stat_b, b_sz);
	stat[stat_sz - 1] = '\0';

	unsigned char name_align= 21 - strlen(name);

	printf("%s%s █ ", c_header, permissions);
	for (int i = 0; i < size_align_t; i++)
		putchar(' ');
	printf("%s  █  ", size);
	if (right_align)
	{
		for (int i = 0; i < name_align - 1; i++)
			putchar(' ');
	}
	printf("%s%s\n", name, c_regular);
	if (mix_types && files)
	{
		print_all(files, n, stat_sz, size_align, right_align, cmd_after, c_directory, c_regular);
		free(files);
	}
	else if (swap_print_order)
	{
		if (narrow_type <= 0 && dirs)
		{
			print_dirents(dirs, dn, stat_sz, size_align, right_align, cmd_after, c_directory);
			free(dirs);
		}
		if (narrow_type >= 0 && files)
		{
			print_dirents(files, n, stat_sz, size_align, right_align, cmd_after, c_regular);
			free(files);
		}
	}
	else
	{
		if (narrow_type >= 0 && files)
		{
			print_dirents(files, n, stat_sz, size_align, right_align, cmd_after, c_regular);
			free(files);
		}
		if (narrow_type <= 0 && dirs)
		{
			print_dirents(dirs, dn, stat_sz, size_align, right_align, cmd_after, c_directory);
			free(dirs);
		}
	}
	return 0;
}
