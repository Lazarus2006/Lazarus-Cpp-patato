#include <iostream>
#include <string>
#include <string.h>
#include <unistd.h>

int main(){
    std::string alphabet = " ABCDEFGHIJKLMNOPQRSTUVWXYZ";
    std::string word = "HELLO WORLD";
    std::string printer = "";

    for(int i = 0; i < word.length() ; i++){
        for(int j = 0; j < alphabet.length(); j++){
        printf("\r \033[32m%s\033[0;31m%c",  printer.c_str() , alphabet[j]);
        fflush(stdout);
        usleep(130000);
        if(alphabet[j] == word[i]){
            printer += alphabet[j]; 
            break;
        }
        }
    }
    printf("\r \033[32m%s\033[0m\n", printer.c_str());
    return 0;
}