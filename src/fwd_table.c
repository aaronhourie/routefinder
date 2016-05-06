#include "../hdr/fwd_table.h"

/* 
 * Initializes a forwarding table. Requires that the host name be not NULL.
 * @param hostname - the "owner" host's human-readable name
 * @param size - the number of entries in the forwarding table.
 *
 * @return - the generated table, or NULL if the table failed to generate.
 */
fwd_table_t* fwd_table_init(){

  fwd_table_t* the_table;
  the_table = malloc(sizeof(fwd_table_t));
  the_table->hostname = calloc(80, sizeof(char));
  the_table->ip_host = 0;
  the_table->size = 0;

  return the_table;
}

/* 
 * Initializes a subnet entry. Sets all values to 0.
 *
 * @return - the generated subnet.
 */
subnet_t* subnet_init(){

  subnet_t* the_subnet = malloc(sizeof(subnet_t));

  the_subnet->ip_subnet = 0x0;
  the_subnet->ip_mask = 0x0;
  the_subnet->ip_forward = 0x0;
 
  return the_subnet;
}

/**
 * Destroys the table.
 */
void fwd_table_destroy(fwd_table_t* the_table){

  if (the_table != NULL){

    free(the_table->hostname);

    for (int i = 0; i < the_table->size; i++){
      if (the_table->subnets[i] != NULL){
        subnet_destroy(the_table->subnets[i]);
      }
    }

    free(the_table->subnets);

    free(the_table);
  }
}

/**
 * Destroys the subnet.
 * This is only a function so that there is consistency,
 * and in case more data is added to the struct, we won't have
 * to go searching for frees.
 */
void subnet_destroy(subnet_t* the_subnet){

  free(the_subnet);
}

/* What this function says it does:
 *  Turns an string IP into a real one.
 * What it actually does:
 *  Turns a string IP into an array of integers (size 4).
 *
 * This function then sends this array off to it's brother function
 * intarr_to_ip. If you're looking for the bitwise shifting... it's there.
 *
 * Returns the IP as an unsigned int, or NULL (0x0) if it failed.
 */

unsigned int str_to_ip(char* the_string){

  if (the_string != NULL){

    unsigned int ip[IPV4_LEN] = {0x0, 0x0, 0x0, 0x0};
    char copy[80];
    strncpy(copy, the_string, 80);
    char* token;
    int i = 0;
    bool failed = false;

    token = strtok(copy, "."); 
    for (i = 0; i < IPV4_LEN; i++){
      // Seperates numbers between dots.
      // If the number exists.
      if (token != NULL){
        
        // Adds it to the array.
        sscanf(token, "%u", &ip[i]);
      }
      else {
        fprintf(stderr, "Invalid IP adress entered.\n");
        failed = true;
        break;
      }

      token = strtok(NULL, "."); 
    }

    // IP was only partially found.
    if (failed){
      // Sets them all back to zero.
      for (int i = 0; i < IPV4_LEN; i++){
        ip[i] = 0x0;
      }
    }
    return intarr_to_ip(ip);
  }
  return 0x0;
}

/**
 * Turns an array of 4 integers into an IP address. 
 * Grabs each integer, and shifts the bits accordingly.
 * Then it uses bitwise OR to combine it with the all 0 
 * integer to preserve the data.
 *
 * Returns the integer with the proper combination, and NULL (0x0) if it failed.
 */
unsigned int intarr_to_ip(unsigned int the_ip[IPV4_LEN]){

  unsigned int ip_out = 0x0;
  int i = 0;
  bool failed = false;
  // Loops through each value.
  for (i = 0; i <  IPV4_LEN; i++){
    // If the IP is within range.
    if (the_ip[i] < 256 && the_ip[i] >= 0){
      // Shifts the bits by the appropriate amount.
      // Adds them to the ip.
      ip_out |= the_ip[i] << (24 - i * 8); 
    }
    else {
      fprintf(stderr, "Invalid IP adress entered.\n");
      failed = true;
      break;
    }
  }

  // If IP was not entered correctly.
  if (failed){

    ip_out = 0x0;
  }

  return ip_out;
}

// From a mask length (in bits) this returns the IP mask.
unsigned int build_ip_mask(int mask_size){

  unsigned int mask = 0x0;

  if (mask_size > 0 && mask_size <= (IPV4_LEN * 8)){

    // Take the complement.
    mask = ~mask;
    // Shift it over some number of bits.
    mask = mask << ((IPV4_LEN * 8) - mask_size);
  }
  return mask;
}

/*
 * This function pulls in a SINGLE LINE and creates the medata, and
 * space for the according table. It should be in the following format
 *
 * hostname.host 255.255.255.255 1
 *
 * Where the first item is the host name, then the host IP, and finally
 * the number of subnet entries in the table.
 */
fwd_table_t* init_table_from_str(char* the_string){

  fwd_table_t* table;
  char* token;
  char delim[] = " ";

  if (the_string != NULL){

    // So the original is not destroyed.
    char* string_copy = calloc(80, sizeof(char));
    strncpy(string_copy, the_string, 80);

    // Initialize the new table.
    table = fwd_table_init();

    // Take the first token (The hostname)
    token = strtok(the_string, delim);
    strncpy(table->hostname, token, 80);

    // Store the host IP (it will be tokenized after).
    token = strtok(NULL, delim);
    char* host_ip = token;

    // Get the number of entries.
    token = strtok(NULL, delim);
    if (token != NULL){
      table->size = atoi(token);
    }

    // Allocate the according space for the list of entries.
    table->subnets = calloc(table->size, sizeof(subnet_t*));
    
    // convert the host ip to int ip form.
    // this has to be done after because it also
    // uses strtok.
    table->ip_host = str_to_ip(host_ip);
    free(string_copy);
  }
  return table;
}

/*
 * Initializes a subnet from a formatted string.
 * This format is given in the README
 */
subnet_t* init_subnet_from_str(char* the_string){

  subnet_t* subnet;
  char* token;

  if (the_string != NULL){

    subnet = subnet_init();
    // So the original is not destroyed.
    char* string_copy = calloc(80, sizeof(char));
    strncpy(string_copy, the_string, 80);
    
    // Get subnet IP
    token = strtok(string_copy, "/");
    char* subnet_ip = token;

    // Get the mask sze.
    token = strtok(NULL, " ");
    int mask_size = atoi(token);

    // Get forward IP
    token = strtok(NULL, " ");
    char* forward_ip = token;

    subnet->ip_subnet = str_to_ip(subnet_ip);
    subnet->ip_forward = str_to_ip(forward_ip);
    subnet->ip_mask = build_ip_mask(mask_size);

    free(string_copy);
  }
  return subnet;
}

// Applys the subnet mask.
unsigned int apply_mask(unsigned int ip_check, unsigned int ip_mask){

  return ip_check & ip_mask;
}
