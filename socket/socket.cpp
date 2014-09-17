// socket.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"

// declare functions
static void error_exit(char *errorMessage);
static void connect_port(char *ip, int source_port, int port);
static void run_connect(char *ip, int source_start_port, int source_end_port, int destination_port);

/**
 * @brief handle error
 *
 * Print error and exit
 * 
 * @param char errorMessage Error Message
 */
static void error_exit(char *errorMessage) 
{

  fprintf(stderr,"%s: %d\n", errorMessage, WSAGetLastError());

  exit(EXIT_FAILURE);
}

/**
 * @brief connect
 *
 * Connect to IP:Port and close it.
 * 
 * @param char ip           IP-Address
 * @param int  source_port  source Portnumber
 * @param int  port         Portnumber
 * @param int  wait               wait in ms after connect
 */
static void connect_port(char *ip, int source_port, int port, int wait) 
{
  struct sockaddr_in server, localhost;
  struct hostent *host_info;
  unsigned long addr;
  
  SOCKET sock;
  
  // Initialisiere TCP for Windows ("winsock").
  WORD wVersionRequested;
  WSADATA wsaData;
  wVersionRequested = MAKEWORD (1, 1);
  if (WSAStartup (wVersionRequested, &wsaData) != 0)
      error_exit( "Fehler beim Initialisieren von Winsock");


  // generate the Socket.
  sock = socket( AF_INET, SOCK_STREAM, IPPROTO_TCP);

  if (sock < 0)
      error_exit( "Fehler beim Anlegen eines Sockets");

  // generate socketaddress
  memset( &server, 0, sizeof (server));
  if ((addr = inet_addr(ip)) != INADDR_NONE) {
    memcpy( (char *)&server.sin_addr, &addr, sizeof(addr));
  } else {
    // convert DNSName to IP-Address
    host_info = gethostbyname(ip);
    if (NULL == host_info)
        error_exit("Unbekannter Server");
    memcpy( (char *)&server.sin_addr, host_info->h_addr, host_info->h_length );
  }

  // IPv4-Connection and portnumber
  server.sin_family = AF_INET;
  server.sin_port = htons(port);

  localhost.sin_family = AF_INET;
  localhost.sin_port = htons(source_port);
  localhost.sin_addr.s_addr = htonl (INADDR_ANY);
  
  if(bind(sock,(SOCKADDR *)&localhost,sizeof(localhost)) == SOCKET_ERROR) 
    printf("%s:%i - no bind\n", inet_ntoa(localhost.sin_addr), source_port);

  // connect to server
  if(connect(sock,(SOCKADDR *)&server,sizeof(server)) == SOCKET_ERROR) {
    printf("%s:%i", inet_ntoa(localhost.sin_addr), source_port);
    printf(" -> ");
    printf("%s:%i", inet_ntoa(server.sin_addr), port);
    printf(" - can not connect to server\n");
  } else {
    printf("%s:%i - press enter to close\n", inet_ntoa(localhost.sin_addr), port);
    std::cin.ignore();
    printf("%s:%i", inet_ntoa(localhost.sin_addr), source_port);
    printf(" -> ");
    printf("%s:%i", inet_ntoa(server.sin_addr), port);
    printf(" + close connection to server\n");
  }

  // close connection and socket and cleanup winsock
  closesocket(sock);
  WSACleanup();
  Sleep(wait);
}

/**
 * @brief run connect
 *
 * Run the connect to IP:Port from special source port and close it.
 * 
 * @param char ip                 IP-Address
 * @param int  source_start_port  start source Portnumber
 * @param int  source_end_port    end source Portnumber
 * @param int  destination_port   Destination Portnumber
 * @param int  wait               wait in ms after connect
 */
static void run_connect(char *ip, int source_start_port, int source_end_port, int destination_port, int wait) 
{
  int i;

  if (source_start_port <= source_end_port) {
    for (i = source_start_port; i <= source_end_port; i++) {
      connect_port(ip, i, destination_port, wait);
    }
  } else {
    connect_port(ip, source_start_port, destination_port, wait);
  }
}

int main(int argc, char *argv[])
{
  int source_start_port, source_end_port, end_port, start_port, i, wait;
  
  if (argc == 7 || argc == 6) {
    //printf("argc = %i \n", (char) argc);
    //for(i=0; i < argc; i++) {
    //  printf("argv[%d] = %s ", i, argv[i]);
    //  printf("\n");
    //}
  } else {
    printf("\nParameters:\n");
    printf("wait_in_ms IP_address source_start_port source_end_port destination_start_port [destination_end_port]\n\n");
    printf("wait_in_ms as e.g. 100 for 100ms wait after connect and close\n");
    printf("IP_address as e.g. 127.0.0.1\n");
    printf("\n\n");
    return EXIT_SUCCESS;
  }

  wait = atoi(argv[1]);
  source_start_port = atoi(argv[3]);
  source_end_port = atoi(argv[4]);
  start_port = atoi(argv[5]);

  printf("Close Connection form IP %s\n", argv[2]);

  if (argc == 7) {
    end_port = atoi(argv[6]);
    
    if (start_port <= end_port) {
      for (i = start_port; i <= end_port; i++) {
        run_connect(argv[2], source_start_port, source_end_port, i, wait);
      }
      
      return EXIT_SUCCESS;
    }
  } 
  
  run_connect(argv[2], source_start_port, source_end_port, start_port, wait);
  
  return EXIT_SUCCESS;
}

