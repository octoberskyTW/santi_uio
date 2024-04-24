/****************************************************************************
 *
 * Copyright (c) WhiteGaussian <z900300@gmail.com>
 *
 ****************************************************************************/
#include <errno.h>
#include <fcntl.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/mman.h>
#include <unistd.h>

#define UIO_DEVICE "/dev/uio0"
#define MAP_SIZE                                                               \
  4096 // Adjust the size to match the actual memory size of the UIO device

// Function to read a 32-bit value from a specific offset
uint32_t read_from_offset(void *mapped_addr, off_t offset) {
  if (offset < 0 || offset + sizeof(uint32_t) > MAP_SIZE) {
    fprintf(stderr, "Offset out of bounds\n");
    return 0;
  }
  return *((uint32_t *)(mapped_addr + offset));
}

// Function to write a 32-bit value to a specific offset
void write_to_offset(void *mapped_addr, off_t offset, uint32_t value) {
  if (offset < 0 || offset + sizeof(uint32_t) > MAP_SIZE) {
    fprintf(stderr, "Offset out of bounds\n");
    return;
  }
  *((uint32_t *)(mapped_addr + offset)) = value;
}

int main(int argc, char *argv[]) {
  if (argc > 4 || argc < 3) {
    fprintf(stderr, "Usage: %s -r <offset>\n", argv[0]);
    fprintf(stderr, "       %s -w <offset> <value>\n", argv[0]);
    return EXIT_FAILURE;
  }

  int uio_fd = open(UIO_DEVICE, O_RDWR);
  if (uio_fd < 0) {
    perror("Failed to open UIO device");
    return EXIT_FAILURE;
  }

  void *mapped_addr =
      mmap(NULL, MAP_SIZE, PROT_READ | PROT_WRITE, MAP_SHARED, uio_fd, 0);
  if (mapped_addr == MAP_FAILED) {
    perror("Failed to mmap UIO device");
    close(uio_fd);
    return EXIT_FAILURE;
  }
  // printf("mmap base addr = 0x%p\n", mapped_addr);

  off_t offset = strtoul(argv[2], NULL, 0);
  if (errno != 0) {
    perror("Invalid offset");
    munmap(mapped_addr, MAP_SIZE);
    close(uio_fd);
    return EXIT_FAILURE;
  }

  if (strcmp(argv[1], "-r") == 0) {
    // Read operation
    uint32_t read_value = read_from_offset(mapped_addr, offset);
    printf("Value at offset 0x%lx: 0x%08x\n", (long)offset, read_value);
  } else if (strcmp(argv[1], "-w") == 0 && argc == 4) {
    // Write operation
    uint32_t value = strtoul(argv[3], NULL, 0);
    if (errno != 0) {
      perror("Invalid value");
      munmap(mapped_addr, MAP_SIZE);
      close(uio_fd);
      return EXIT_FAILURE;
    }
    write_to_offset(mapped_addr, offset, value);
    printf("Wrote 0x%08x to offset 0x%lx\n", value, (long)offset);
  } else {
    fprintf(stderr, "Invalid arguments\n");
    munmap(mapped_addr, MAP_SIZE);
    close(uio_fd);
    return EXIT_FAILURE;
  }

  // Clean up
  munmap(mapped_addr, MAP_SIZE);
  close(uio_fd);

  return EXIT_SUCCESS;
}