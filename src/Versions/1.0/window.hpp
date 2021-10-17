#ifndef window_hpp
#define window_hpp

#include <ncurses.h>
#include <vector>
#include <string>
#include "text.hpp"

class _window_{
public:
    MEVENT mouse;
    int mxpos;                              //x-position of mouse
    int mypos;                              //y-position of mouse

    int width;
    int height;

    int cxpos;                              //x-position of cursor
    int cypos;                              //y-position of cursor

    int buffer;

    _window_(){
        cxpos = 0;
        cypos = 0;
        width = COLS;
        height = LINES;
    }
    ~_window_(){}

    void setup(){
        initscr();
        noecho();
        cbreak();
        keypad(stdscr, true);
        mousemask(ALL_MOUSE_EVENTS, NULL);
        clear();
    }

    void quit(){
        endwin();
    }

    void get_event(){
        buffer = getch();
        width = COLS;
        height = LINES;
    }

    int get(){
        return buffer;
    }

    MEVENT getmouse(){
        return mouse;
    }

    void clearwin(){
        clear();
    }

    void print_text(int x, int y, _text_ * text){
        for(int i = 0; i < text->size(); i++){
            mvprintw(y+i, x, text->getline(i).c_str());
        }
    }

    void printl(int x, int y, std::string str){
        mvprintw(y, x, str.c_str());
    }

    void printlc(int x, int y, char * str){
        mvprintw(y, x, str);
    }

    void update_cursor(){
        move(cypos, cxpos);
    }

    void set_cursor(int x, int y){
        move(y, x);
    }
};


#endif /*window_hpp*/
