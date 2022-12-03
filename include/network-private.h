// Grupo 58
// Luís Santos 56341
// Afonso Aleluia 56363
// Daniel Marques 56379

#ifndef _NETWORK_PRIVATE_H
#define _NETWORK_PRIVATE_H

void network_pipe_close(int);

void network_abort(int);

int network_zookeeper_init(char* address, int port);

#endif