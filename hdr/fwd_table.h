#ifndef _FWD_TABLE_H_
#define _FWD_TABLE_H_

#define IPV4_LEN 4

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

// This holds indivudal entries in the forwarding table.
typedef struct {
  // The subnet IP
  unsigned int ip_subnet;
  // The IP mask.
  unsigned int ip_mask;
  // The address to forward to
  unsigned int ip_forward;
} subnet_t;

// This holds one table for forwarding addresses.
typedef struct {
  // Number of entries.
  int size;
  // String hostname
  char* hostname;
  // The host IP
  int ip_host;
  // List of subnets.
  subnet_t** subnets;
} fwd_table_t;

// Memory management
fwd_table_t* fwd_table_init();
subnet_t* subnet_init();
void fwd_table_destroy(fwd_table_t* the_fwd_table);
void subnet_destroy(subnet_t* the_subnet);

// Minor data parsing.
unsigned int str_to_ip(char* string);
unsigned int intarr_to_ip(unsigned int ip[IPV4_LEN]);
unsigned int build_ip_mask(int mask_size);

// Main data parsing 
fwd_table_t* init_table_from_str(char* the_string);
subnet_t* init_subnet_from_str(char* the_string);

// Data processing
unsigned int apply_mask(unsigned int ip_check, unsigned int ip_mask);

#endif
