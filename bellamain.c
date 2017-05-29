# include <stdio.h>
# include <stdlib.h>
# include <getopt.h>
# include <string.h>
# include <unistd.h>
# include <time.h>

# include <Windows.h>
# include "bella.h"

static void usage(void);
static void print_last_error(void);

static void usage(void)
{
    printf("Usage: ./bella.exe [OPTIONS]\n\n");
    printf("Default save directory: ./saves/\n");
    printf("Default bindings:\n"
           "    hotkey1     <alt 1>    Download media from highlighted url.\n"
           "    hotkey2     <alt 2>    Download media from url in clipboard.\n"
           "    hotkey3     <alt 3>    Download all in ./bellaslist.txt\n"
           "    hotkey4     <alt 4>    Store highlighted content to ./bellaslist.txt\n"
           "    hotkey5     <alt 5>    Store highlighted content to ./bellaslist.txt and to clip board\n");

    printf("Options:\n"
           "    -h, --help              Prints this help text and exit.\n");
}

static void print_last_error(void)
{
    DWORD errorMessageID = GetLastError();
    if (errorMessageID != 0)
    {
        LPSTR messageBuffer = NULL;
        size_t size = FormatMessageA(FORMAT_MESSAGE_ALLOCATE_BUFFER |
                FORMAT_MESSAGE_FROM_SYSTEM |
                FORMAT_MESSAGE_IGNORE_INSERTS,
                NULL, errorMessageID,
                MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), 
                (LPSTR)&messageBuffer, 0, NULL);
        
        printf("Last error: %s\n", messageBuffer);

        LocalFree(messageBuffer);
    }
}

int main(int argc, char *argv[])
{
    setbuf(stdout, NULL);
    opterr = 0;

    static struct option long_options[] = {
        {"help", no_argument, 0, 'h'}
    };

    int option, option_index = 0;
    while ( (option = getopt_long(argc, argv, "h", 
                                long_options, &option_index)) != -1) 
    {
        switch (option)
        {
            case 'h':
                usage();
                break;

            default:
                abort();
        }
    }

    build_hotkeys();

    MSG msg = {0};
    while (GetMessage(&msg, NULL, 0, 0) > 0)
    {
        if (msg.message == WM_HOTKEY)
        {
            switch(msg.wParam)
            {
                case(VK_HOTKEY1):
                    printf("VK_HOTKEY1 entered\n");

                    //while(GetAsyncKeyState(VK_D)        < 0 ||
                    //      GetAsyncKeyState(VK_LCONTROL) < 0 ||
                    //      GetAsyncKeyState(VK_MENU)     < 0)
                    //{
                    //    printf("d is pressed\n");
                    //    nanosleep(&mssleep, NULL);
                    //}
                    while ( iskeydown( hotkeys[VK_HOTKEY1].keys ) ) { nanosleep(&mssleep, NULL); }

                    UINT result = SendInput(4, (LPINPUT) copy, sizeof(INPUT));

                    // remove duplicate hotkey inputs
                    while (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE)); 

                    printf("Number of events: %d\n", result);

                    if (!result)
                        print_last_error();

                    break;
                case(VK_HOTKEY2):
                    printf("VK_HOTKEY2 entered\n");
                    break;
                default:
                    printf("Something has gone horribly wrong\n");
            }
        }

    }

    HWND cbOwner = GetClipboardOwner();
    if (OpenClipboard(cbOwner)) 
    {
        printf("Opened clipboard\n");
    }
    else
    {
        fprintf(stderr, "Failed to open clipboard\n");
    }
        
    HANDLE clipboard;
    clipboard = GetClipboardData(CF_TEXT);

    if (!clipboard) 
    { 
        fprintf(stderr, "clipboard error\n"); 
        print_last_error(); 
        exit( EXIT_FAILURE ); 
    }
    else 
    { 
        LPSTR text;
        text = GlobalLock(clipboard);
        if (!text)
        {
            fprintf(stderr, "invalid text?\n");
            exit( EXIT_FAILURE);
        }
        else
        {
            printf("in clipboard: %s\n", text); 
        }

        GlobalUnlock(clipboard);
    }
    
    CloseClipboard();

    //int i;
    //printf ("Checking if processor is available...");
    //if (system(NULL)) puts ("Ok");
    //else exit (EXIT_FAILURE);
    //printf ("Executing system command...\n");
    //FILE *s = popen("youtube-dl  -o \"%(title)s.%(ext)s\" -i --format m4a --embed-thumbnail --add-metadata https://www.youtube.com/watch?v=UXGJCzGx8w4", "r"); 

    //char buffer[256];
    //while (fgets(buffer, 256, s) != NULL)
    //{
    //    //printf("buffer: %s\n", buffer);
    //}

    ////i= system ("youtube-dl  -o \"%(title)s.%(ext)s\" -i --format m4a \
    //        --embed-thumbnail --add-metadata https://www.youtube.com/watch?v=UXGJCzGx8w4");
    //printf ("The value returned was: %d.\n",i);
    //pclose(s);
    return 0;
}
