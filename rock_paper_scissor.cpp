#include <iostream>
#include <random>
#include <string.h>
#include <algorithm>

void greeting();
int get_random();
int get_user();
void result(int computer , int user);

int main(){
    std::cout << "Welcome to rock paper scissors\n";
    greeting();
    int computer = get_random();
    int user = get_user();
    result(computer , user);

    return 0;
}

void greeting(){
    std::cout 
    << "1. Rock\n"
    << "2. Paper\n"
    << "3. Scissor\n";
}

int get_random(){
    std::random_device rd; 
    std::mt19937 gen(rd()); 
    std::uniform_int_distribution<> dis(1, 3);
    int randomNum = dis(gen);
    return randomNum;
}

int get_user(){
    std::string choice;
    while(true){
        std::cout << "Enter your choice: ";
        std::cin >> choice;

        std::transform(choice.begin(), choice.end(), choice.begin(), ::toupper);
        
        if(choice == "1" || choice == "ROCK"){return 1;}
        else if(choice == "2" || choice == "PAPER"){return 2;}
        else if(choice == "3" || choice == "SCISSOR"){return 3;}
        else{std::cout << "Please Enter a valid choice\n";}
    }
}

void result(int computer , int user){
    switch (computer)
    {
    case 1: std::cout << "The computer choose Rock\n"; break;
    case 2: std::cout << "The computer choose Paper\n"; break;
    case 3: std::cout << "The computer choose Scissor\n"; break;
    }

    if (computer == user){std::cout << "It's a draw\n";}
    else if(computer == 1){
        if(user == 2){ std::cout << "You won\n";}
        else{ std::cout << "You lose\n";}}
    else if(computer == 2){
        if(user == 3){ std::cout << "You won\n";}
        else{ std::cout << "You lose\n";}}
    else if(computer == 3){
        if(user == 1){ std::cout << "You won\n";}
        else{ std::cout << "You lose\n";}}
}
