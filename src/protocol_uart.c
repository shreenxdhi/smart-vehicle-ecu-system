#include <stdio.h>
#include <string.h>

void uart_send(char *message)
{
    if (!message) return;
    printf("[UART TX] \"%s\"\n", message);
}
