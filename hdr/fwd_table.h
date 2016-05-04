#ifndef _FWD_TABLE_H_
#define _FWD_TABLE_H_

#define IPV4_LEN 4

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// This holds indivudal entries in the forwarding table.
typedef struct {
  // The subnet IP
  int ip_subnet;
  // The IP mask.
  int ip_mask;
  // The address to forward to
  int ip_forward;
} subnet_t;

// This holds one table for forwarding addresses.
typedef struct {
  // Number of entries.
  int size;
  // String hostname
  char* hostname;
  //
  subnet_t** subnets;
} fwd_table_t;

// Memory management
fwd_table_t* fwd_table_init(char* hostname, int size);
subnet_t* subnet_init();
void fwd_table_destroy(fwd_table_t* the_fwd_table);
void subnet_destroy(subnet_t* the_subnet);

// Helper functions
unsigned int str_to_ip(char* string);
unsigned int intarr_to_ip(unsigned int ip[IPV4_LEN]);

#endif
