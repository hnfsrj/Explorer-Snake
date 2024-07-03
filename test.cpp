
#include <iostream>
#include <cstdio>
#include <string>

#include <termios.h>
#include <unistd.h>

#include <thread>

#include <filesystem>
#include <random>
#include <fstream>



int random_generator(int max){
    std::random_device rd;
    std::mt19937 gen(rd());

    std::uniform_int_distribution<> dis(0, (max-1));

    return dis(gen);
}



void initialize(int width, int height, int pos[2]){
    // remove and create the screen folder

    std::filesystem::path dir{"screen"};

    std::filesystem::remove_all(dir);

    std::filesystem::create_directory(dir);

    //generate the head location

    int head_y = 1;
    int head_x = 1;

    pos[0] = head_y;
    pos[1] = head_x;

    //generate the apple location

    int apple_y = random_generator(height);
    int apple_x = random_generator(width);

    while (apple_y == head_y && apple_x == head_x){
        apple_y = random_generator(height);
        apple_x = random_generator(width);
    }


    //generate the head and apple

    std::filesystem::path from{"assets/head.png"};
    std::filesystem::path to{"screen/head.png"};

    std::filesystem::copy_file(from, to);

    from = "assets/apple.png";
    to = "screen/apple.png";

    std::filesystem::copy_file(from, to);

    std::string hx,hy,ax,ay;

    hx = std::to_string(head_x);
    hy = std::to_string(head_y);
    ax = std::to_string(apple_x);
    ay = std::to_string(apple_y);

    // std::cout<<"screen/" + hy + hx + "0.png";
    rename("screen/head.png",("screen/" + hy + hx + "0.png").c_str());
    rename("screen/apple.png",("screen/" + ay + ax + "2.png").c_str());

    //generate the txt files

    for(int w = 0;w<width;w++){
        for(int h = 0;h<height;h++){
            if (!(w == head_x && h == head_y) && !(w == apple_x && h == apple_y)){

                //create the txt file
                std::string file_path = "screen/" + std::to_string(h) + std::to_string(w) + "1.txt";

                std::ofstream(file_path).close();
                
            }

        }
    }


}





class Segment{

    int y, x;

    public:
        
        Segment(int y_val, int x_val){
            y = y_val;
            x = x_val;
        }

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

            reverseFrom = "screen/" + to + "1.txt";
            reverseTo = "screen/" + from + "1.txt";

            rename(completeFromPath.c_str(), completeToPath.c_str());

            rename(reverseFrom.c_str(), reverseTo.c_str());
        }

};


class Apple{
    public:

    int y;
    int x;

    void reposition(){

    }


};





// bool collision(Apple apple){

//     if(moves[0].x == apple.x && moves[0].y == apple.y) return true;

//     return false;
// }



void keylogger(termios& oldTerm, termios& newTerm, Segment& segment){

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
                        // std::cout << "Up arrow key pressed" << std::flush;
                        segment.move("up");
                        break;
                    case 'B':
                        // std::cout << "Down arrow key pressed" << std::flush;
                        segment.move("down");
                        break;
                    case 'C':
                        // std::cout << "Right arrow key pressed" << std::flush;
                        segment.move("right");
                        break;
                    case 'D':
                        // std::cout << "Left arrow key pressed" << std::flush;
                        segment.move("left");
                        break;
                }
            }
        } else {
            std::cout << c << std::flush;

            if (c == 'q') {
                break;
            }
        }

        // std::this_thread::yield();
    }
}




void beforeExit(termios& oldTerm, std::thread& t){

    t.join();
    tcsetattr(STDIN_FILENO, TCSANOW, &oldTerm);
}


int main(){

    int width,height;

    width = 5;
    height = 6;

    int pos[2];

    // initialize the playground
    initialize(width,height,pos);

    // std::vector<Segment> segments;

    // segments.push_back(segment(pos[0], pos[1]));



    // Apple apple;


    Segment segment(pos[0], pos[1]);







    struct termios oldTerm, newTerm;

    std::thread t(keylogger, std::ref(oldTerm), std::ref(newTerm), std::ref(segment));    
   
    
    beforeExit(oldTerm, std::ref(t));

    return 0;
}