#include <iostream>
#include <cstdlib>
#include <limits>
#include <climits>
#include <algorithm>

void clear_screen() {
#ifdef _WIN32
    system("cls"); 
#else
    system("clear");
#endif
}

void draw_board(char *spaces);
void player_move(char *spaces , char player);
void computer_move(char *spaces , char player , char computer);
bool check_winner(char *spaces , char player , char computer);
bool check_draw(char *spaces);

int evaluate_board(char *spaces , char player , char computer);
bool board_full(char *spaces);
int minimax(char *spaces , bool is_maximining , char player , char computer);
int get_best_move(char *spaces , char player , char computer);

int main(){

    char spaces[] = {' ',' ',' ',' ',' ',' ',' ',' ',' ' }; 
    char player = 'X';
    char computer = 'O';
    bool running = true;

    draw_board(spaces);

    while(running){
        player_move(spaces , player);
        clear_screen();
        draw_board(spaces);
        if(check_winner(spaces , player , computer)){
            running = false;
            break;
        }
        else if(check_draw(spaces)){
            running = false;
            break;
        }
        computer_move(spaces , player , computer);
        clear_screen();
        draw_board(spaces);
        if(check_winner(spaces , player , computer)){
            running = false;
            break;
        }
        else if(check_draw(spaces)){
            running = false;
            break;
        }
        
    }

    std::cout << "Thanks for playing\n";

    return 0;
}


void draw_board(char *spaces){
    std::cout << "   |   |   \n";
    std::cout << " " << spaces[0] << " | "<< spaces[1] << " | "<< spaces[2] << " \n";
    std::cout << "___|___|___\n";
    std::cout << "   |   |   \n";
    std::cout << " " << spaces[3] << " | "<< spaces[4] << " | "<< spaces[5] << " \n";
    std::cout << "___|___|___\n";
    std::cout << "   |   |   \n";
    std::cout << " " << spaces[6] << " | "<< spaces[7] << " | "<< spaces[8] << " \n";
    std::cout << "   |   |   \n";
}


void player_move(char *spaces , char player){
    int number = -1;
    while(true){
        std::cout << "Enter a spot to place a marker(1-9): ";
        std::cin >> number;
        number--;

        if (number >= 0 && number <= 8 && spaces[number] == ' '){
            spaces[number] = player;
            break;
        }
        else if(number < 0 || number > 8){
            std::cout << "Please enter a valid number only\n";
        }
        else{
            std::cout << "That spot is already taken , please pick a different spot\n";
        }

        std::cin.clear();
        std::cin.ignore(std::numeric_limits<std::streamsize>::max() , '\n');
    }
}

void computer_move(char *spaces , char player , char computer){
    int move = get_best_move(spaces , player , computer);
    if (move != -1){
        spaces[move] = computer;
    }
}

bool check_winner(char *spaces , char player , char computer){

    if(spaces[0] != ' ' && spaces[0] == spaces[1] && spaces[1] == spaces[2]){
        spaces[0] == player ? std::cout << "YOU WON!!!\n" : std::cout << "You lose\n";
    }
    else if(spaces[3] != ' ' && spaces[3] == spaces[4] && spaces[4] == spaces[5]){
        spaces[3] == player ? std::cout << "YOU WON!!!\n" : std::cout << "You lose\n";
    }
    else if(spaces[6] != ' ' && spaces[6] == spaces[7] && spaces[7] == spaces[8]){
        spaces[6] == player ? std::cout << "YOU WON!!!\n" : std::cout << "You lose\n";
    }
    else if(spaces[0] != ' ' && spaces[0] == spaces[3] && spaces[3] == spaces[6]){
        spaces[0] == player ? std::cout << "YOU WON!!!\n" : std::cout << "You lose\n";
    }
    else if(spaces[1] != ' ' && spaces[1] == spaces[4] && spaces[4] == spaces[7]){
        spaces[1] == player ? std::cout << "YOU WON!!!\n" : std::cout << "You lose\n";
    }
    else if(spaces[2] != ' ' && spaces[2] == spaces[5] && spaces[5] == spaces[8]){
        spaces[2] == player ? std::cout << "YOU WON!!!\n" : std::cout << "You lose\n";
    }
    else if(spaces[0] != ' ' && spaces[0] == spaces[4] && spaces[4] == spaces[8]){
        spaces[0] == player ? std::cout << "YOU WON!!!\n" : std::cout << "You lose\n";
    }
    else if(spaces[2] != ' ' && spaces[2] == spaces[4] && spaces[4] == spaces[6]){
        spaces[2] == player ? std::cout << "YOU WON!!!\n" : std::cout << "You lose\n";
    }
    else{
        return false;
    }
    return true;
}

bool check_draw(char *spaces){
    for (int i = 0 ; i < 9 ; i++){
        if(spaces[i] == ' '){
            return false;
        }
    }
    std::cout << "It's a Tie\n";
    return true;
}


int evaluate_board(char *spaces , char player , char computer){
    int lines[8][3] =   {{0 ,1 ,2} , {3 ,4 ,5} , {6 , 7,8} , {0 ,3 ,6},
                        {1 ,4 ,7} , {2 ,5 ,8} , {0 , 4,8} , {2 ,4 ,6}};

    for(auto &line : lines){
        if (spaces[line[0]] != ' ' && spaces[line[0]] == spaces[line[1]] &&
        spaces[line[1]] == spaces[line[2]]){
            if(spaces[line[0]] == computer){return 1;}
            if (spaces[line[0]] == player){return -1;}
        }
    }
    return 0; 
}

bool board_full(char *spaces){
    for(int i = 0 ; i < 9 ; i++){
        if(spaces[i] == ' '){return false;}
    }
    return true;
}

int minimax(char *spaces , bool is_maximining , char player , char computer){
    int score = evaluate_board(spaces , player , computer);
    
    if (score == 1){return 1;}
    if (score == -1){return -1;}

    if(board_full(spaces)){return 0;}

    if (is_maximining){
        int best = INT_MIN;
        for (int i = 0; i < 9 ; i++){
            if(spaces[i] == ' '){
                spaces[i] = computer;
                int current = minimax(spaces , false , player , computer);
                spaces[i] = ' ';
                if (current > best){
                    best = current;
                }
            }
        }
        return best;
    }
    else{
        int best = INT_MAX;
        for (int i = 0 ; i < 9 ; i++){
            if(spaces[i] == ' '){
                spaces[i] = player;
                int current = minimax(spaces , true , player , computer);
                spaces[i] = ' ';
                if (current < best){
                    best = current;
                }
            }
        }
        return best;
    }
}

int get_best_move(char *spaces , char player , char computer){
    int best_score = INT_MIN;
    int best_move = -1;

    for(int i = 0; i < 9; i++){
        if (spaces[i] == ' '){
            spaces[i] =  computer;
            int movescore = minimax(spaces , false , player , computer);
            spaces[i] = ' ';
            
            if(movescore > best_score){
                best_score = movescore;
                best_move = i;
            }
        }
    }
    return best_move;
}




