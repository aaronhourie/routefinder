#include"../hdr/routefinder.h"

int main (int argc, char** argv){

  fwd_table_t** tables = parse_arguments(argc, argv);

  fprintf(stdout, "Please enter the source IP:");
  char source_ip[80];
  scanf("%s", source_ip);
  fprintf(stdout, "Please enter the destination IP:");
  char dest_ip[80];
  scanf("%s", dest_ip);

  unsigned int ip_source = str_to_ip(source_ip);
  unsigned int ip_dest = str_to_ip(dest_ip);

  if (ip_source != 0x0 && ip_dest != 0x0){
    find_path(tables, ip_source, ip_dest); 
  }

  destroy_tables(tables);
  return 0;
}

/*
 * This grabs the arguments for the command line and initializes the 
 * table accordingly.
 */
fwd_table_t** parse_arguments(int argc, char** argv){

  fwd_table_t** tables;

  if (argv != NULL) {
    char host_filename[80] = "hosts.txt";
    // Parse arguments
    for (int i = 1; i < argc; i++){
      
      switch (argv[i][1]){

        case 'f':
          i++;
          strncpy(host_filename, argv[i], 80);
          break;
      }
    }
    // Adjust behavour
    tables = get_tables_from_file(host_filename); 
  }

  return tables;
}

fwd_table_t** get_tables_from_file(char* filename){

  fwd_table_t** tables;

  if (filename != NULL){
    // Opens file.
    FILE* file_tables = fopen(filename, "r");
    if (file_tables != NULL){
      // This will hold the lines coming out of the file.
      char line[80];
      // Gives a starting size. 
      int num_tables = 0;
      int max_tables = 2;
      tables = calloc(2, sizeof(fwd_table_t*));
      // Gets the first line of the file.
      while (fgets(line, 80, file_tables) != NULL){

        // Build the table from the first line.
        fwd_table_t* table = init_table_from_str(line);
        tables[num_tables] = table;
        num_tables ++;
        // If the maximum has been reached, extend the array.
        if (num_tables >= max_tables - 1){
          max_tables *= 2;
          tables = realloc(tables, (sizeof(fwd_table_t*) * max_tables));
          // Ensure newly allocated values are NULL
          for (int i = num_tables + 1; i < max_tables; i++){
            tables[i] = NULL;
          }
        }
        
        // Build the table entries.
        for (int i = 0; i < table->size; i++){
          // gets the next line (presumably a subnet entry
          if (fgets(line, 80, file_tables) != NULL){
            // Build it from the line.
            subnet_t* subnet = init_subnet_from_str(line);   
            // Add it to the table.
            table->subnets[i] = subnet;
          }
          else { // Invalid format.
            fprintf(stderr, "Error: Invalid subnet entry\n");
            break;
          }
        }
      }
    }
    fclose(file_tables);
  }

  return tables;
}

/*
 * This is the main function. It is called recursively and generates
 * a list of subnets within the host table of ip_source that match the
 * subnet mask. Then, it finds the most specific table and forwards to
 * it by calling the function on it again.
 *
 * The terminating 
 */
void find_path(fwd_table_t** the_tables, unsigned int ip_source, unsigned int ip_dest){

  if (the_tables != NULL){
    // Terminating case
    if (ip_source == ip_dest){
      fwd_table_t* source = find_host_ip(the_tables, ip_source);
      fprintf(stdout, "%s\n", source->hostname);
    }
    // Terminating case (Not located on forwarding tables)
    else if (ip_source == 0x0){
      fprintf(stdout, " the Internet...\n");
    }
    else {
      // Gets the table for host ip.
      fwd_table_t* source = find_host_ip(the_tables, ip_source);
      fprintf(stdout, "%s", source->hostname);

      // To store the possible subnets (a maximum size equal to the table's size)
      subnet_t* possible_subnets[source->size];
      // Keeps track of index to store at
      int stored_subnets = 0;

      // Loops through all the available subnets in source.
      for (int i = 0; i < source->size; i++){
        subnet_t* to_check = source->subnets[i];
        // Null signifies the end of the list.
        if (to_check != NULL){
          // Checks if the destination IP with the subnet mask applied
          // is the same as the subnet. If so, it will forward it.
          if (apply_mask(ip_dest, to_check->ip_mask) == to_check->ip_subnet){
            // Adds it to the list
            possible_subnets[stored_subnets] = to_check;
            // Increments the counter.
            stored_subnets++;
          }
        }
        else {
          break;
        }
      }
      
      if (stored_subnets >= 1){
        // Checks to find the most specific subnet
        subnet_t* largest = possible_subnets[0];

        for (int i = 0; i < stored_subnets; i++){
          if (possible_subnets[i]->ip_mask > largest->ip_mask){
            largest = possible_subnets[i];
          } 
        }

        // Forward to new IP
        unsigned int new_source = largest->ip_forward;
        fprintf(stdout, " -> ");
        find_path(the_tables, new_source, ip_dest); 
      }
      else {

        //fprintf(stdout, "%d", ip_source);
      }
    }
  }
}

/*
 * This function searches through the tables array to find an table
 * with a host IP equal to the query.
 * Linear search, 0(n)
 */
fwd_table_t* find_host_ip(fwd_table_t** the_tables, unsigned int ip_query){
  
  fwd_table_t* result = NULL;

  if (the_tables != NULL){
    // This size may be larger than the number of items.
    int size = sizeof(the_tables);

    for (int i = 0; i < size; i++){
      if (the_tables[i] != NULL) {
        if (the_tables[i]->ip_host == ip_query){
          // Found query.
          result = the_tables[i];
          break;
        }
      }
      else {
        // No more items
        fprintf(stderr, "Error: IP address not found!\n");
        break;
      }
    }
  }

  return result;
}

/*
 * Destroys the tables array.
 * Calls the destroy function for every entry in the table
 * NULL signifies the end of the array.
 */
void destroy_tables(fwd_table_t** the_tables){

  // Make sure data is not null
  if (the_tables != NULL){

    // Get the max size of the table list.
    int size = sizeof(the_tables);

    for (int i = 0; i < size; i++){
      // We have not reached the end of used list space
      if (the_tables[i] != NULL){
        fwd_table_destroy(the_tables[i]);
      }
      else {
        // This will happen when we have reached the end of the used
        // list space.
        break;
      }
    }
  }

  free(the_tables);
}

/*
 * A debugging function. This prints the contents of the tables
 * array to stdout
 */
void enumerate_tables(fwd_table_t** the_tables){

  if (the_tables != NULL){
    int size = sizeof(the_tables);
    // Enumerates the tables.
    for (int i = 0; i < size; i++){
      if (the_tables[i] != NULL){
        printf("Host name: %s\nHost IP: 0x%08x\nSize: %d\n", the_tables[i]->hostname, the_tables[i]->ip_host, the_tables[i]->size);
        // Enumerates the entries
        for (int j = 0; j < the_tables[i]->size; j++){
          // Eliminates potential seg faults.
          if (the_tables[i]->subnets[j] != NULL){
            printf("Subnet: 0x%08x -> 0x%08x - Subnet Mask: 0x%08x\n", the_tables[i]->subnets[j]->ip_subnet, the_tables[i]->subnets[j]->ip_forward, the_tables[i]->subnets[j]->ip_mask);
          } 
        }
      }
      else {
        break;
      }
    }
  }
}


