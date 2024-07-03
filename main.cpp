
#include <iostream>
#include <cstdio>
#include <string>

#include <termios.h>
#include <unistd.h>

#include <thread>

#include <filesystem>
#include <random>
#include <fstream>

#include <chrono>




enum directions{
    Up, Down, Right, Left
};

int random_generator(int max){
    std::random_device rd;
    std::mt19937 gen(rd());

    std::uniform_int_distribution<> dis(0, (max-1));

    return dis(gen);
}



void initialize(int width, int height, int pos[4]){
    // remove and create the screen folder

    std::filesystem::path dir{"screen"};

    std::filesystem::remove_all(dir);

    std::filesystem::create_directory(dir);

    //generate the head location

    // int head_y = random_generator(height);
    int head_y = 1;

    // int head_x = random_generator(width);
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

    pos[2] = apple_y;
    pos[3] = apple_x;


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

    public:

        int y, x;

        int move_reader = 0;
        
        Segment(int y_val, int x_val){
            y = y_val;
            x = x_val;
        }

        void move(directions direction){

            std::string from, to;

            from = std::to_string(y) + std::to_string(x);



            if (direction == Up){
                to = std::to_string(--y) + std::to_string(x);

            }else if (direction == Down){
                to = std::to_string(++y) + std::to_string(x);

            }else if (direction == Right){
                to = std::to_string(y) + std::to_string(++x);

            }else if (direction == Left){
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


            move_reader++;
        }

};


class Apple{

    public:

        int y,x;


        Apple(int y_val, int x_val){
            y = y_val;
            x = x_val;
        }


        void reposition(int new_apple_y, int new_apple_x){
            std::string from, to, completeFromPath, completeToPath, reverseFrom, reverseTo;

            from = std::to_string(y) + std::to_string(x);
            to = std::to_string(new_apple_y) + std::to_string(new_apple_x);

            completeFromPath = "screen/" + from + "2.png";
            completeToPath = "screen/" + to + "2.png";

            reverseFrom = "screen/" + to + "1.txt";
            reverseTo = "screen/" + from + "1.txt";

            y = new_apple_y;
            x = new_apple_x;

            rename(completeFromPath.c_str(), completeToPath.c_str());
            rename(reverseFrom.c_str(), reverseTo.c_str());

        }


};





void collision(int head_y, int head_x, int apple_y, int apple_x, Apple& apple, directions direction, std::vector<Segment> segments, int width, int height){
            
    if (direction == Up){
        head_y--;

    }else if (direction == Down){
        head_y++;

    }else if (direction == Right){
        head_x++;

    }else if (direction == Left){
        head_x--;

    }

    if (apple_y == head_y && apple_x == head_x){
        //collision will happen on this next move

        bool generate = true;
        int new_apple_y, new_apple_x;

        while (generate){
            new_apple_y = random_generator(height);
            new_apple_x = random_generator(width);

            if (!(new_apple_y == apple_y && new_apple_x == apple_x)){

                generate = false;
            }else{

                for (int s=0;s<segments.size();s++){

                    if (new_apple_y == segments[s].y && new_apple_x == segments[s].x){
                        generate = true;
                        break;
                    }else{
                        generate = false;
                    }

                }

            }

            // set new apple position

            apple.reposition(new_apple_y, new_apple_x);

        }
    
    }

}



void keylogger(termios& oldTerm, termios& newTerm, std::vector<directions>& moves){

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
                        moves.push_back(Up);
                        break;
                    case 'B':
                        // std::cout << "Down arrow key pressed" << std::flush;
                        moves.push_back(Down);
                        break;
                    case 'C':
                        // std::cout << "Right arrow key pressed" << std::flush;
                        moves.push_back(Right);
                        break;
                    case 'D':
                        // std::cout << "Left arrow key pressed" << std::flush;
                        moves.push_back(Left);
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



void renderer(std::vector<directions>& moves, std::vector<Segment>& segments, Apple& apple, int width, int height){
    
    int watching = 0;

    while (true){

        //adding the next move to be executed

        if (moves.empty()){
            
            //this runs when there is nothing in moves i.e when the game starts.
            moves.push_back(Right);
        }else{
            
            if (moves.size() <= watching){
                //this will run when there is no user input for direction

                moves.push_back(moves[moves.size()-1]); //same direction as the last one
            }
            
            //no need to auto add a move when there has been an input from the user

            

        }

        watching++;

        //check if the head is going to collide with the apple and take action
        collision(segments[0].y, segments[0].x, apple.y, apple.x, apple, moves[moves.size()-1],segments, width, height);


        for (int s=0;s<segments.size();s++){
            segments[s].move(moves[segments[s].move_reader]); //move to the direction respective to its own position in the series of moves
        }



        std::this_thread::sleep_for(std::chrono::seconds(1));
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

    int pos[4];

    // initialize the playground
    initialize(width,height,pos);

    std::vector<Segment> segments;

    std::vector<directions> moves;

    segments.push_back(Segment(pos[0], pos[1]));


    Apple apple(pos[2], pos[3]);




    struct termios oldTerm, newTerm;

    std::thread t(keylogger, std::ref(oldTerm), std::ref(newTerm), std::ref(moves));    
   


    renderer(moves, segments, apple, width, height);
    
    beforeExit(oldTerm, std::ref(t));

    return 0;
}