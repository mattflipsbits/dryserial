// TODO: Add write functionality
//       Functionize read/write

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <fcntl.h>      
#include <termios.h>   
#include <unistd.h>   
#include <string.h>  
#include <ctype.h>

#define BUFFER_SIZE     (256)

int configure_port(int fd, unsigned long rawBaud);
void list_bauds();
bool valid_baud(unsigned long baud);
speed_t convert_baud(unsigned long rawBaud);

int main(int argc, char* argv[]) {
    if (argc == 2 && !strcmp(argv[1], "-b")) {
        list_bauds();

        return 0;
    }

    if (argc < 3) {
        printf("Usage: dryserial <port> <baud>\n");
        printf("Example: dryserial /dev/ttyACM0 9600\n");
        printf("List of baud rates: dryserial -b\n");
        
        return 1;
    }

    char* strPortError = "Error opening serial port";
    char* strBaudError = "Invalid baud rate\n";
    char* strConnect = "Connected:";
    char* strPort = argv[1];
    char* strBaud = argv[2];

    int serialPort = open(argv[1], O_RDWR | O_NOCTTY);
    
    if (serialPort == -1) {
        perror(strPortError);
        
        return 1;
    }

    char* end = NULL;
    unsigned long rawBaud = strtoul(strBaud, &end, 10);

    if (!valid_baud(rawBaud)) {
        fprintf(stderr, "%s", strBaudError);
        puts("For valid baud rates: dryserial -b");

        return 1;
    }

    configure_port(serialPort, rawBaud);
    
    printf("%s %s\n", strConnect, strPort);

    char buffer[BUFFER_SIZE];
    int bytesRead;
    
    while (1) {
        static int i;
        static char ch;

        bytesRead = read(serialPort, &ch, 1);
        
        if (bytesRead > 0) {
            buffer[i++] = ch;

            if (ch == '\n' || i >= BUFFER_SIZE - 1) {
                if (i > BUFFER_SIZE - 1) {
                    i = BUFFER_SIZE - 1;
                }

                buffer[i] = '\0'; 
                printf("Received: %s", buffer);
                i = 0;
            }
        }

        /*
             Write data

             char *msg = "Hello STM32!\n";
             write(serialPort, msg, strlen(msg));
             printf("Sent: %s", msg);
        */
    }

    close(serialPort);
    return 0;
}

int configure_port(int fd, unsigned long rawBaud) {
    struct termios options;
    speed_t baud = convert_baud(rawBaud);

    tcgetattr(fd, &options);  // Get current port settings

    // Set baud rate to 9600
    cfsetispeed(&options, baud);
    cfsetospeed(&options, baud);

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

// TODO: ALL the baud rate stuff can be done better (struct/hash map?)
//       Works for now, though

void list_bauds() {
    unsigned long baudList[] = {
        0,
        50,
        75,
        110,
        134,
        150,
        200,
        300,
        600,
        1200,
        1800,
        2400,
        4800,
        9600,
        19200,
        38400,
        57600,
        115200,
        230400,
        460800,
        500000,
        576000,
        921600,
        1000000,
        1152000,
        1500000,
        2000000
    };

    puts("Valid baud rates:");

    for (size_t i = 0; i < sizeof(baudList) / sizeof(unsigned long); i++) {
        printf("%ld\n", baudList[i]);
    }
}

bool valid_baud(unsigned long baud) {
    switch(baud) {
        // Fall-through intended
        case 0:
        case 50:
        case 75:
        case 110:
        case 134:
        case 150:
        case 200:
        case 300:
        case 600:
        case 1200:
        case 1800:
        case 2400:
        case 4800:
        case 9600:
        case 19200:
        case 38400:
        case 57600:
        case 115200:
        case 230400:
        case 460800:
        case 500000:
        case 576000:
        case 921600:
        case 1000000:
        case 1152000:
        case 1500000:
        case 2000000:
            return true;
            break;
        default:
            return false;
            break;
    }
}

speed_t convert_baud(unsigned long rawBaud) {
    switch(rawBaud) {
        case 0:
            return B0;
            break;
        case 50:
            return B50;
            break;
        case 75:
            return B75;
            break;
        case 110:
            return B110;
            break;
        case 134:
            return B134;
            break;
        case 150:
            return B150;
            break;
        case 200:
            return B200;
            break; 
        case 300:
            return B300;
            break;
        case 600:
            return B600;
            break;
        case 1200:
            return B1200;
            break;
        case 1800:
            return B1800;
            break;
        case 2400:
            return B2400;
            break;
        case 4800:
            return B4800;
            break;
        case 9600:
            return B9600;
            break;
        case 19200:
            return B19200;
            break;
        case 38400:
            return B38400;
            break;
        case 57600:
            return B57600;
            break;
        case 115200:
            return B115200;
            break;
        case 230400:
            return B230400;
            break;
        case 460800:
            return B460800;
            break;
        case 500000:
            return B500000;
            break;
        case 576000:
            return B576000;
            break;
        case 921600:
            return B921600;
            break;
        case 1000000:
            return B1000000;
            break;
        case 1152000:
            return B1152000;
            break;
        case 1500000:
            return B1500000;
            break;
        case 2000000:
            return B2000000;
            break;
        default:
            return -1;
            break;
    }
}
