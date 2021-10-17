#include <iostream>
#include "definitions.hpp"
#include "fileIO.hpp"
#include "text.hpp"
#include "window.hpp"

_text_ text;
_window_ win;
_file_ file;







bool running = true;
int lxpos;                                                                      //stores the last x pos (for changing lines)
bool printing = true;                                                           //false if screen is resized or font is scaled
int line = 0;                                                                   //buffer that stores scrolled lines
bool show_info;


void set_line(){
    line = win.cypos - win.height + 2;
    if(line < 0){
        line = 0;
    }
}

void setup(){
    win.setup();
    win.printl(0, 0, file.get_name());
    win.print_text(0, 1, &text);
    win.set_cursor(0, 1);
}

void check(){
    printing = true;
    if(win.get() == DELETE){
        if(win.cxpos > 0){
            text.remove(win.cxpos-1, win.cypos);
            win.cxpos--;
            lxpos = win.cxpos;
        }
        else if(win.cxpos == 0){
            if(win.cypos > 0){
                win.cypos--;
                win.cxpos = text.get_size(win.cypos);
                text.remove(SOL, win.cypos+1);
            }
        }
    }
    else if(win.get() == TAB){
        text.add(win.cxpos, win.cypos, ' ');
        text.add(win.cxpos, win.cypos, ' ');
        text.add(win.cxpos, win.cypos, ' ');
        text.add(win.cxpos, win.cypos, ' ');
        win.cxpos += 4;
    }
    else if(win.get() == LF){
        text.newline(win.cxpos, win.cypos);
        win.cxpos = 0;
        win.cypos++;
        lxpos = 0;
    }
    else if(win.get() == KEY_UP){
        if(win.cypos > 0){
            if(text.get_size(win.cypos-1) < lxpos){
                win.cxpos = text.get_size(win.cypos-1);
                win.cypos--;
            }
            else{
                win.cxpos = lxpos;
                win.cypos--;
            }
        }
        else{
            if(win.cxpos > 0){
                win.cxpos--;
                lxpos = win.cxpos;
            }
        }
    }
    else if(win.get() == KEY_DOWN){
        if(win.cypos < text.size()-1){
            if(text.get_size(win.cypos+1) < lxpos){
                win.cxpos = text.get_size(win.cypos+1);
                win.cypos++;
            }
            else{
                win.cxpos = lxpos;
                win.cypos++;
            }
        }
        else{
            if(win.cxpos < text.get_size(win.cypos)){
                win.cxpos++;
                lxpos = win.cxpos;
            }
        }
    }
    else if(win.get() == KEY_RIGHT){
        if(win.cxpos < text.get_size(win.cypos)){
            win.cxpos++;
            lxpos = win.cxpos;
        }
        else{
            if(win.cypos < text.size()-1){
                win.cxpos = 0;
                win.cypos++;
                lxpos = win.cxpos;
            }
        }
    }
    else if(win.get() == KEY_LEFT){
        if(win.cxpos > 0){
            win.cxpos--;
            lxpos = win.cxpos;
        }
        else{
            if(win.cypos > 0){
                win.cxpos = text.get_size(win.cypos-1);
                win.cypos--;
                lxpos = win.cxpos;
            }
        }
    }
    else if(win.get() == KEY_MOUSE){
        if(getmouse(&win.mouse) == OK){
            if(win.mouse.bstate & BUTTON1_CLICKED){
                win.mxpos = win.mouse.x;
                win.mypos = win.mouse.y-1;                                      //-1 because of first line displaying filename
                if(win.mypos < text.size() and win.mypos > 0){
                    if(win.mxpos < text.get_size(win.mypos)){
                        win.cxpos = win.mxpos;
                        win.cypos = win.mypos;
                        lxpos = win.cxpos;
                    }
                    else{
                        win.cxpos = text.get_size(win.mypos);
                        win.cypos = win.mypos;
                        lxpos = win.cxpos;
                    }
                }
                else{
                    win.cypos = text.size()-1;
                    win.cxpos = text.get_size(win.cypos);
                }
            }
        }
    }
    else if(win.get() == OPTION_SHIFT_S){
        file.write(text.get_text());
    }
    else if(win.get() == OPTION_SHIFT_I){
        show_info ? show_info = false: show_info = true;
    }
    else if(win.get() == OPTION_SHIFT_Q){
        running = false;
    }
    else if(win.get() == FN_DELETE){
        if(win.cxpos < text.get_size(win.cypos)){
            text.remove(win.cxpos, win.cypos);
        }
        else if(win.cxpos == text.get_size(win.cypos)){
            if(win.cypos+1 < text.size()){
                text.remove(SOL, win.cypos+1);
            }
        }
    }
    else if(win.get() >= 0 and win.get() < 141){
        text.add(win.cxpos, win.cypos, win.get());
        win.cxpos++;
        lxpos = win.cxpos;
    }
    set_line();
}

void run(){
    win.get_event();
    check();
    if(printing){
        win.clearwin();

        if(show_info){
            win.printl(0, 0, file.get_name() + " | C: " + std::to_string(win.cxpos) + "/" + std::to_string(text.get_size(win.cypos)) + ", L: " + std::to_string(win.cypos+1) + "/" + std::to_string(text.size()) + " | Total: " + std::to_string(text.get_total()));
        }
        else{
            win.printl(0, 0, file.get_name());
        }

        int linebuffer = line;
        for(int i = 0; i < win.height; i++){
            win.printl(0, i+1, text.getline(linebuffer));
            linebuffer++;
            if(linebuffer == text.size()){
                break;
            }
        }
    }
    win.set_cursor(win.cxpos, win.cypos+1-line);                               //+1 because of first line displaying filename
}

std::vector<std::string> str_to_vecstr(std::string str){
    std::string buffer = "";
    std::vector<std::string> retstr;

    for(int i = 0; i < str.size(); i++){
        if(str[i] == '\n'){
            retstr.push_back(buffer);
            buffer.clear();
        }
        else{
            buffer += str[i];
        }
    }
    retstr.push_back(buffer);
    return retstr;
}





int main(int argc, const char * argv[]){

    file.set_name("Unnamed.txt");

    if(argc > 1){                                                               //open passed files
        file.set_name(argv[1]);
        text.set_text(str_to_vecstr(file.read()));
    }

    setup();

    while(running){
        run();
    }

    win.quit();

    return 0;
}
