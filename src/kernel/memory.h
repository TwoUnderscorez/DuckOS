#ifndef MEMORY_H
#define MEMORY_H

struct page_table_entry {
    unsigned char page_frame_number : 8;
    unsigned char present : 1;
    unsigned char protection: 3;
    unsigned char modified : 1;
    unsigned char caching : 1;
    unsigned char padding : 2;
    unsigned short reserved : 16;
} __attribute__((packed));

typedef struct page_table_entry page_table_entry_t;

#endif