#include <iostream>
#include <X11/Xlib.h>
#include <X11/Xutil.h>

#include <termios.h>
#include <unistd.h>

#include <X11/extensions/XTest.h>
#include <X11/extensions/record.h>
#include <X11/keysym.h>
#include <cstdlib>












Display *display;
XRecordContext recordContext;

void eventCallback(XPointer priv, XRecordInterceptData *hook) {
    if (hook->category == XRecordFromServer) {
        const xEvent *xev = (const xEvent *)hook->data;
        if (xev->u.u.type == KeyPress) {
            KeySym keysym = XKeycodeToKeysym(display, xev->u.u.detail, 0);
            char *keyString = XKeysymToString(keysym);
            if (keyString) {
                std::cout << "Key pressed: " << keyString << std::endl;
            }
        }
    }
    XRecordFreeData(hook);
}




int main() {

    /*
    struct termios oldTerm, newTerm;

    // Get current terminal attributes
    tcgetattr(STDIN_FILENO, &oldTerm);
    newTerm = oldTerm;

    // Modify terminal attributes for non-canonical mode
    newTerm.c_lflag &= ~(ICANON | ECHO);  // Turn off canonical mode and echo
    newTerm.c_cc[VMIN] = 1;  // Minimum number of characters to read
    newTerm.c_cc[VTIME] = 0; // Timeout in tenths of a second (0 = no timeout)

    tcsetattr(STDIN_FILENO, TCSANOW, &newTerm);  

    */

    display = XOpenDisplay(NULL);
    if (!display) {
        std::cerr << "Cannot open display" << std::endl;
        return -1;
    }

    XSynchronize(display, True);

    XRecordRange *recordRange = XRecordAllocRange();
    if (!recordRange) {
        std::cerr << "Cannot allocate XRecordRange" << std::endl;
        return -1;
    }
    recordRange->device_events.first = KeyPress;
    recordRange->device_events.last = KeyRelease;

    XRecordClientSpec clients = XRecordAllClients;
    recordContext = XRecordCreateContext(display, 0, &clients, 1, &recordRange, 1);
    if (!recordContext) {
        std::cerr << "Cannot create XRecordContext" << std::endl;
        return -1;
    }

    if (!XRecordEnableContextAsync(display, recordContext, eventCallback, NULL)) {
        std::cerr << "Cannot enable XRecordContext" << std::endl;
        return -1;
    }

    while (true) {
        XEvent event;
        
        std::cout<<"inside\n";
        XNextEvent(display, &event);
        std::cout<<"inside end\n";

        // read(STDIN_FILENO, &c, 1);

        // std::cout << c;
        // if (ev.type == KeyPress) {
        //     std::cout << "Key pressed: " << XLookupKeysym(&ev.xkey, 0) << std::endl;
        // }
    }




    /*
    Display* dpy = XOpenDisplay(nullptr);
    if (!dpy) {
        std::cerr << "Cannot open display" << std::endl;
        return 1;
    }
    
    XSelectInput(dpy, DefaultRootWindow(dpy), KeyPressMask);

    // Window root = DefaultRootWindow(dpy);
    XEvent ev;

    // char c;
    
    while (true) {
        std::cout<<"inside\n";
        XNextEvent(dpy, &ev);
        std::cout<<"inside end\n";

        // read(STDIN_FILENO, &c, 1);

        // std::cout << c;
        if (ev.type == KeyPress) {
            std::cout << "Key pressed: " << XLookupKeysym(&ev.xkey, 0) << std::endl;
        }
    }

    std::cout<<"outside\n";
    */
    
    XCloseDisplay(display);

    return 0;
}
