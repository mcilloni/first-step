/*
 *  This file is part of First Step.
 *  
 *  First Step is free software: you can redistribute it and/or modify it under the terms of the GNU Lesser General Public License as published by the Free Software 
 *  Foundation, either version 3 of the License, or (at your option) any later version. 
 *
 *  First Step is distributed in the hope that it will be useful, but 
 *  WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU Lesser General Public License for more details.
 *
 *  You should have received a copy of the GNU Lesser General Public License
 *  along with First Step.  If not, see <http://www.gnu.org/licenses/>
 *
 *  Copyright (C) Marco Cilloni <marco.cilloni@yahoo.com> 2014
 *
 */
#undef _GNU_SOURCE

#include <cerrno>
#include <fcntl.h>
#include <cstdint>
#include <cstring>
#include <unistd.h>

#include <sys/stat.h>

extern "C" {

int64_t open_readfile(char *filename, char *error, uintptr_t len) {
  int64_t fd = open(filename, O_RDONLY);

  if (fd < 0) {
    strerror_r(errno, error, len);
    return -1;
  }

  return fd;
}


int64_t open_writefile(char *filename, char *error, uintptr_t len) {
  int64_t fd = creat(filename, S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH);

  if (fd < 0) {
    strerror_r(errno, error, len);
    return -1;
  }

  return fd;
}

uint8_t stream_closefileInternal(int64_t fd, char *error, uintptr_t len) {
  int res = close(fd);

  if (res == -1) {
    strerror_r(errno, error, len);
    return 0;
  }

  return 1;
}

intptr_t stream_readfileInternal(int64_t fd, void *data, intptr_t len, char *error, uintptr_t errl) {
  intptr_t res = read(fd, data, len);

  if (res < 0) {
   strerror_r(errno, error, errl);
   return -1;
  }

  return res;
}

intptr_t stream_writefileInternal(int64_t fd, void *data, intptr_t len, char *error, uintptr_t errl) {
  intptr_t res = write(fd, data, len);

  if (res < 0) {
    strerror_r(errno, error, errl);
    return -1;
  }

  return res;
}

int8_t stream_closeFileInternal(int64_t fd) {
  return close(fd) == 0;
}

}

