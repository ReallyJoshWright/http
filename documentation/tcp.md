# TCP Documentation

## Info
- a tcp connection can stay open or close after receiving message

## Steps for a TCP server
1. Create an IPv4 TCP socket
2. Set the option on the socket to have a reusable IPv4 address and port
3. Define the address and port information
4. Bind the address and port information to the socket
5. Prepare the socket to listen on the address and port it is bound to
6. Wait to accept incoming connections and create a new client socket with addr
7. (Optionally), convert and print out the client address and port
8. Close the IPv4 TCP socket for server and all clients

## Description for a TCP server
```c++
int server_fd = socket(AF_INET, SOCK_STREAM, 0);
```

This creates a socket and returns an integer that is a file descriptor to the
socket that then needs to be closed. A file descriptor is an index or handle
that the operating system uses to refer to the socket. The socket is created
using IPv4 over TCP. socket() is a system call. A non negative integer means
that a socket was successfully created. A -1 means that it failed to create
the socket.

```c++
close(server_fd);
```

This tells the operating system to deallocate/reclaim the socket.
close() is a system call.

```c++
int opt = 1;
int is_success = setsockopt(
    server_fd,
    SOL_SOCKET,SO_REUSEADDR,
    (const char *)&opt, sizeof(opt)
);
```

setsockopt() is a system call that sets an option for the socket.
SOL_SOCKET defines the level, indicating that the option applies to the socket
layer itself. SO_REUSEADDR sets the option to reuse the IPv4 address and port
with another socket without having to wait for the TIME_WAIT period.
opt here is a boolean flag that sets the option to enabled. is_success, the
return value is 0 for success or -1 for failure. opt is passed as a pointer for
consistency with other options.

```c++
struct sockaddr_in address;
socklen_t addrlen = sizeof(address);
address.sin_family = AF_INET;
address.sin_addr.s_addr = INADDR_ANY;
address.sin_port = htons(port);
```

This creates a variable that is a struct and the size of that variable. The
address is set to be IPv4 and can listen on any and all addresses on the
server. The port is set and converted to bytes that the network requires.

```c++
int result = bind(server_fd, (struct sockaddr *)&address, sizeof(address));
```

This will return a -1 for failure and 0 for success. This binds the address
and port to the socket.

```c++
int result = listen(server_fd, 10);
```

This is another system call that puts the socket into a listening state where
it is ready to accept connections. The result can be a 0 for success or -1
for failure. 10 is the length or number of connections that can be in the
queue.

```c++
int client_socket = accept(server_fd, (struct sockaddr *)&address, &addrlen);
```

This is another system call that is usually in an infinite loop that accepts
connections and returns the client socket handle or -1 for failure. The
address is assigned the client's IPv4 address and port.

```c++
char client_ip[INET_ADDRSTRLEN];
inet_ntop(AF_INET, &(address.sin_addr), client_ip, INET_ADDRSTRLEN);
println(
    "Connection accepted from {}:{}",
    client_ip,
    ntohs(address.sin_port)
);
```

This pulls the client address information to display the address and port
in human readable format. On failure, it will return a null pointer. On
success, it returns client_ip.
