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
#define MAP_SIZE \
    4096  // Adjust the size to match the actual memory size of the UIO device


/*FE Interrupt Parameters Definition*/
#define FE_INT_STATUS 0x08
#define FE_INT_ENABLE 0x0c

enum mtk_fe_event_id {
    MTK_EVENT_FORCE = 0,
    MTK_EVENT_WARM_CNT = 1,
    MTK_EVENT_COLD_CNT = 2,
    MTK_EVENT_TOTAL_CNT = 3,
    MTK_EVENT_FQ_EMPTY = 8,
    MTK_EVENT_TSO_FAIL = 12,
    MTK_EVENT_TSO_ILLEGAL = 13,
    MTK_EVENT_TSO_ALIGN = 14,
    MTK_EVENT_RFIFO_OV = 18,
    MTK_EVENT_RFIFO_UF = 19,
    MTK_MAC1_LINK = 24,
    MTK_MAC2_LINK = 25,
};

char *mtk_fe_event_name[32] = {
    [MTK_EVENT_FORCE] = "Force",
    [MTK_EVENT_WARM_CNT] = "Warm",
    [MTK_EVENT_COLD_CNT] = "Cold",
    [MTK_EVENT_TOTAL_CNT] = "Total",
    [MTK_EVENT_FQ_EMPTY] = "FQ Empty",
    [MTK_EVENT_TSO_FAIL] = "TSO Fail",
    [MTK_EVENT_TSO_ILLEGAL] = "TSO Illegal",
    [MTK_EVENT_TSO_ALIGN] = "TSO Align",
    [MTK_EVENT_RFIFO_OV] = "RFIFO OV",
    [MTK_EVENT_RFIFO_UF] = "RFIFO UF",
    [MTK_MAC1_LINK] = "MAC1 LINK STAT CHANGE",
    [MTK_MAC2_LINK] = "MAC2 LINK STAT CHANGE",
};


/* Function to read a 32-bit value from a specific offset*/
uint32_t read_from_offset(void *mapped_addr, off_t offset)
{
    if (offset < 0 || offset + sizeof(uint32_t) > MAP_SIZE) {
        fprintf(stderr, "Offset out of bounds\n");
        return 0;
    }
    return *((uint32_t *) (mapped_addr + offset));
}

/* Function to write a 32-bit value to a specific offset*/
void write_to_offset(void *mapped_addr, off_t offset, uint32_t value)
{
    if (offset < 0 || offset + sizeof(uint32_t) > MAP_SIZE) {
        fprintf(stderr, "Offset out of bounds\n");
        return;
    }
    *((uint32_t *) (mapped_addr + offset)) = value;
}

void fe_irq_handler(void *mapped_addr)
{
    uint32_t status = 0, val = 0;

    status = read_from_offset(mapped_addr, FE_INT_STATUS);

    while (status) {
        val = ffs((unsigned int) status) - 1;
        status &= ~(1 << val);

        if ((val == MTK_EVENT_TSO_FAIL) || (val == MTK_EVENT_TSO_ILLEGAL) ||
            (val == MTK_EVENT_TSO_ALIGN) || (val == MTK_EVENT_RFIFO_OV) ||
            (val == MTK_EVENT_RFIFO_UF) || (val == MTK_MAC1_LINK) ||
            (val == MTK_MAC2_LINK))
            fprintf(stderr, "[%s] Detect FE event: %s !\n", __func__,
                    mtk_fe_event_name[val]);
    }
    /*reset interrupt status CR*/
    write_to_offset(mapped_addr, FE_INT_STATUS, 0xFFFFFFFF);
    return;
}


int main(int argc, char *argv[])
{
    int irq_event = 0;

    int uio_fd = open(UIO_DEVICE, O_RDWR);
    if (uio_fd < 0) {
        perror("Failed to open UIO device");
        return EXIT_FAILURE;
    }

    /*Get virtual memory address by mmap*/
    void *mapped_addr =
        mmap(NULL, MAP_SIZE, PROT_READ | PROT_WRITE, MAP_SHARED, uio_fd, 0);
    if (mapped_addr == MAP_FAILED) {
        perror("Failed to mmap UIO device");
        close(uio_fd);
        return EXIT_FAILURE;
    }

    /* Handle the interrupts from uio device*/
    do {
        read(uio_fd, (void *) &irq_event, sizeof(int));
        fe_irq_handler(mapped_addr);
        /*Re-enable FE_INT_ENABLE*/
        write_to_offset(mapped_addr, FE_INT_ENABLE, 0x030C7000);
        irq_event--;
    } while (1);

    /*Clean up*/
    munmap(mapped_addr, MAP_SIZE);
    close(uio_fd);

    return EXIT_SUCCESS;
}