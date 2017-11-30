#include<stdio.h>
#include<string.h>    //strlen
#include<stdlib.h>    //strlen
#include<sys/socket.h>
#include<arpa/inet.h> //inet_addr
#include<unistd.h>    //write
#include<pthread.h> //for threading , link with lpthread
#include "mcrypt.h"
#include <openssl/md5.h>

#include <math.h>
#include <stdint.h>
// function prototypes
void *connection_handler(void *);
char *str2md5(const char*,int);
// Save number_of_client which are connected to the server until now.
int number_of_client=0;
// This function return MD5 of a string.
char *str2md5(const char *str, int length) {
  fflush(stdout);
    int n;
    MD5_CTX c;
    unsigned char digest[16];
    char *out = (char*)malloc(33);

    MD5_Init(&c);

    while (length > 0) {
        if (length > 512) {
            MD5_Update(&c, str, 512);
        } else {
            MD5_Update(&c, str, length);
        }
        length -= 512;
        str += 512;
    }

    MD5_Final(digest, &c);

    for (n = 0; n < 16; ++n) {
        snprintf(&(out[n*2]), 16*2, "%02x", (unsigned int)digest[n]);
    }

    return out;
}
// This is a main function. The project start from here!
int main(int argc , char *argv[])
{
    int socket_desc , client_sock , c , *new_sock;
    struct sockaddr_in server , client;

    //Create socket
    socket_desc = socket(AF_INET , SOCK_STREAM , 0);
    if (socket_desc == -1)
    {
        printf("Could not create socket");
    }
    puts("Socket created");

    //Prepare the sockaddr_in structure
    server.sin_family = AF_INET;
    server.sin_addr.s_addr = INADDR_ANY;
    server.sin_port = htons( 6722 );

    //Bind
    if( bind(socket_desc,(struct sockaddr *)&server , sizeof(server)) < 0)
    {
        //print the error message
        perror("bind failed. Error");
        return 1;
    }
    puts("bind done");

    //Listen
    listen(socket_desc , 3);

    //Accept and incoming connection
    puts("Waiting for incoming connections...");
    c = sizeof(struct sockaddr_in);


    //Accept and incoming connection
    puts("Waiting for incoming connections...");
    c = sizeof(struct sockaddr_in);
    while( (client_sock = accept(socket_desc, (struct sockaddr *)&client, (socklen_t*)&c)) )
    {
        puts("Connection accepted");

        pthread_t sniffer_thread;
        new_sock = malloc(1);
        *new_sock = client_sock;
        // Here I created a thread for each client that want to join  to server.
        // The function of thread is connection_handler.
        if( pthread_create( &sniffer_thread , NULL ,  connection_handler , (void*) new_sock) < 0)
        {
            perror("could not create thread");
            return 1;
        }

        puts("Handler assigned");
    }

    if (client_sock < 0)
    {
        perror("accept failed");
        return 1;
    }

    return 0;
}

// This is myappend function to append a char to a char*
 void myappend(char* s, char c) {
        int len = strlen(s);
        s[len] = c;
        s[len+1] = '\0';
}
// This is an encrypt function
int encrypt(
    void* buffer,
    int buffer_len, /* Because the plaintext could include null bytes*/
    char* IV,
    char* key,
    int key_len
){
  MCRYPT td = mcrypt_module_open("rijndael-128", NULL, "cbc", NULL);
  int blocksize = mcrypt_enc_get_block_size(td);
  if( buffer_len % blocksize != 0 ){return 1;}

  mcrypt_generic_init(td, key, key_len, IV);
  mcrypt_generic(td, buffer, buffer_len);
  mcrypt_generic_deinit (td);
  mcrypt_module_close(td);

  return 0;
}
//This function is just for my test. it display the ciphertext.
void display(char* ciphertext, int len){
  int v;
  for (v=0; v<len; v++){
    printf("%d ", ciphertext[v]);
  }
  printf("\n");
}
//This function return the number of digit in an Integer variable
int countDigits( int value )
{
  int result = 0;
  if(value<0)
  {
    result++;
    value=-value;
  }

    while( value != 0 ) {
       value /= 10;
       result++;
    }
    return result;
}
// This function concat two char* .
char* myconcat(char* str1, char* str2){
  char * str3 = (char *) malloc(1 + strlen(str1)+ strlen(str2) );
      strcpy(str3, str1);
      strcat(str3, str2);
      return str3;
}
/*
 * This will handle connection for each client
 * */
