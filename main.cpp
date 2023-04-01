#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <pthread.h>

void *handle_client(void *arg);

int main(int argc, char *argv[])
{
  int server_socket, client_socket, addr_len, *new_sock;
  struct sockaddr_in server_addr, client_addr;

  // Create socket
  server_socket = socket(AF_INET, SOCK_STREAM, 0);
  if (server_socket == -1)
  {
    printf("Error creating socket\n");
    exit(EXIT_FAILURE);
  }

  // Set server address and port
  server_addr.sin_family = AF_INET;
  server_addr.sin_addr.s_addr = INADDR_ANY;
  server_addr.sin_port = htons(8888);

  // Bind socket to address and port
  if (bind(server_socket, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0)
  {
    printf("Error binding socket\n");
    exit(EXIT_FAILURE);
  }

  // Listen for incoming connections
  listen(server_socket, 5);
  printf("Waiting for incoming connections...\n");

  // Accept incoming connections and create new threads for each client
  while (1)
  {
    addr_len = sizeof(struct sockaddr_in);
    client_socket = accept(server_socket, (struct sockaddr *)&client_addr, (socklen_t *)&addr_len);
    printf("New client connected: %s:%d\n", inet_ntoa(client_addr.sin_addr), ntohs(client_addr.sin_port));

    new_sock = (int *)malloc(1);
    *new_sock = client_socket;

    pthread_t client_thread;
    if (pthread_create(&client_thread, NULL, handle_client, (void *)new_sock) < 0)
    {
      printf("Error creating thread\n");
      exit(EXIT_FAILURE);
    }
  }

  return 0;
}

void *handle_client(void *arg)
{
  int client_socket = *(int *)arg;
  char buffer[1024] = {0};
  ssize_t num_bytes;

  // Read data from client
  while ((num_bytes = read(client_socket, buffer, sizeof(buffer))) > 0)
  {
    // Process data here...
    printf("Received %ld bytes: %s\n", num_bytes, buffer);

    // Send response back to client
    write(client_socket, "pio", 3);

    memset(buffer, 0, sizeof(buffer));
  }

  // Close socket and free memory
  close(client_socket);
  free(arg);
  pthread_exit(NULL);
}
