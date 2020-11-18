// Module Name: Server.c   
//   
// Description:   
//    This example illustrates a simple TCP server that accepts    
//    incoming client connections. Once a client connection is    
//    established, a thread is spawned to read data from the    
//    client and echo it back (if the echo option is not    
//    disabled).   
//   
// Compile:   
//    cl -o Server Server.c ws2_32.lib   
//   
// Command line options:   
//    server [-p:x] [-i:IP] [-o]   
//           -p:x      Port number to listen on   
//           -i:str    Interface to listen on   
//           -o        Receive only, don't echo the data back   
//   
#include <winsock2.h>   
#include <windows.h>
#include <io.h>

#include <stdio.h>   
#include <stdlib.h>   
   
#define DEFAULT_PORT        9999   
#define DEFAULT_BUFFER      4096   
#define DEFAULT_IAP_BIN_PATH  ""
   
int    iPort      = DEFAULT_PORT; // Port to listen for clients on   
BOOL   bInterface = FALSE,   // Listen on the specified interface   
       bRecvOnly  = FALSE;   // Receive data only; don't echo back   
char   szAddress[128] = "127.0.0.1";       // Interface to listen for clients on   
char    runPath[MAX_PATH];
char    binPath[MAX_PATH];

static void soft_sleep(void)
{
    for (int i = 0; i < 1000 * 1000; i++)
        for (int j = 0; j < 1000; j++);
}

//   
// Function: usage   
//   
// Description:   
//    Print usage information and exit   
//   
void usage()   
{   
    printf("usage: server [-p:x] [-i:IP] [-o]\n\n");   
    printf("       -p:x      Port number to listen on\n");   
    printf("       -i:str    Interface to listen on\n");   
    printf("       -o        Don't echo the data back\n\n");   
    ExitProcess(1);   
}   
     
//   
// Function: ValidateArgs   
//   
// Description:   
//    Parse the command line arguments, and set some global flags    
//    to indicate what actions to perform   
//   
void ValidateArgs(int argc, char **argv)   
{   
    int i;   
   
    for(i = 1; i < argc; i++)   
    {   
        if ((argv[i][0] == '-') || (argv[i][0] == '/'))   
        {   
            switch (tolower(argv[i][1]))   
            {   
                case 'p':   
                    iPort = atoi(&argv[i][3]);   
                    break;   
                case 'i':   
                    bInterface = TRUE;   
                    if (strlen(argv[i]) > 3)   
                        strcpy(szAddress, &argv[i][3]);   
                    break;   
                   case 'o':   
                   bRecvOnly = TRUE;   
                       break;   
                default:   
                    usage();   
                    break;   
            }   
        }   
    }   
}   
   
//   
// Function: ClientThread   
//   
// Description:   
//    This function is called as a thread, and it handles a given   
//    client connection.  The parameter passed in is the socket    
//    handle returned from an accept() call.  This function reads   
//    data from the client and writes it back.   
//   
DWORD WINAPI ClientThread(LPVOID lpParam)   
{   
    SOCKET        sock=(SOCKET)lpParam;   
    char          szBuff[DEFAULT_BUFFER];
    int ret;

    while(1)   
    {   
        // Perform a blocking recv() call   
        //   
        ret = recv(sock, szBuff, DEFAULT_BUFFER, 0);   
        if (ret == 0)        // Graceful close   
            break;   
        else if (ret == SOCKET_ERROR)   
        {   
            printf("recv() failed: %d\n", WSAGetLastError());   
            break;   
        }   
        szBuff[ret] = '\0';   
        printf("RECV: '%s'\n", szBuff);   
           
        // If we selected to echo the data back, do it       
        if (!strncmp("WantUpdate", szBuff, 10))
        {
            Sleep(3*1000);
            //Get current path and check update_bin exist?        
            printf("Check path %s.", binPath);
            if(!access(binPath, 0))
                printf("file existed.\n");
            else
                printf("file doesn't exist.\n");

            //open bin file
            char rBuf[2048];
            int  rsz, nLeft, idx, fileSize;

            rsz = idx = nLeft = fileSize = 0;
            FILE *binStream = fopen(binPath, "rb+");

            do{
                rsz = fread(rBuf, 1, 1024, binStream);
                // Make sure we write all the data
                fileSize += rsz;
                nLeft = rsz;
                idx = ret = 0;
                while(nLeft > 0)   
                {   
                    ret = send(sock, &rBuf[idx], nLeft, 0);   
                    if (ret == 0)   
                        break;   
                    else if (ret == SOCKET_ERROR)   
                    {   
                        printf("send() failed: %d\n",    
                            WSAGetLastError());   
                        break;   
                    }   
                    nLeft -= ret;   
                    idx += ret;   
               }
               soft_sleep();
            } while (rsz >= 1024);

            printf("file size %d ?\n", fileSize);
            fclose(binStream);
        }   
    }   
    return 0;   
}   
   
