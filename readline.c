#include "readline.h"
#include <string.h>
#include "usart.h"

#define KEY_CR  '\r'
#define KEY_LF  '\n'
#define KEY_BS  '\b'
#define KEY_ESC '\033'
#define KEY_SPACE ' '
#define KEY_TAB '\t'

#define SEQ_BS "\033[D \033[D"

static char cmdline[READLINE_BUF_SIZE];
static int  cursor = 0;

static char *argv[READLINE_MAX_ARGS];
static int  argc = 0;

static void split(void)
{
    int idx = 0;
    int lim = cursor;

    argc = 0;

    while (1) {
        while ((cmdline[idx] == '\0') && (idx < lim)) {
            idx ++;
        }

        if ( !(idx < lim) )
            break;

        argv[ argc++ ] = cmdline + idx;

        if (argc >= READLINE_MAX_ARGS)
            break;

        while ((cmdline[idx] != '\0') && (idx < lim)) {
            idx ++;
        }

        if ( !(idx < lim) )
            break;
    }
}

static void readline_prompt(void)
{
    usart_puts("> ");
}

static void readline_insert_char(char ch)
{
    switch (ch) {
        
        case KEY_CR:
        case KEY_LF:

            usart_puts("\r\n");
            
            cmdline[cursor] = '\0';
            split();

            readline_exec(argc, argv);

            cursor = 0;
            readline_prompt();

            break;

        case KEY_ESC:
            break;

        case KEY_BS:

            if (cursor > 0) {
                usart_puts(SEQ_BS);
                cursor--;
            }

            break;

        default:

            if (cursor < (READLINE_BUF_SIZE - 1)) {

                usart_putc(ch);

                if (ch == KEY_TAB || ch == KEY_SPACE)
                    cmdline[cursor] = '\0';
                else
                    cmdline[cursor] = ch;

                cursor++;
            }

            break;
    }
}

void readline_receive_task(void)
{
    int ch = usart_getc();

    if (ch == USART_NO_DATA)
        return;

    readline_insert_char((char) ch);
}

void readline_init()
{
    cursor = 0;
    argc   = 0;

    readline_prompt();
}


//
//
//
//
//
//
//
//
//
//
//
//
//
//void add_char(char c)
//{
//    if (cursor < MAX_BUF)
//    {
//        line[cursor] = c;
//        cursor++;
//        usart_putc(c);
//    }
//}
//
//void backspace(void)
//{
//    if (cursor > 0) {
//        cursor--;
//
//        usart_puts("\033[D \033[D");
//    }
//}
//
//void parse_line(void)
//{
//    char *ptr = line;
//
//    argc = 0;
//    argv[argc] = ptr;
//
//    
//}
//
//void process_char(char c)
//{
//    switch(c) {
//        case '\b':
//            backspace();
//            break;
//
//        case '\r':
//        case '\n':
//            line[cursor] = '\0';
//            cursor = 0;
//
//            usart_puts("\r\n");
//            parse_line();
//            break;
//
//        default:
//            add_char(c);
//            break;
//    }
//}
//
//void console(void)
//{
//    int c = usart_getc();
//
//    if (c == USART_NO_DATA)
//        return;
//
//    process_char((char) c);
//}
