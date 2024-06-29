
#include <iostream>
#include <cstdio>
#include <string>

#include <termios.h>
#include <unistd.h>





class Segment{

    public:
    int y;
    int x;

    void move(std::string direction){

        std::string from, to;

        from = std::to_string(y) + std::to_string(x);



        if (direction == "up"){
            to = std::to_string(--y) + std::to_string(x);

        }else if (direction == "down"){
            to = std::to_string(++y) + std::to_string(x);

        }else if (direction == "right"){
            to = std::to_string(y) + std::to_string(++x);

        }else if (direction == "left"){
            to = std::to_string(y) + std::to_string(--x);

        }else{
            std::cout<<"invalid parameter";
        }


        std::string completeFromPath, completeToPath, reverseFrom, reverseTo;


        completeFromPath = "screen/" + from + "0.png";
        completeToPath = "screen/" + to + "0.png";

        reverseFrom = "screen/" + to + "1";
        reverseTo = "screen/" + from + "1";

        rename(completeFromPath.c_str(), completeToPath.c_str());

        rename(reverseFrom.c_str(), reverseTo.c_str());
    }

};



void keylogger(termios& oldTerm, termios& newTerm){

    tcgetattr(STDIN_FILENO, &oldTerm);
    newTerm = oldTerm;

    
    newTerm.c_lflag &= ~(ICANON | ECHO);
    newTerm.c_cc[VMIN] = 1;
    newTerm.c_cc[VTIME] = 0;

    tcsetattr(STDIN_FILENO, TCSANOW, &newTerm);

    char c;

    while (true) {
        read(STDIN_FILENO, &c, 1);
        std::cout << c << std::flush;

        if (c == '\033') { // Escape character
            // Read the next two characters
            char seq[2];
            read(STDIN_FILENO, &seq[0], 1);
            read(STDIN_FILENO, &seq[1], 1);

            if (seq[0] == '[') {
                switch (seq[1]) {
                    case 'A':
                        std::cout << "Up arrow key pressed" << std::endl;
                        break;
                    case 'B':
                        std::cout << "Down arrow key pressed" << std::endl;
                        break;
                    case 'C':
                        std::cout << "Right arrow key pressed" << std::endl;
                        break;
                    case 'D':
                        std::cout << "Left arrow key pressed" << std::endl;
                        break;
                }
            }
        } else {
            std::cout << c << std::flush;

            if (c == 'q') {
                break;
            }
        }
    }
}




void beforeExit(termios& oldTerm){

    tcsetattr(STDIN_FILENO, TCSANOW, &oldTerm);

}


int main(){

    // rename("screen/010.png","screen/110.png");


    // Segment segment;
    // segment.y = 1;
    // segment.x = 2;

    // segment.move("left");



    struct termios oldTerm, newTerm;

    keylogger(oldTerm, newTerm);

    
    beforeExit(oldTerm);

    return 0;
}