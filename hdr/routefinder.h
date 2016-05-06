#ifndef _ROUTEFINDER_H_
#define _ROUTEFINDER_H_

#include<stdlib.h>
#include<stdio.h>
#include<stdbool.h>
#include"fwd_table.h"


// Data parsing.
fwd_table_t** parse_arguments(int argc, char** argv);
fwd_table_t** get_tables_from_file(char* filename);

// Data processing
void find_path(fwd_table_t** the_tables, unsigned int ip_source, unsigned int ip_dest);
fwd_table_t* find_host_ip(fwd_table_t** the_tables, unsigned int ip_query);

// Memory Management
void destroy_tables(fwd_table_t** the_tables);


// Debugging
void enumerate_tables(fwd_table_t** the_tables);
#endif
