#include "../hdr/fwd_table.h"

/* 
 * Initializes a forwarding table. Requires that the host name be not NULL.
 * @param hostname - the "owner" host's human-readable name
 * @param size - the number of entries in the forwarding table.
 *
 * @return - the generated table, or NULL if the table failed to generate.
 */
fwd_table_t* fwd_table_init(char* hostname, int size){

  fwd_table_t* the_table;
  if (hostname != NULL){
    the_table = malloc(sizeof(fwd_table_t));
    the_table->hostname = hostname;
    the_table->size = size;
    the_table->subnets = calloc(size, sizeof(subnet_t));
  }
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
  }

  for (int i = 0; i < the_table->size; i++){
    subnet_destroy(the_table->subnets[i]);
  }

  free(the_table->subnets);

  free(the_table);
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

unsigned int str_to_ip(char* the_string){

  if (the_string != NULL){

    unsigned int ip[IPV4_LEN] = {0x0, 0x0, 0x0, 0x0};
    char* token;
    int i = 0;

    token = strtok(the_string, "."); 
    for (i = 0; i < IPV4_LEN; i++){
      // Seperates numbers between dots.
      // If the number exists.
      if (token != NULL){
        
        // Adds it to the array.
        sscanf(token, "%u", &ip[i]);
      }
      else {
        fprintf(stderr, "Invalid IP adress entered.");
        break;
      }

      token = strtok(NULL, "."); 
    }

    // IP was only partially found.
    if (i < IPV4_LEN - 1){
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
 */
unsigned int intarr_to_ip(unsigned int the_ip[IPV4_LEN]){

  unsigned int ip_out = 0x0;
  int i = 0;
  // Loops through each value.
  for (i = 0; i <  IPV4_LEN; i++){
    // If the IP is within range.
    if (the_ip[i] < 256 && the_ip[i] >= 0){
      // Shifts the bits by the appropriate amount.
      ip_out |= the_ip[i] << (24 - i * 8); 
      // Adds them to the ip.
    }
    else {
      fprintf(stderr, "Invalid IP adress entered.");
      break;
    }
  }

  // If IP was not entered correctly.
  if (i < IPV4_LEN - 1){

    ip_out = 0x0;
  }

  return ip_out;
}
