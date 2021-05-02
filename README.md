# Gunrock Web Server
This web server is a simple server used in ECS 150 for teaching about multi-threaded programming and operating systems. This version of the server can only handle one client at a time and simply serves static files. Also, it will close each connection after reading the request and responding, but generally is still HTTP 1.1 compliant.

This server was written by Sam King from UC Davis and is actively maintained by Sam as well. The `http_parse.c` file was written by [Ryan Dahl](https://github.com/ry) and is licensed under the BSD license by Ryan. This programming assignment is from the [OSTEP](http://ostep.org) textbook (tip of the hat to the authors for writing an amazing textbook).

# Quickstart
To compile and run the server, open a terminal and execute the following commands:
```bash
$ git clone https://github.com/kingst/gunrock_web.git
$ cd gunrock_web
$ make
$ ./gunrock_web
```

To test it out, you can either open up a web browser on the same machine and give it this url `http://localhost:8080/hello_world.html` or if you want to use curl on the command line you can test it out like this:
```bash
$ # get a basic HTML file
$ curl http://localhost:8080/hello_world.html
$ # get a basic HTML file with more detailed information
$ curl -v http://localhost:8080/hello_world.html
$ # head a basic HTML file
$ curl --head http://localhost:8080/hello_world.html
$ # test out a file that does not exist (404 status code)
$ curl -v http://localhost:8080/hello_world2.html
$ # test out a POST, which isn't supported currently (405 status code)
$ curl -v -X POST http://localhost:8080/hello_world.html
```

We also included a full website that you can use for testing, try pointing your browser to: `http://localhost:8080/bootstrap.html`

# Overview

In this assignment, you will be developing a concurrent web server. To
simplify this project, we are providing you with the code for a non-concurrent
(but working) web server. This basic web server operates with only a single
thread; it will be your job to make the web server multi-threaded so that it
can handle multiple requests at the same time.

The goals of this project are:
- To learn the basic architecture of a simple web server
- To learn how to add concurrency to a non-concurrent system
- To learn how to read and modify an existing code base effectively

Useful reading from [OSTEP](http://ostep.org) includes:
- [Intro to threads](http://pages.cs.wisc.edu/~remzi/OSTEP/threads-intro.pdf)
- [Using locks](https://pages.cs.wisc.edu/~remzi/OSTEP/threads-locks.pdf)
- [Producer-consumer relationships](http://pages.cs.wisc.edu/~remzi/OSTEP/threads-cv.pdf)
- [Server concurrency architecture](http://pages.cs.wisc.edu/~remzi/OSTEP/threads-events.pdf)

# HTTP Background

Before describing what you will be implementing in this project, we will
provide a very brief overview of how a classic web server works, and the HTTP
protocol (version 1.1) used to communicate with it; although web browsers and
servers have [evolved a lot over the
years](https://developer.mozilla.org/en-US/docs/Web/HTTP/Basics_of_HTTP/Evolution_of_HTTP),
the old versions still work and give you a good start in understanding how
things work. Our goal in providing you with a basic web server is that you can
be shielded from learning all of the details of network connections and the
HTTP protocol needed to do the project; however, the network code has been
greatly simplified and is fairly understandable should you choose to to study
it.

Classic web browsers and web servers interact using a text-based protocol
called **HTTP** (**Hypertext Transfer Protocol**). A web browser opens a
connection to a web server and requests some content with HTTP. The web server
responds with the requested content and closes the connection. The browser
reads the content and displays it on the screen.

HTTP is built on top of the **TCP/IP** protocol suite provided by the
operating system. Together, TCP and IP ensure that messages are routed to
their correct destination, get from source to destination reliably in the face
of failure, and do not overly congest the network by sending too many messages
at once, among other features. To learn more about networks, take a networking
class (or many!), or read [this free book](https://book.systemsapproach.org).

Each piece of content on the web server is associated with a file in the
server's file system. The simplest is *static* content, in which a client
sends a request just to read a specific file from the server. Slightly more
complex is *dynamic* content, in which a client requests that an executable
file be run on the web server and its output returned to the client.
Each file has a unique name known as a **URL** (**Universal Resource
Locator**). 

As a simple example, let's say the client browser wants to fetch static
content (i.e., just some file) from a web server running on some machine.  The
client might then type in the following URL to the browser:
`http://www.cs.wisc.edu/index.html`. This URL identifies that the HTTP
protocol is to be used, and that an HTML file in the root directory (`/`) of
the web server called `index.html` on the host machine `www.cs.wisc.edu`
should be fetched.

The web server is not just uniquely identified by which machine it is running
on but also the **port** it is listening for connections upon. Ports are a
communication abstraction that allow multiple (possibly independent) network
communications to happen concurrently upon a machine; for example, the web
server might be receiving an HTTP request upon port 80 while a mail server is
sending email out using port 25. By default, web servers are expected to run
on port 80 (the well-known HTTP port number), but sometimes (as in this
project), a different port number will be used. To fetch a file from a web
server running at a different port number (say 8000), specify the port number
directly in the URL, e.g., `http://www.cs.wisc.edu:8000/index.html`.

# The HTTP Request

When a client (e.g., a browser) wants to fetch a file from a machine, the
process starts by sending a machine a message. But what exactly is in the body
of that message? These *request contents*, and the subsequent *reply
contents*, are specified precisely by the HTTP protocol.

Let's start with the request contents, sent from the web browser to the
server. This HTTP request consists of a request line, followed by zero or more
request headers, and finally an empty text line. A request line has the form:
`method uri version`. The `method` is usually `GET`, which tells the web
server that the client simply wants to read the specified file; however, other
methods exist (e.g., `POST`). The `uri` is the file name, and perhaps optional
arguments (in the case of dynamic content). Finally, the `version` indicates
the version of the HTTP protocol that the web client is using (e.g.,
HTTP/1.1).

The HTTP response (from the server to the browser) is similar; it consists of
a response line, zero or more response headers, an empty text line, and
finally the interesting part, the response body. A response line has the form
version `status message`. The `status` is a three-digit positive integer that
indicates the state of the request; some common states are `200` for `OK`,
`403` for `Forbidden` (i.e., the client can't access that file), and `404` for
`File Not Found` (the famous error). Two important lines in the header are
`Content-Type`, which tells the client the type of the content in the response
body (e.g., HTML or gif or otherwise) and `Content-Length`, which indicates
the file size in bytes.

For this project, you don't really need to know this information about HTTP
unless you want to understand the details of the code we have given you. You
will not need to modify any of the procedures in the web server that deal with
the HTTP protocol or network connections. However, it's always good to learn
more, isn't it?

# A Basic Web Server

The code for the web server is available in this repository.  You can compile
the files herein by simply typing `make`. Compile and run this basic web
server before making any changes to it! `make clean` removes .o files and
executables and lets you do a clean build.

When you run this basic web server, you need to specify the port number that
it will listen on; ports below number 1024 are *reserved* (see the list
[here](https://www.iana.org/assignments/service-names-port-numbers/service-names-port-numbers.xhtml))
so you should specify port numbers that are greater than 1023 to avoid this
reserved range; the max is 65535. Be wary: if running on a shared machine, you
could conflict with others and thus have your server fail to bind to the
desired port. If this happens, try a different number!

When you then connect your web browser to this server, make sure that
you specify this same port. For example, assume that you are running on
`bumble21.cs.wisc.edu` and use port number 8003; copy your favorite HTML file
to the directory that you start the web server from. Then, to view this file
from a web browser (running on the same or a different machine), use the url
`bumble21.cs.wisc.edu:8003/favorite.html`. If you run the client and web
server on the same machine, you can just use the hostname `localhost` as a
convenience, e.g., `localhost:8003/favorite.html`.

To make the project a bit easier, we are providing you with a minimal web
server, consisting of only a few hundred lines of C++ code. As a result, the
server is limited in its functionality; it does not handle any HTTP requests
other than `GET` and `HEAD`, and understands only a few content types. This web server is also
not very robust; for example, if a web client closes its connection to the
server, it may trip an assertion in the server causing it to exit. We do not
expect you to fix these problems (though you can, if you like, you know, for
fun).

# Finally: Some New Functionality!

In this project, you will be adding one key piece of functionality to the
basic web server: you will make the web server multi-threaded. 
You will also be modifying how the web server is invoked so
that it can handle new input parameters (e.g., the number of threads to
create).

## Part 1: Multi-threaded
 
The basic web server that we provided has a single thread of
control. Single-threaded web servers suffer from a fundamental performance
problem in that only a single HTTP request can be serviced at a time. Thus,
every other client that is accessing this web server must wait until the
current http request has finished; this is especially a problem if the current
HTTP request is a long-running CGI program or is resident only on disk (i.e.,
is not in memory). Thus, the most important extension that you will be adding
is to make the basic web server multi-threaded.

The simplest approach to building a multi-threaded server is to spawn a new
thread for every new http request. The OS will then schedule these threads
according to its own policy. The advantage of creating these threads is that
now short requests will not need to wait for a long request to complete;
further, when one thread is blocked (i.e., waiting for disk I/O to finish) the
other threads can continue to handle other requests. However, the drawback of
the one-thread-per-request approach is that the web server pays the overhead
of creating a new thread on every request.

Therefore, the generally preferred approach for a multi-threaded server is to
create a fixed-size *pool* of worker threads when the web server is first
started. With the pool-of-threads approach, each thread is blocked until there
is an http request for it to handle. Therefore, if there are more worker
threads than active requests, then some of the threads will be blocked,
waiting for new HTTP requests to arrive; if there are more requests than
worker threads, then those requests will need to be buffered until there is a
ready thread.

In your implementation, you must have a master thread that begins by creating
a pool of worker threads, the number of which is specified on the command
line. Your master thread is then responsible for accepting new HTTP
connections over the network and placing the descriptor for this connection
into a fixed-size buffer; in your implementation, the master thread
should not read from this connection. The number of elements in the buffer is
also specified on the command line. Note that the existing web server has a
single thread that accepts a connection and then immediately handles the
connection; in your web server, this thread should place the connection
descriptor into a fixed-size buffer and return to accepting more connections.

Each worker thread is able to handle requests. A
worker thread wakes when there is an HTTP request in the queue; when there are
multiple HTTP requests available, which request is handled depends upon the
scheduling policy, described below. Once the worker thread wakes, it performs
the read on the network descriptor, obtains the specified content (by either
reading the static file or executing the CGI process), and then returns the
content to the client by writing to the descriptor. The worker thread then
waits for another HTTP request.

Note that the master thread and the worker threads are in a producer-consumer
relationship and require that their accesses to the shared buffer be
synchronized. Specifically, the master thread must block and wait if the
buffer is full; a worker thread must wait if the buffer is empty. In this
project, you are required to use condition variables. Note: if your
implementation performs any busy-waiting (or spin-waiting) instead, you will
be heavily penalized.

## Part 2: Scheduling Policies

In this project, you will implement one scheduling
policy. Note that when your web server has multiple worker threads running
(the number of which is specified on the command line), you will not have any
control over which thread is actually scheduled at any given time by the
OS. Your role in scheduling is to determine which HTTP request should be
handled by each of the waiting worker threads in your web server.

The scheduling policy is:

- **First-in-First-out (FIFO)**: When a worker thread wakes, it handles the
first request (i.e., the oldest request) in the buffer. Note that the HTTP
requests will not necessarily finish in FIFO order; the order in which the
requests complete will depend upon how the OS schedules the active threads.

## Security

Running a networked server can be dangerous, especially if you are not
careful. Thus, security is something you should consider carefully when
creating a web server. One thing you should always make sure to do is not
leave your server running beyond testing, thus leaving open a potential
backdoor into files in your system.

Your system should also make sure to constrain file requests to stay within
the sub-tree of the file system hierarchy, rooted at the base working
directory that the server starts in. You must take steps to ensure that
pathnames that are passed in do not refer to files outside of this sub-tree. 
One simple (perhaps overly conservative) way to do this is to reject any
pathname with `..` in it, thus avoiding any traversals up the file system
tree. More sophisticated solutions could use `chroot()` or Linux containers,
but perhaps those are beyond the scope of the project.

# Gunrock internals

## Command line arguments
Your C++ program must be invoked exactly as follows:

```bash
$ ./gunrock_web [-p port] [-t threads] [-b buffers]
```

The command line arguments to your web server are to be interpreted as
follows.

- **port**: the port number that the web server should listen on; the basic web
  server already handles this argument. Default: 8080.
- **threads**: the number of worker threads that should be created within the web
  server. Must be a positive integer. Default: 1.
- **buffers**: the number of request connections that can be accepted at one
  time. Must be a positive integer. Note that it is not an error for more or
  less threads to be created than buffers. Default: 1.

For example, you could run your program as:
```
$ ./gunrock_web -p 8003 -t 8 -b 16
```

In this case, your web server will listen to port 8003, create 8 worker threads for
handling HTTP requests, and allocate 16 buffers for connections that are currently
in progress (or waiting).

## Key concepts
The main idea behind this server is to make adding handlers as easy as writing a function. The `FileService.cpp` is a simple service that will read a file from the `static` directory and serve it back to the client as HTML. If you want to write new handlers, you'd do it by adding the new service and inheriting from `HttpService`, adding your source file to the `Makefile` and registering your service with the main `gunrock.cpp` file as a new service.

To match services to requests, the main `gunrock.cpp` logic tries to find the first path prefix match that it can, and when it finds a match it forwards the request on to the service for handling.

From within the service, you set the body of the request, or if there is an error you set the appropriate status code in the response object.

## Thread functions

We created a pthread replacement library, called `dthread`, that you must
use for this project. `dthread` logs information about your use of threads,
mutexes, and condition variables so that we can grade your project.

We anticipate that you will find the following routines useful for creating
and synchronizing threads: `dthread_create()`, `dthread_detach`,
`dthread_mutex_lock()`, `dthread_mutex_unlock()`,
`dthread_cond_wait()`, `dthread_cond_signal()`. To find information on these
library routines, read the man pages for the pthread version of these same
routines. To initialize your mutex and condition variables, assign them
to the `PTHREAD_MUTEX_INITIALIZER` and `PTHREAD_COND_INITIALIZER` macros
and you'll get initialized mutex and conidition variables.

## Key files
To make this server multithreaded, you're going to need to modify the main `gunrock.cpp` file and potentially `FileService.cpp`. You'll need to modify these files so that client requests are handled by a pool of threads with some priority logic to handle high priority files first. See the project README for more details.

## Other files
- **gunrock** - The main function + basic request handling
- **FileService** - Main file service, where the application logic for reading files goes
- **dthread** -- The main threading utilities, use the functions in this file for your threads
- **HTTP** - Higher level HTTP object, interfaces with the `http_parser`
- **http_parser** - HTTP protocol parsing state machine and callback functionality
- **HTTPRequest** - The HTTP request object, this is filled in by the framework
- **HTTPResponse** - The HTTP response object, the data for the response is filled in by the service
- **HttpUtils** - Simple utility functions for working with HTTP data
- **MyServerSocket** - High level abstraction on top of server sockets, accepts connections from new clients
- **MySocket** - High level abstraction on top of sockets, used by the framework to read requests and write responses

## Autograding
We aren't providing test cases for this project, so an important part of the project is developing your own test cases. We'll try to give descriptive text that explains what an autograded test case tests, but it won't be perfect. Our suggestion is to invest in an extensive test suite that you write yourself to exercise your server.

When you submit your project via gradescope, you will turn in two files: `gunrock.cpp` and `FileService.cpp`.