//   
// Function: main   
//   
// Description:   
//    Main thread of execution. Initialize Winsock, parse the   
//    command line arguments, create the listening socket, bind   
//    to the local address, and wait for client connections.   
//   
int main(int argc, char **argv)   
{   
    WSADATA       wsd;   
    SOCKET        sListen,   
                  sClient;   
    int           iAddrSize;   
    HANDLE        hThread;   
    DWORD         dwThreadId;   
    struct sockaddr_in local,   
                       client;   
   
    ValidateArgs(argc, argv);   
    if (WSAStartup(MAKEWORD(2,2), &wsd) != 0)   
    {   
        printf("Failed to load Winsock!\n");   
        return 1;   
    }   

    //get run path
    GetModuleFileName(NULL, runPath, MAX_PATH-16);          
    strncpy(binPath, runPath, strlen(runPath));
    binPath[strlen(binPath) - 6] = '\0'; // delete str ts.exe
    strcat(binPath, "UPDATE.BIN");

    // Create our listening socket   
    //   
    sListen = socket(AF_INET, SOCK_STREAM, IPPROTO_IP);   
    if (sListen == SOCKET_ERROR)   
    {   
        printf("socket() failed: %d\n", WSAGetLastError());   
        return 1;   
    }   
    // Select the local interface and bind to it   
    //   
    if (bInterface)   
    {   
        local.sin_addr.s_addr = inet_addr(szAddress);   
        if (local.sin_addr.s_addr == INADDR_NONE)   
            usage();   
    }   
    else   
        local.sin_addr.s_addr = htonl(INADDR_ANY);   
    local.sin_family = AF_INET;   
    local.sin_port = htons(iPort);   
   
    if (bind(sListen, (struct sockaddr *)&local,    
            sizeof(local)) == SOCKET_ERROR)   
    {   
        printf("bind() failed: %d\n", WSAGetLastError());   
        return 1;   
    }   
    listen(sListen, 8);   
    //   
    // In a continous loop, wait for incoming clients. Once one    
    // is detected, create a thread and pass the handle off to it.   
    //   
    while (1)   
    {   
        iAddrSize = sizeof(client);   
        sClient = accept(sListen, (struct sockaddr *)&client,   
                        &iAddrSize);           
        if (sClient == INVALID_SOCKET)   
        {           
            printf("accept() failed: %d\n", WSAGetLastError());   
            break;   
        }   
        printf("Accepted client: %s:%d\n",    
            inet_ntoa(client.sin_addr), ntohs(client.sin_port));   
   
        hThread = CreateThread(NULL, 0, ClientThread,    
                    (LPVOID)sClient, 0, &dwThreadId);   
        if (hThread == NULL)   
        {   
            printf("CreateThread() failed: %d\n", (int)GetLastError());   
            break;   
        }   
        CloseHandle(hThread);   
    }   
    closesocket(sListen);   
       
    WSACleanup();   
    return 0;   
}   