/*
 * This code was greatly inspired by parts of Git by Linus Torvalds
 * https://github.com/git/git
 * (C) 2005 Nicolas Pitre <nico@fluxnic.net>
 *
 * Rewritten for mgz-diff by Grégoire Lejeune <gregoire.lejeune@free.fr>, (C) 2013
 *
 * This code is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 */

#include <stdio.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/mman.h>
#include "internal/diff.h"

ssize_t xwrite(int fd, const void *buf, size_t len)
{
  ssize_t nr; 
  while (1) {
    nr = write(fd, buf, len);
    if ((nr < 0) && (errno == EAGAIN || errno == EINTR))
      continue;
    return nr; 
  }
}

ssize_t write_in_full(int fd, const void *buf, size_t count)
{
  const char *p = buf;
  ssize_t total = 0;

  while (count > 0) {
    ssize_t written = xwrite(fd, p, count);
    if (written < 0)
      return -1; 
    if (!written) {
      errno = ENOSPC;
      return -1; 
    }   
    count -= written;
    p += written;
    total += written;
  }

  return total;
}

static const char usage_str[] =
	"test-delta (-d|-p) <from_file> <data_file> <out_file>";

int main(int argc, char *argv[])
{
	int fd;
	struct stat st;
	void *from_buf, *data_buf, *out_buf;
	unsigned long from_size, data_size, out_size;

	if (argc != 5 || (strcmp(argv[1], "-d") && strcmp(argv[1], "-p"))) {
		fprintf(stderr, "Usage: %s\n", usage_str);
		return 1;
	}

	fd = open(argv[2], O_RDONLY);
	if (fd < 0 || fstat(fd, &st)) {
		perror(argv[2]);
		return 1;
	}
	from_size = st.st_size;
	from_buf = mmap(NULL, from_size, PROT_READ, MAP_PRIVATE, fd, 0);
	if (from_buf == MAP_FAILED) {
		perror(argv[2]);
		close(fd);
		return 1;
	}
	close(fd);

	fd = open(argv[3], O_RDONLY);
	if (fd < 0 || fstat(fd, &st)) {
		perror(argv[3]);
		return 1;
	}
	data_size = st.st_size;
	data_buf = mmap(NULL, data_size, PROT_READ, MAP_PRIVATE, fd, 0);
	if (data_buf == MAP_FAILED) {
		perror(argv[3]);
		close(fd);
		return 1;
	}
	close(fd);

	if (argv[1][1] == 'd')
		out_buf = diff_delta(from_buf, from_size,
				     data_buf, data_size,
				     &out_size, 0);
	else
		out_buf = patch_delta(from_buf, from_size,
				      data_buf, data_size,
				      &out_size);
	if (!out_buf) {
		fprintf(stderr, "delta operation failed (returned NULL)\n");
		return 1;
	}

	fd = open (argv[4], O_WRONLY|O_CREAT|O_TRUNC, 0666);
	if (fd < 0 || write_in_full(fd, out_buf, out_size) != out_size) {
		perror(argv[4]);
		return 1;
	}

	return 0;
}
