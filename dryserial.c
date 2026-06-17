#include <ncurses.h>
#include <stdlib.h>
#include <fcntl.h>      
#include <termios.h>   
#include <unistd.h>   
#include <string.h>  
#include <ctype.h>

// Included in ncurses - left just in case 
//  #include <stdio.h>

#define STD_COLORS      (1)
#define MAX_HUE         (1000)

#define BUFFER_SIZE     (256)

int configure_port(int fd);

int main(int argc, char* argv[]) {
    if (argc < 2) {
        printf("Usage: dryserial <port>\n");
        printf("Example: dryserial /dev/ttyACM0\n");
        
        return 1;
    }

    initscr();
    cbreak();
    noecho();

    if (has_colors()) {
        start_color();
        init_color(COLOR_BLACK, 0, 0, 0);
        init_pair(STD_COLORS, COLOR_WHITE, COLOR_BLACK);
        attr_set(A_NORMAL, STD_COLORS, NULL);
    }


    char* strPortError = "Error opening serial port";
    char* strConnect = "Connected:";
    char* strPort = argv[1];

    // Open the serial port
    int serial_port = open(argv[1], O_RDWR | O_NOCTTY);
    if (serial_port == -1) {
        perror(strPortError);
        
        endwin();
        return 1;
    }

    mvprintw(5, 0, "serial_port: %d", serial_port);
    mvprintw(0, 0, "%s %s", strConnect, strPort);

    getch();

    configure_port(serial_port);

    char buffer[BUFFER_SIZE];
    int bytes_read;
    
    mvprintw(1, 0, "Begin spin cycle....");
    refresh();

    while (1) {
        // Read data from the STM32
        static int i;
        static char ch;

        bytes_read = read(serial_port, &ch, 1);
        
        if (bytes_read > 0) {
            buffer[i++] = ch;

            if (ch == '\n' || i >= BUFFER_SIZE - 1) {
                if (i > BUFFER_SIZE - 1) {
                    i = BUFFER_SIZE - 1;
                }

                buffer[i] = '\0'; 
                mvprintw(2, 0, "Received: %s", buffer);
                i = 0;
            }

            refresh();
        }
    }

    endwin();
    close(serial_port);
    return 0;
}

int configure_port(int fd) {
    struct termios options;

    tcgetattr(fd, &options);  // Get current port settings

    // Set baud rate to 9600
    cfsetispeed(&options, B9600);
    cfsetospeed(&options, B9600);

    // Configure 8N1 mode (8 data bits, no parity, 1 stop bit)
    options.c_cflag &= ~PARENB;          // No parity
    options.c_cflag &= ~CSTOPB;          // 1 stop bit
    options.c_cflag &= ~CSIZE;           // Clear current data size
    options.c_cflag |= CS8;              // 8 data bits

    options.c_cflag |= CREAD | CLOCAL;   // Enable receiver, ignore modem control lines

    // Raw input/output (no processing)
    options.c_lflag &= ~(ICANON | ECHO | ECHOE | ISIG); 
    options.c_iflag &= ~(IXON | IXOFF | IXANY);
    options.c_oflag &= ~OPOST;

    tcsetattr(fd, TCSANOW, &options);    // Apply new settings immediately

    return 0;
}
