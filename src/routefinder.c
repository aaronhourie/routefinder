#include <stdio.h>
#include "../hdr/fwd_table.h"

int main (int argc, char** argv){


  char* hostname = malloc(16);
  fwd_table_t* table = fwd_table_init(hostname, 10);

  for (int i = 0; i < 10; i++){

    subnet_t* subnet = subnet_init();
    table->subnets[i] = subnet;
  }

  unsigned int ip = str_to_ip(argv[1]);

  printf("IP: %08x\n", ip);
  fwd_table_destroy(table);

  return 0;
}
