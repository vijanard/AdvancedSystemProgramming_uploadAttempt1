#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <string.h>
#include <utime.h>

int main(int argc, char* argv[])
{
	int opt = 0, force = 0, write_flags = (O_WRONLY | O_CREAT | O_TRUNC);
	while((opt = getopt(argc, argv, "fa")) != -1) {
        switch(opt) {
            case 'f': force = 1; break;
        }
    }
	if ((argc - optind) < 2)
	{
		printf("Useage: %s FILE1 FILE2\n", argv[0]);
		exit(EXIT_FAILURE);
	}
	if (force != 1)
	{
		write_flags |= O_EXCL;
	}
	struct stat stat_info = {};
	int cannot_stat = 0;
	if (stat(argv[optind + 1], &stat_info) == -1)
	{
		cannot_stat = 1;
	}
	int fd1 = open(argv[optind], O_RDONLY);
	if (fd1 < 0)
	{
		perror("unable to open first file");
		exit(EXIT_FAILURE);
	}
	char* dest = NULL;
	if (cannot_stat == 0 && S_ISDIR(stat_info.st_mode) == 1)
	{
		const size_t len1 = strlen(argv[optind]);
		const size_t len2 = strlen(argv[optind + 1]);
		const size_t len = len1 + len2 + 1;  // +1 for path seperator
		if ((dest = malloc((len + 1) * sizeof(char))) == NULL)  // +1 for null
		{
			perror("malloc");
			close(fd1);
			exit(EXIT_FAILURE);
		}
		memset(dest, '\0', len + 1);
		strncpy(dest, argv[optind + 1], len2);
		strncpy(dest+len2, "/", 1);
		strncpy(dest+len2+1, argv[optind], len1);
	}
	else
	{
		dest = strdup(argv[optind + 1]);
	}
	int fd2 = open(dest, write_flags, S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH);
	if (fd2 < 0)
	{
		perror("error second file");
		close(fd1);
		free(dest);
		exit(EXIT_FAILURE);
	}

	int read_size = 0;
	char read_bytes[1000];
	while((read_size = read(fd1, read_bytes, 1000)) > 0)
	{
		if (write(fd2, read_bytes, read_size) == -1)
		{
			perror("unable to write");
		}
	}
	if (read_size < 0)
	{
		perror("read failed");
	}

	if (fstat(fd1, &stat_info) != 0)
	{
		perror("cannot stat file1");
		close(fd1);
		close(fd2);
		free(dest);
		return EXIT_FAILURE;
	}
	close(fd1);

	int exit_status = EXIT_SUCCESS;
	if (fchown(fd2, stat_info.st_uid, stat_info.st_gid) != 0)
	{
		perror("error changing ownership");
		exit_status = EXIT_FAILURE;
	}
	if (fchmod(fd2, stat_info.st_mode) != 0)
	{
		perror("error changing permissions");
		exit_status = EXIT_FAILURE;
	}

	close(fd2);

	struct utimbuf new_time = { stat_info.st_atim.tv_sec, stat_info.st_mtim.tv_sec };
	if (utime(dest, &new_time) != 0)
	{
		perror("error changing timestamps");
		exit_status = EXIT_FAILURE;
	}

	free(dest);
	return exit_status;
}
