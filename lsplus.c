#include <dirent.h>
#include <stdio.h>
#include <sys/types.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

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

int is_number(char c)
{
	return (c >= 48 && c <= 58);
}
int is_lowercase(char c)
{
	return (c >= 97 && c <= 122);
}
int is_uppercase(char c)
{
	return (c >= 64 && c <= 90);
}

int asort(const struct dirent** a, const struct dirent** b)
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

int reverse_sort(const struct dirent** a, const struct dirent** b)
{
	int result = alphasort(a,b);
	if (result < 0)
		return 1;
	else
		return -1;
}

int reverse_asort(const struct dirent** a, const struct dirent** b)
{
	int result = asort(a,b);
	if (result < 0)
		return 1;
	else
		return -1;
}
int main(int argc, char** argv)
{
	char narrow_type = 0;
	char sort_order  = 0;
	int target = -1;

	for (int i = 1; i < argc; i++)
	{
		if		(strcmp(argv[i],"-f") == 0) narrow_type =  1;
		else if (strcmp(argv[i],"-d") == 0) narrow_type = -1;
		else if (strcmp(argv[i],"-r") == 0) sort_order  =  1;
	}

	struct dirent** files;
	int n = 0;
	if (narrow_type >= 0)
	{
		if (sort_order == 1)
			n = scandir(".", &files, is_file, reverse_asort);
		else
			n = scandir(".", &files, is_file, asort);
		if (n < 1)
		{
			printf("Could not scan files. Return val: %d\n", n);
			return n;
		}
	}

	struct dirent** dirs;
	int dn = 0;
	if (narrow_type <= 0)
	{
		if (sort_order == 1)
			dn = scandir(".", &dirs, is_dir, reverse_asort);
		else
			dn = scandir(".", &dirs, is_dir, asort);
		if (dn < 1)
		{
			printf("Could not scan files. Return val: %d\n", dn);
			return dn;
		}
	}

	int right_align = 0;

	const char* permissions = "Permissions";
	const char* size		= "Size";
	const char* name		= "Name";
	const char* block		= "█";
	const char* c_header	= "\x1b[33m";
	const char* c_regular	= "\x1b[m";
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

	printf("\x1b[33mPermissions █ ");
	for (int i = 0; i < size_align_t; i++)
		putchar(' ');
	printf("Size  █  ");
	if (right_align)
	{
		for (int i = 0; i < name_align; i++)
			putchar(' ');
	}
	printf("Name\x1b[m\n");
	if (narrow_type >= 0)
	{
		for (int i = 0; i < n; i++)
		{
			int name_len = strlen(files[i]->d_name);
			char* cmd = malloc(stat_sz + strlen(files[i]->d_name) + 1);
			sprintf(cmd, "stat --printf=\"%%A  █ %%%ss \" \"%s\"", size_align, files[i]->d_name);
			if (!system(cmd))
			{
				if (right_align)
					printf("%s%20s\n", cmd_after, files[i]->d_name);
				else
					printf("%s%s\n", cmd_after, files[i]->d_name);
			}
			free(files[i]);
			free(cmd);
		}
		free(files);
	}
	if (narrow_type <= 0)
	{
		for (int i = 0; i < dn; i++)
		{
			char* cmd = malloc(stat_sz + strlen(dirs[i]->d_name) + 1);
			sprintf(cmd, "stat --printf=\"%%A  █ %%%ss \" \"%s\"", size_align, dirs[i]->d_name);
			if (!system(cmd))
			{
				if (right_align)
					printf("%s\x1b[32m%20s\x1b[m\n", cmd_after, dirs[i]->d_name);
				else
					printf("%s\x1b[32m%s\x1b[m\n", cmd_after, dirs[i]->d_name);
			}
			free(dirs[i]);
			free(cmd);
		}
		free(dirs);
	}
	return 0;
}
