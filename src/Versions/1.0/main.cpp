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
bool coding = false;


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



void display(){
    if(printing){
        win.clearwin();

        if(show_info){
            win.printl(0, 0, file.get_name() + " | C: " + std::to_string(win.cxpos) + "/" + std::to_string(text.get_size(win.cypos)) + ", L: " + std::to_string(win.cypos+1) + "/" + std::to_string(text.size()) + " | Total: " + std::to_string(text.get_total()) + (coding ? " | [C] " : ""));
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
}



bool rename(int x, bool pass_value){
    int buffer = x;
    int old_cxpos = win.cxpos;
    int old_cypos = win.cypos;
    std::string name_buffer = file.get_name();
    win.cxpos = name_buffer.size();
    if(pass_value){
        win.cxpos = buffer;
    }
    bool renaming = true;

    win.set_cursor(win.cxpos, 0);

    while(renaming){

        win.get_event();

        if(win.get() == LF){
            renaming = false;
        }
        else if(win.get() == ESC){
            win.cxpos = old_cxpos;
            win.cypos = old_cypos;
            return false;
        }
        else if(win.get() == DELETE){
            if(win.cxpos > 0){
                name_buffer.erase(name_buffer.begin()+win.cxpos-1);
                win.cxpos--;
            }
        }
        else if(win.get() == FN_DELETE){
            name_buffer.erase(name_buffer.begin()+win.cxpos);
        }
        else if(win.get() == KEY_RIGHT or win.get() == KEY_DOWN){
            if(win.cxpos < name_buffer.size()){
                win.cxpos++;
            }
        }
        else if(win.get() == KEY_LEFT or win.get() == KEY_UP){
            if(win.cxpos > 0){
                win.cxpos--;
            }
        }
        else if(win.get() == KEY_MOUSE){
            if(getmouse(&win.mouse) == OK){
                if(win.mouse.bstate & BUTTON1_CLICKED){
                    win.mxpos = win.mouse.x;
                    win.mypos = win.mouse.y;

                    if(win.mypos == 0){
                        if(win.mxpos <= name_buffer.size()){
                            win.cxpos = win.mxpos;
                        }
                        else{
                            win.cxpos = name_buffer.size();
                        }
                    }
                    else{
                        renaming = false;
                        if(win.mypos-1+line < text.size()){
                            if(win.mxpos < text.get_size(win.mypos-1+line)){
                                old_cxpos = win.mxpos;                          //overwrite
                            }
                            else{
                                old_cxpos = text.get_size(win.mypos-1+line);
                            }
                            old_cypos = win.mypos-1+line;
                        }
                        else{
                            old_cypos = text.size()-1;
                            old_cxpos = text.get_size(old_cypos);
                        }
                    }
                }
            }
        }
        else if(win.get() < 128){
            name_buffer.insert(name_buffer.begin() + win.cxpos, win.get());
            win.cxpos++;
        }

        win.clearwin();

        win.printl(0, 0, name_buffer);

        int linebuffer = line;
        for(int i = 0; i < win.height; i++){
            win.printl(0, i+1, text.getline(linebuffer));
            linebuffer++;
            if(linebuffer == text.size()){
                break;
            }
        }

        win.set_cursor(win.cxpos, 0);
    }

    file.set_name(name_buffer);
    win.cxpos = old_cxpos;
    win.cypos = old_cypos;
    return true;
}


int pos_not_char(_text_ * text, int line, char chr){
    int pos = 0;
    std::string buffer = text->getline(line);

    for(int i = 0; i < buffer.size(); i++){
        if(buffer[i] != chr){
            pos = i;
            break;
        }
        else if(i == buffer.size()-1){
            pos = i+1;
            break;
        }
    }

    return pos;
}

int pos_is_char(_text_ * text, int line, char chr){
    int pos = 0;
    std::string buffer = text->getline(line);

    for(int i = 0; i < buffer.size(); i++){
        if(buffer[i] == chr){
            pos = i;
            break;
        }
        else if(i == buffer.size()-1){
            pos = i+1;
            break;
        }
    }

    return pos;
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
        if(coding){
            if(text.getchr(win.cxpos-1, win.cypos) == '{' and text.getchr(win.cxpos, win.cypos) != '}'){
                text.newline(win.cxpos, win.cypos);
                win.cxpos = pos_not_char(&text, win.cypos, SPACE) + 4;           //+4 = TAB size
                win.cypos++;
                for(int spaces = 0; spaces < win.cxpos; spaces++){
                    text.add(0, win.cypos, SPACE);
                }
            }
            else if(text.getchr(win.cxpos-1, win.cypos) == '{' and text.getchr(win.cxpos, win.cypos) == '}'){
                text.newline(win.cxpos, win.cypos);
                text.newline(0, win.cypos+1);
                for(int spaces1 = 0; spaces1 < pos_not_char(&text, win.cypos, SPACE)+4; spaces1++){
                    text.add(0, win.cypos+1, SPACE);
                }
                for(int spaces2 = 0; spaces2 < pos_not_char(&text, win.cypos, SPACE); spaces2++){
                    text.add(0, win.cypos+2, SPACE);
                }
                win.cxpos = pos_not_char(&text, win.cypos, SPACE)+4;
                win.cypos++;
            }
            else{
                text.newline(win.cxpos, win.cypos);
                win.cxpos = pos_not_char(&text, win.cypos, SPACE);
                for(int spaces = 0; spaces < win.cxpos; spaces++){
                    text.add(0, win.cypos+1, SPACE);
                }
                win.cypos++;
                lxpos = win.cxpos;
            }
        }
        else{
            text.newline(win.cxpos, win.cypos);
            win.cxpos = 0;
            win.cypos++;
            lxpos = 0;
        }
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
                win.mypos = win.mouse.y+line;
                if(win.mypos+line <= text.size() and win.mypos-line > 0){
                    if(win.mxpos < text.get_size(win.mypos-1)){                 //e.g.: 6 < text.get_size(1-1)
                        win.cxpos = win.mxpos;                                  //                          ^because of first line being file name
                        win.cypos = win.mypos-1;
                        lxpos = win.cxpos;
                    }
                    else{
                        win.cxpos = text.get_size(win.mypos-1);
                        win.cypos = win.mypos-1;
                        lxpos = win.cxpos;
                    }
                }
                else if(win.mypos-line == 0){
                    if(win.mxpos < file.get_name().size()){
                    rename(win.mxpos, true);
                    }
                    else{
                    rename(file.get_name().size(), true);
                    }
                }
                else{
                    win.cypos = text.size()-1;
                    win.cxpos = text.get_size(win.cypos);
                }
            }
        }
    }
    else if(win.get() == OPTION_SHIFT_C){
        coding ? coding = false : coding = true;
    }
    else if(win.get() == OPTION_SHIFT_S){
        file.write(text.get_text());
    }
    else if(win.get() == OPTION_SHIFT_I){
        show_info ? show_info = false: show_info = true;
    }
    else if(win.get() == OPTION_SHIFT_R){
        rename(0, false);
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
        if(coding){
            if(win.get() == '{'){
                text.add(win.cxpos+1, win.cypos, '}');
            }
            else if(win.get() == '('){
                text.add(win.cxpos+1, win.cypos, ')');
            }
            else if(win.get() == '['){
                text.add(win.cxpos+1, win.cypos, ']');
            }
            else if(win.get() == '\''){
                text.add(win.cxpos+1, win.cypos, '\'');
            }
            else if(win.get() == '\"'){
                text.add(win.cxpos+1, win.cypos, '\"');
            }
        }
        win.cxpos++;
        lxpos = win.cxpos;
    }
    set_line();
}




void run(){
    win.get_event();
    check();
    display();
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
