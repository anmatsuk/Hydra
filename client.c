#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>

#define	PORT 4141

char* scan_line(char* buffer, int buffer_size) {
   char		*p = buffer;
   int		count = 0;
   char 	c;
   do {
       scanf("%c", &c);
       if (c == '\r' || c == '\n' || c == 0 || c == EOF) {
           *p = 0;
           break;
       }
       *p++ = c; // add the valid character into the buffer
   } while (count < buffer_size - 1);  // don't overrun the buffer
   buffer[buffer_size - 1] = 0;
   return buffer;
}

int main()
{
int socket_desc, val;
struct sockaddr_in client_addr;
char *buffer;

buffer = (char*)malloc(256 * sizeof(char));
bzero(buffer,256);
socket_desc = socket(AF_INET, SOCK_STREAM, 0);

client_addr.sin_family = AF_INET;
client_addr.sin_addr.s_addr =  INADDR_ANY; 
client_addr.sin_port = htons(PORT);

if(connect(socket_desc, (struct sockaddr*)&client_addr, sizeof(client_addr)) == 0)
	printf("CONNECT STATE: Connected to Server on port %d\n", PORT);
else
	printf("Connection to server failed !\n");
write(socket_desc,buffer,0);
read(socket_desc,buffer,255);
printf("Message from server: %s",buffer);
while(1) 
{
	printf("Message to server: ");
	bzero(buffer,256);
	buffer = scan_line(buffer, 256);
	write(socket_desc,buffer,strlen(buffer));
	bzero(buffer,256);
	
	read(socket_desc,buffer,255);
	if (strlen(buffer) != 1)
		printf("Message from server: %s\n",buffer);
}
	close(socket_desc);
	free(buffer);
	return 0;	
}
