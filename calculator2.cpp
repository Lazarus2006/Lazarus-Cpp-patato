#include <iostream>
#include <limits>

double getnumber(int number);
char getsign();

int main(){

    double num1 = getnumber(1);
    char sign = getsign();
    double num2 = getnumber(2);
    double result = 0;

    switch (sign)
    {
    case '+':
        result = num1 + num2;
        break;
    case '-':
        result = num1 - num2;
        break;
    case '*':
        result = num1 * num2;
        break;
    case '/':
        if(num2 == 0){std::cout << "Can't divide by zero\n";return 0;}
        result = num1 / num2;
        break;
    default:
        std::cout << "Please enter a valid sign\n";return 0;
    }

    std::cout << "The result is: " << result << std::endl;
    return 0;
}

double getnumber(int number){
    double output = 0;
    std::cout << "Enter The #" << number << " number: ";
    while (!(std::cin >> output)){
        std::cout << "Invalid input. Enter a number: ";
        std::cin.clear();
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
    }
    std::cin.clear();
    std::cin.ignore(std::numeric_limits<std::streamsize>::max() , '\n');

    return output;
}

char getsign(){
    char sign = '\0';
    std::cout << "Enter the sign( + , - , * , / ): "; std::cin >> sign;
    while(sign != '+' && sign != '-' && sign != '*' && sign != '/' ){
        std::cin.clear();
        std::cin.ignore(std::numeric_limits<std::streamsize>::max() , '\n');
        std::cout << "Invalid operation , Enter a valid sign ( + , - , * , / ): "; std::cin >> sign;
    }
    std::cin.clear();
    std::cin.ignore(std::numeric_limits<std::streamsize>::max() , '\n');
    return sign;
}