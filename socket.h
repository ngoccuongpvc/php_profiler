#define GLOB(v) ZEND_MODULE_GLOBALS_ACCESSOR(php_profiler, v)

static zend_always_inline void init_sock(char* sock_file)
{
    struct sockaddr_un server;

    GLOB(sock) = socket(AF_UNIX, SOCK_STREAM, 0);
    if (GLOB(sock) < 0) {
          return;
      }
      server.sun_family = AF_UNIX;
      strcpy(server.sun_path, sock_file);
      if (connect(GLOB(sock), (struct sockaddr *) &server, sizeof(struct sockaddr_un)) < 0) {
          close(GLOB(sock));
          return;
      }
}

static zend_always_inline void send_data()
{
    if (write(GLOB(sock), GLOB(function_chunk), GLOB(chunk_length)) < 0) {
        ///todo stop sending through socket
    } else {
        GLOB(chunk_length) = 0;
    }
}