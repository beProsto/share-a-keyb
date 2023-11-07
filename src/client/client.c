#include <share-a-keyb/networking.h>
#include <share-a-keyb/keyboard.h>

#define DESIRED_ADDRESS "127.0.0.1"
#define BUFSIZE 512

int main(int argc, char **argv) {
  // assume the other arguments are the address and the port we want
  const char *chosen_address = ((argc > 1) ? argv[1] : DESIRED_ADDRESS);
  const uint16_t chosen_port = ((argc > 2) ? atoi(argv[2]) : DESIRED_PORT);

  if (init_networking() != 0) {
    perror("Init failed");
    return 1;
  }

  address_t addr = make_address(chosen_address, chosen_port);

  socket_t sock = make_tcp_client_socket(&addr);
  if (sock < 0) {
    perror("Client failed to connect");
    return 2;
  }

  while (true) {
    key_input_info_t keyinputinfo = {0};
    int received_size = recv(sock, &keyinputinfo, sizeof(keyinputinfo), 0);
    if (received_size == 0) {
      printf("connection ended by server\n");
      return 3;
    } else if (received_size < 0) {
      printf("connection error\n");
      return 4;
    }

    printf("%s: %d\n", (keyinputinfo.eventtype == 0 ? "KeyUp" : (keyinputinfo.eventtype == 1 ? "KeyDown" : "KeyHold")), keyinputinfo.scancode);

    simulate_keyboard_input(keyinputinfo);
  }
  return 0;
}