#ifndef text_hpp
#define text_hpp

#include <string>
#include <vector>
#include "definitions.hpp"

class _text_{
    std::string line_buffer;                                                    //to store copied part of a line
    std::vector<std::string> text;                                              //contains the text. Each string represents a line
    std::vector<std::string> original;                                          //contains the original text.

public:
    _text_(){
        text.push_back(std::string());
    }
    ~_text_(){}

    bool add(int pos, int line, char chr){
        if(line < text.size()){                                                 //line is in range of text
            if(pos <= text[line].size()){                                        //pos is in range of selected line
                text[line].insert(text[line].begin()+pos, chr);                 //insert given char chr in text at line, position pos
                return true;
            }
        }
        return false;
    }

    bool newline(int pos, int line){                                            //inserts new string at position
        if(line < text.size()){                                                 //line is in range of text
            if(pos <= text[line].size()){                                       //pos is in range of selected line
                line_buffer = text[line];
                line_buffer.erase(line_buffer.begin(), line_buffer.begin()+pos);
                text.insert(text.begin()+line+1, line_buffer);
                text[line].erase(text[line].begin()+pos, text[line].end());
                return true;
            }
        }
        return false;
    }

    bool remove(int pos, int line){
        if(line < text.size()){                                                 //line is in range of text
            if(pos <= text[line].size() and pos >= 0){                           //pos is in range of selected line but not 0
                text[line].erase(text[line].begin()+pos);                       //remove given char chr in text at line, position pos
                return true;
            }
            else if(pos == SOL){                                                //if pos is beginning of line delete "LF"
                line_buffer = text[line];                                       //copy line to line_buffer
                text.erase(text.begin()+line);                                  //delete line
                text[line-1].append(line_buffer);                               //add line_buffer to line above delete line
                return true;
            }
        }
        return false;
    }

    char getchr(int pos, int line){
        if(line < text.size()){
            if(pos < text[line].size()){
                return text[line][pos];
            }
        }
        return ' ';
    }

    std::string get_text(){
        std::string retstr;                                                     //string to be returned from function
        for(int line = 0; line < text.size(); line++){
            retstr.append(text[line]);                                          //add line line from text to retstr
            if(line < text.size()-1){                                           //if not last line, add LF
                retstr += "\n";
            }
        }
        return retstr;
    }

    int get_size(int line){
        if(line < text.size()){
            return text[line].size();
        }
        return 0;
    }

    int size(){
        return text.size();
    }

    std::string getline(int line){
        if(line < text.size()){
            return text[line];
        }
        else{
            return 0;
        }
    }

    void set_text(std::vector<std::string> content){
        text = content;
    }

    void set_original(std::vector<std::string> content){
        original = content;
    }

    void set_original_to_text(){
        original = text;
    }

    int get_total(){
        int buffer = 0;

        for(int i = 0; i < text.size(); i++){
            buffer += text[i].size();
        }
        buffer += text.size()-1;

        return buffer;
    }

    bool is_original(){
        if(text == original){
            return true;
        }
        return false;
    }
};

#endif /*text_hpp*/