void *connection_handler(void *socket_desc)
{
    //Get the socket descriptor
    int sock = *(int*)socket_desc;
    int read_size;
    char *message , client_message[2000];

    //Send some messages to the client


//Send Client ID to the client
number_of_client++;
puts("*************************************************************");
int number = number_of_client;
char buffer [32];
int ret = snprintf(buffer, sizeof(buffer), "%d", number);
char * num_string = buffer;
//String terminator is added by snprintf
int ik=strlen(num_string);
num_string[ik]='\n';

printf("Client number %s Joined!",num_string );

write(sock , num_string , strlen(num_string));
    //Receive a message from client
    int first_message=1;
    char* md5_calculated;
    while( (read_size = recv(sock , client_message , 2000 , 0)) > 0 )
    {
        //Send the message back to client

        if(first_message==1)
        {
          puts("First Message recieved");
          // This is the first message Client send to server
          // This message is original string.
          first_message=0;
          write(sock , client_message , strlen(client_message));
          puts("Responsed First message");
          client_message[strlen(client_message)-1] = '\0';
          // Here MD5 is calculated.
          md5_calculated=str2md5(client_message,strlen(client_message));
        //  puts(md5_calculated);

      }
      else if(first_message==0){
        // This is the second message that server recieved.
        puts("Second Message recieved");

        char*filenameinput;
        num_string[strlen(num_string)-1] = '\0';
        myappend(num_string,'.');
          myappend(num_string,'t');
            myappend(num_string,'x');
              myappend(num_string,'t');

// Here we try to open the File which Client put the key there
        FILE *stream;
	char *line = NULL;
	size_t len = 0;
	ssize_t read;

	stream = fopen(num_string, "r");
	if (stream == NULL)
		exit(EXIT_FAILURE);

	while ((read = getline(&line, &len, stream)) != -1) {
		//printf("Retrieved line of length %zu :\n", read);
		//printf("%s", line);
	}
fclose(stream);
	char* IV = "AAAAAAAAAAAAAAAA";
  MCRYPT td, td2;
  int keysize = 16;
  char* buffer;
  int buffer_len = 16;
  buffer = calloc(1, buffer_len);
  strncpy(buffer, md5_calculated, buffer_len);
  //encrypt the MD5 by key.
  encrypt(buffer, buffer_len, IV, line, keysize);
  //Next line was for my testing
// display(buffer , buffer_len);
int w=0;
char* reschar="";
// Here I try to concat all of encryption code...
for (int v=0; v<buffer_len; v++){
  int temp=(int)buffer[v];
  int number_of_digit=countDigits(temp);

char str[number_of_digit];
sprintf(str, "%d", temp);
reschar=myconcat(reschar,str);
}
// The result which should be send to the Client
        char* result="False\n";
        size_t destination_size = sizeof (client_message);

        client_message[destination_size - 1] = '\0';
        int ik=strlen(reschar);
        reschar[ik]='\n';
        // check two char*
        if(strcmp(client_message,reschar)==0)
        { result="True\n";}
        printf("The Result was %s\n",result );
        puts("Responsed Second message");

        write(sock , result , strlen(result));

        free(line);

      }

    }

    if(read_size == 0)
    {
        puts("Client disconnected");
        fflush(stdout);
    }
    else if(read_size == -1)
    {
        perror("recv failed");
    }

    //Free the socket pointer
    free(socket_desc);
    puts("*************************************************************");

    return 0;
}
