## Webserv

```
TCP program flow

A TCP client program must first know the TCP server's address. This is often input by a
user. In the case of a web browser, the server address is either input directly by the user
into the address bar, or is known from the user clicking on a link. The TCP client takes this
address (for example, http://example.com) and uses the getaddrinfo() function to
resolve it into a struct addrinfo structure. The client then creates a socket using a call to
socket(). The client then establishes the new TCP connection by calling connect(). At
this point, the client can freely exchange data using send() and recv().

A TCP server listens for connections at a particular port number on a particular interface.
The program must first initialize a struct addrinfo structure with the proper listening
IP address and port number. The getaddrinfo() function is helpful so that you can do
this in an IPv4/IPv6 independent way. The server then creates the socket with a call to
socket(). The socket must be bound to the listening IP address and port. This is
accomplished with a call to bind().

The server program then calls listen(), which puts the socket in a state where it listens
for new connections. The server can then call accept(), which will wait until a client
establishes a connection to the server. When the new connection has been established,
accept() returns a new socket. This new socket can be used to exchange data with the
client using send() and recv(). Meanwhile, the first socket remains listening for new
connections, and repeated calls to accept() allow the server to handle multiple clients.
```
text from ```Hands-On Network Programming with C - Lewis Van Winkle```, pages 48 (see books section)


---------------------------------------------------------------------------------------------------------------

Serveur http a partie de 0 : https://medium.com/from-the-scratch/http-server-what-do-you-need-to-know-to-build-a-simple-http-server-from-scratch-d1ef8945e4fa

Fichier de configuration nginx : https://www.plesk.com/blog/various/nginx-configuration-guide/

Selection du block nginx : https://www.digitalocean.com/community/tutorials/understanding-nginx-server-and-location-block-selection-algorithms-fr

autoindex : https://www.keycdn.com/support/nginx-directory-index

Directives nginx : http://nginx.org/en/docs/http/ngx_http_core_module.html#directives

Methods HTTP : https://developer.mozilla.org/fr/docs/Web/HTTP/Methods
et https://explorweb.github.io/cours2018/cours/commande_HTTP_Postman.html

client_max_body_sixe : https://linuxhint.com/what-is-client-max-body-size-nginx/

Methods a implementer : https://www.rfc-editor.org/rfc/rfc7231#section-4

Doc par rapport au projet : https://webserv42.notion.site/Webserv-cbb6ab4136ba4b4c8cb4f98109d5fc1f

-----------------------------------------------------------------------------------------------------

### FUNCTIONS
#### getaddrinfo
- https://linuxhint.com/c-getaddrinfo-function-usage/

### STRUCTS
#### sockaddr - sockaddr_in 
- https://blog.actorsfit.com/a?ID=00650-753e312f-e4a7-410f-b3e0-4604572635d2
---------------------------------------------------------------------------------------------------

### SOCKET
- https://www.howtogeek.com/devops/what-are-unix-sockets-and-how-do-they-work/
- https://developer.ibm.com/tutorials/l-sock/?mhsrc=ibmsearch_a&mhq=linux%20sockets%20 </br>
- https://beej.us/guide/bgnet/html/split/
- https://www.isi.edu/~hussain/TEACH/Spring2014/notes/Steven00a.pdf

##### close connection TCP | why use setsockopt | bind error: error msg already use 
- https://hea-www.harvard.edu/~fine/Tech/addrinuse.html

#### BYTE ORDER
- https://www.quora.com/What-are-network-byte-order-and-host-byte-order-in-computer-networking

### MULTIPLEXING
#### select()
- https://www.geeksforgeeks.org/socket-programming-in-cc-handling-multiple-clients-on-server-without-multi-threading/
- https://jvns.ca/blog/2017/06/03/async-io-on-linux--select--poll--and-epoll/

------------------------------------------------------------------------------------------------------

#### TELNET
- https://www.digitalocean.com/community/tutorials/telnet-command-linux-unix

-------------------------------------------------------------------------------------------------------

## GENERAL RESSOURCES - BOOKS | TUTOS
- Hands-On Network Programming with C - Lewis Van Winkle (https://github.com/mtellal/docs/blob/main/hands-on-network-programming-with-c-learn-socket-programming-in-c-and-write-secure-and-optimized-network-code-true-pdf-1nbsped-9781789349863_compress.pdf)
- https://www.freesoft.org/CIE/Course/index.htm - Tuto (introduction to networks) 

RFC 7230: Hypertext Transfer Protocol (HTTP/1.1): Message Syntax and
Routing (https://tools.ietf.org/html/rfc7230) </br>
RFC 7231: Hypertext Transfer Protocol (HTTP/1.1): Semantics and Content (https://
tools.ietf.org/html/rfc7231) </br>
RFC 1866: Hypertext Markup Language – 2.0 (https://tools.ietf.org/html/
rfc1866) </br>
RFC 3986: Uniform Resource Identifier (URI): Generic Syntax (https://tools.ietf.
org/html/rfc3986)

