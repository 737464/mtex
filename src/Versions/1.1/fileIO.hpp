#ifndef fileIO_hpp
#define fileIO_hpp

#include <fstream>
#include <string>

class _file_{
    std::ifstream input;
    std::ofstream output;
    std::string name;
    std::string buffer;

public:
    _file_(){}
    ~_file_(){}

    void set_name(std::string set_name){
        name = set_name;
    }

    std::string get_name(){
        return name;
    }

    std::string read(){
        buffer.clear();
        input.open(name);

        if(input.good()){
            while(input.good()){
                buffer += input.get();
            }

            buffer.pop_back();                                                  //remove EOF character

            input.close();
        }
        else{
                                                                                //add error
        }

        return buffer;
    }

    bool write(std::string content){
        output.open(name);

        if(output.good()){

            output << content;

            output.close();

            return true;
        }
        return false;
    }

};

#endif /*fileIO_hpp*/
