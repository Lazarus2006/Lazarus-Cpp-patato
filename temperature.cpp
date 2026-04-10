#include <iostream>
#include <limits>
#include <string>
#include <cctype>

char getunit();
double get_temp(char unit);


int main(){
    double temp;
    char unit;

    std::cout << "Temperature conversion" << std:: endl << "F = Fahrenheit\nC = Celsius\n";
    
    unit = getunit();
    temp = get_temp(unit);
    
    

    switch (unit)
    {
    case 'F':
        temp = (temp - 32) / 1.8;
        break;
    case 'C':
        temp = (temp * 1.8) + 32;
        break;
    default:
        break;
    }

    std::cout << "The Temperature in " << ((unit != 'C')?"Celsius: " : "Fahrenheit: ") << "is: " 
    << temp << std::endl; 


    return 0;
}

char getunit(){
    std::string input;

    std::cout << "What unit would you like to convert to: " << std::flush;
    while (true) {
        if (std::cin >> input && input.size() == 1 &&
            (input[0] == 'C' || input[0] == 'c' ||
            input[0] == 'F' || input[0] == 'f')) {
            break;
        } else {
            std::cin.clear();
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
            std::cout << "Please enter a valid unit (C or F): ";
        }
    }

    return std::toupper(input[0]);
}

double get_temp(char unit){
    double value = '\0';
    std::cout << "Enter the temperature in " << ((unit == 'C')?"Celsius: " : "Fahrenheit: ");
    while (!(std::cin >> value)){
        std::cout << "Invalid input. Enter a number: ";
        std::cin.clear();
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
    }
    std::cin.clear();
    std::cin.ignore(std::numeric_limits<std::streamsize>::max() , '\n');
    return value;
}