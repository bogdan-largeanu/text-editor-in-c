/*** includes ***/
#include <ctype.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <termios.h>
#include <unistd.h>

/*** data ***/
struct termios orig_termios;

/*** terminal ***/
void die(const char *s){
    /*
    Most C library functions that fail will set the global errno variable to indicate
    what the error was. perror() looks at the global errno variable and 
    prints a descriptive error message for it. It also prints the string given
    to it before it prints the error message, which is meant to provide context
    about what part of your code caused the error.
   */
    perror(s);
    exit(1);
}

void disableRawMode(){
    if (tcsetattr(STDIN_FILENO, TCSAFLUSH, &orig_termios) == -1)
        die("tcsetattr");
}

void enableRawMode(){
    if (tcgetattr(STDIN_FILENO, &orig_termios) == -1) die("tcgetattr");
    atexit(disableRawMode);
    
    struct termios raw = orig_termios;
    // disabling all sort of sig inputs and output to enter "raw mode"
    raw.c_iflag &= ~(BRKINT | ICRNL | INPCK | ISTRIP | IXON);
    raw.c_oflag &= ~(OPOST);
    raw.c_cflag |= (CS8);
    raw.c_lflag &= ~(ECHO | ICANON | IEXTEN | ISIG);
    // setting a timeout if no input comes
    raw.c_cc[VMIN] = 0;
    raw.c_cc[VTIME] = 1;

    if (tcsetattr(STDIN_FILENO, TCSAFLUSH, &raw) == -1) die("tcsetattr");
}

/*** init ***/
int main() {
    enableRawMode();

    char c = '\0';
        // We are asking read() to read 1 byte from the standard input into the variable c
    while (1) {
        char c = '\0';
        if (read(STDIN_FILENO, &c, 1) == -1 && errno != EAGAIN) die("read");
        if (iscntrl(c)){ // iscntrl() tests whether a character is a control character. Control characters are nonprintable characters that we don’t want to print to the screen. ASCII codes 0–31 are all control characters, and 127 is also a control character. ASCII codes 32–126 are all printable. (Check out the ASCII table to see all of the characters.)
            printf("%d\r\n", c);
        }else{
            // printf() can print multiple representations of a byte. 
            // %d tells it to format the byte as a decimal number (its ASCII code),
            //  and %c tells it to write out the byte directly, as a character.
            printf("%d ('%c')\r\n", c, c);
        }
        if (c == 'q') break;
    }

    return 0;
}
