#include <stdio.h>
#include <stdlib.h>
#include <strings.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>

#define PORT 63300
#define BUFFER_SIZE 1024
#define ERROR_CODE -1
#define MAX_CONNECTIONS 3


int setup_server(int port)
{
    struct sockaddr_in6 server_addr;
    int server_fd; // file descriptor
    server_fd = socket(AF_INET6, SOCK_STREAM, 0);
    
    if (server_fd == ERROR_CODE)
    {
        perror("Erro ao criar o socket.");
        exit(EXIT_FAILURE);
    }

    bzero(&server_addr, sizeof(server_addr));
    server_addr.sin6_family = AF_INET6;
    server_addr.sin6_addr = in6addr_any;
    server_addr.sin6_port = htons(port);

    if (bind(server_fd, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0)
    {
        perror("Falha ao bindar.");
        exit(EXIT_FAILURE);
    }

    if (listen(server_fd, MAX_CONNECTIONS) < 0)
    {
        perror("Falhar no listen");
        exit(EXIT_FAILURE);
    }

    printf("Servidor configurado com sucesso na porta: %d\n", port);
    return server_fd;
}

void handle_connection(int client_id)
{
    char buffer[BUFFER_SIZE];
    ssize_t bytes_lidos;

    while((bytes_lidos = read(client_id, buffer, BUFFER_SIZE)) > 0)
    {
        printf("Recebido %zd bytes do cliente: %.*s\n", bytes_lidos, (int)bytes_lidos, buffer);
        write(client_id, buffer, bytes_lidos);
    }

    close(client_id);
}

int main()
{
    int server_fd = setup_server(PORT);

    while(1)
    {
        int client_fd = accept(server_fd, NULL, NULL);

        if (client_fd < 0)
        {
            perror("Erro, ignorando conexão");
            continue;
        }

        printf("Cliente conectado! Chamando handler...\n");
        handle_connection(client_fd);

        printf("Handler finalizado. Conexão fechada com o cliente.\n");
    }
    return 0;
}