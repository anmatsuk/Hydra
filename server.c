#include "socket.h"



static void	info(char *message)
{
	FILE *fd = NULL;
	if (fd)
	{
		fprintf(fd, "%s\n", message);
		fflush(fd);
	}
	else
		printf("%s\n", message);
	return ;
}

void *thread_init(void *socket_dest)
{
	char	*message, client_message[256];
	int		sock, read_size;

	message = "Hey , ping me...=)\n";
	sock = *(int*)socket_dest;
	write(sock , message , strlen(message));

	while( (read_size = recv(sock , client_message , 2000 , 0)) > 0 )
    {
        *(client_message + read_size) = '\0';
        printf("Client ( %d ) message :   ", sock);

        info(client_message);
        if (strcmp(client_message, "ping") == 0)
        	write(sock , "pong pong" , 9);
        else
        	write(sock , "\n" , 1);
    }
    if(read_size == 0)
    {
        printf("Client ( %d ) disconnected ... \n", sock);
        fflush(stdout);
    }
    else if(read_size == -1)
		info("Recieving failed ... \n");
	if (socket_dest)
	{
		close(sock);
		free(socket_dest);
	}
	return (0);
}


int		start_server(int process_id)
{
	struct	sockaddr_in server, client;
	int		server_fd, client_fd,  *new_sock;
	int		size;
	
	if (process_id == 0)
	{
		FILE 	*fp = NULL;
		fp = fopen ("Log.txt", "w+");
		fprintf(fp, "Logging info...\n");
		fflush(fp);
		fclose(fp);
			
	}
	//fprintf(fp, "Logging info...\n");
	//fflush(fp);
	server_fd = socket(AF_INET , SOCK_STREAM , 0);
	if (server_fd == -1)
	{
		info("Could not create server socket ...\n");
		exit (1);
	}
	info("Socket created.\n");

	server.sin_family = AF_INET;
	server.sin_addr.s_addr = INADDR_ANY;
	server.sin_port = htons(PORT);
	if (bind(server_fd, (struct sockaddr *)&server, sizeof(server)) < 0)
	{
		info("Bind failed ...\n");
		return (1);
	}
	info("Bind done.\n");
	listen(server_fd, 3); //backlog 3 - number of max connections for the socket
	info("Waiting for incoming connections ... \n");
	size = sizeof(struct sockaddr_in);

	while((client_fd = accept(server_fd, (struct sockaddr *)&client, (socklen_t *)&size)))
	{
		info("Connection accepted.\n");
		pthread_t sniffer_thread;
        new_sock = malloc(1);
        *new_sock = client_fd;
         
        if( pthread_create( &sniffer_thread , NULL ,  thread_init , (void*) new_sock) < 0)
        {
            info("Could not create thread ...\n");
            return 1;
        }
	}
	//fclose(fd);
	if (client_fd < 0)
	{
		info("Acception failed ... ");
		return (1);
	}
	return (0);
}

int 	main(int argc, char const *argv[])
{
	
	int process_id, sid;
	
	if (argc == 2 && !strcmp(argv[1],"-D"))
	{
		process_id = fork();
		if (process_id < 0)
		{
			printf("Fork failed ...\n");
			exit(1);
		}
		else if (process_id > 0) //child process
		{
			printf("process_id of child process %d \n", process_id);
			exit (0);
		}
		else
		{
			//unmask the file mode
			umask(0);
			//set new session
			sid = setsid();
			if(sid < 0)
			{
			// Return failure
			exit(1);
			}
			// Change the current working directory to root.
			chdir("/");
			// Close stdin. stdout and stderr
			close(STDIN_FILENO);
			close(STDOUT_FILENO);
			close(STDERR_FILENO);
		}
	}
	return (start_server(process_id));
}