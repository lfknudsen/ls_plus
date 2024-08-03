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

// remember: test returns 0 if true!
static int custom_listing_sort(char* a, char* b)
{
	char* ta = malloc(strlen("test -f \"") + strlen(a) + 1 + 1);
	char* tb = malloc(strlen("test -f \"") + strlen(b) + 1 + 1);

	// if b is a regular file, but b isn't, swap them.
	sprintf(ta, "test -f \"%s\"", a);
	sprintf(tb, "test -f \"%s\"", b);
	if (!system(tb))
	{
		if (system(ta))
		{
			free(ta);
			free(tb);
			return 1;
		}
		else
			return strcoll(a, b);
	}

	// if b is a directory, but a isn't, swap them.
	sprintf(ta, "test -d \"%\"", a);
	sprintf(tb, "test -d \"%s\"", b);
	if (!system(tb))
	{
		if (system(ta))
		{
			free(ta);
			free(tb);
			return 1;
		}
		else
			return strcoll(a, b);
	}

	free(ta);
	free(tb);
	int comp = strcoll(a,b);
	if (comp = 0)
	{
		return -1;
		printf("%s == %s.\n", a, b);
	}
	if (comp > 1)
		printf("%s > %s.\n", a, b);
	else
		printf("%s < %s.\n", a, b);
	return comp;
}

// remember: test returns 0 if true!
static int listing_sort(const struct dirent** a, const struct dirent** b)
{
	assert(a[0]->d_name != b[0]->d_name);

	char* ta = malloc(strlen("test -f '") + strlen(a[0]->d_name) + 1 + 1);
	char* tb = malloc(strlen("test -f '") + strlen(b[0]->d_name) + 1 + 1);

	// if b is a regular file, but b isn't, swap them.
	sprintf(ta, "test -f '%s'", a[0]->d_name);
	sprintf(tb, "test -f '%s'", b[0]->d_name);
	if (!system(tb))
	{
		if (system(ta))
		{
			free(ta);
			free(tb);
			return 1;
		}
		else
			return strcoll(a[0]->d_name, b[0]->d_name);
	}

	// if b is a directory, but a isn't, swap them.
	sprintf(ta, "test -d '%s'", a[0]->d_name);
	sprintf(tb, "test -d '%s'", b[0]->d_name);
	if (!system(tb))
	{
		if (system(ta))
		{
			free(ta);
			free(tb);
			return 1;
		}
		else
			return strcoll(a[0]->d_name, b[0]->d_name);
	}

	free(ta);
	free(tb);
	int comp = strcoll(a[0]->d_name,b[0]->d_name);
	if (comp = 0)
		printf("%s == %s.\n", a[0]->d_name, b[0]->d_name);
		return -1;
	if (comp > 1)
		printf("%s > %s.\n", a[0]->d_name, b[0]->d_name);
	else
		printf("%s < %s.\n", a[0]->d_name, b[0]->d_name);
	return comp;
}

void swap(struct dirent** files, int i, int j)
{
	size_t j_sz = sizeof(*files[j]);
	struct dirent* storage = malloc(j_sz);
	memcpy(storage, files[i], j_sz);
	size_t i_sz = sizeof(*files[i]);
	files[i] = malloc(j_sz);
	memcpy(files[i], files[j], j_sz);
	files[j] = malloc(i_sz);
	memcpy(files[j], storage, i_sz);
	free(storage);
}

void ssort(struct dirent** files, int n) {
	int j = n;
	int i = 0;
	while (j > 1)
	{
		while (i < j)
		{
			if (custom_listing_sort(files[i]->d_name, files[j - 1]->d_name) > 0)
			{
				swap(files, i, j - 1);
			}
			i++;
		}
		i = 0;
		j--;
	}
}

int main()
{
	struct dirent** files;
    int n = scandir(".", &files, is_file, alphasort);
	if (n < 1)
	{
		printf("Could not scan files. Return val: %d\n", n);
		return n;
	}
	struct dirent** dirs;
    int dn = scandir(".", &dirs, is_dir, alphasort);
	if (dn < 1)
	{
		printf("Could not scan files. Return val: %d\n", dn);
		return dn;
	}

	size_t cmd_len = strlen("stat --printf=\"%%A  \" \"") + 2;

	printf("\x1b[33mPermissions Name\x1b[m\n", n);
	for (int i = 0; i < n; i++)
	{
		char* cmd = malloc(cmd_len + strlen(files[i]->d_name));
		sprintf(cmd, "stat --printf=\"%%A  \" \"%s\"", files[i]->d_name);
		system(cmd);
		printf("%s\n", files[i]->d_name);
		free(files[i]);
		free(cmd);
	}
	free(files);
	for (int i = 0; i < dn; i++)
	{
		char* cmd = malloc(cmd_len + strlen(dirs[i]->d_name));
		sprintf(cmd, "stat --printf=\"%%A  \" \"%s\"", dirs[i]->d_name);
		system(cmd);
		printf("\x1b[32m%s\x1b[m\n", dirs[i]->d_name);
		free(dirs[i]);
		free(cmd);
	}
	free(dirs);
	return 0;
}
