#include <iostream>
#include <limits>
#include <iomanip>
#include <vector>
#include <string> 

void show_balance(double balance);
double deposit(double balance);
double withdraw(double balance);
void show_history(const std::vector<std::string>& history);

int main(){
    int choice;
    double balance = 86400;
    double amount = 0;
    std::vector<std::string> history;
    
    do{
        std::cout << "\nBanking counter" << std::endl
        << "1. Check balance\n"
        << "2. Deposit\n"
        << "3. Withdraw\n"
        << "4. Transaction history\n"
        << "5. Exit\n"
        << "Enter your choice: ";
        
        std::cin >> choice;
        std::cin.clear();
        std::cin.ignore(std::numeric_limits<std::streamsize>::max() , '\n');

        switch (choice)
        {
        case 1:
            show_balance(balance);
            break;
        case 2:
            amount = deposit(balance);
            balance += amount;
            show_balance(balance);
            
            history.push_back("Deposited: +₹" + std::to_string(amount)) ;
            break;
        case 3:
            amount = withdraw(balance);
            balance -= amount;
            show_balance(balance);

            if (amount > 0){
            history.push_back("Withdrawn: -₹" + std::to_string(amount));
            }
            break;
        case 4:
            show_history(history);
            break;
        case 5:
            std::cout << "Thanks for visiting\n";
            break;
        default:
            std::cout << "Please enter a valid option\n";
            break;
        }
    }while(choice != 5);
    return 0;
}

void show_balance(double balance){
    std::cout << "Your balance is: ₹" << std::setprecision(2) << std::fixed <<  balance << std::endl;
}

double deposit(double balance){
    
    double deposit = 0;
    while(deposit <= 0){
        std::cout << "Enter the amount you want to deposit: ";
        std::cin >> deposit;
        if (deposit < 0){
            std::cout << "Your deposit can't be negative\n"
            << "Please enter a valid amount\n\n";
            continue;
        }
        else if(deposit > 0){
            return deposit;
            break;
        }
        std::cin.clear();
        std::cin.ignore(std::numeric_limits<std::streamsize>::max() , '\n');
        std::cout << "Please enter valid numeric inputs only\n";
    }
    return 0;
}

double withdraw(double balance){
    double withdraw = 0;
    while(withdraw <= 0){
        std::cout << "Enter the amount you want to withdraw: ";
        std::cin >> withdraw;

        if (withdraw > balance){
            std::cout << "You don't have enough balance\n";
            break;
        }
        if (withdraw < 0){
            std::cout << "Your withdrawl amount can't be negative\n"
            << "Please enter a valid amount\n\n";
            continue;
        }
        else if(withdraw > 0){
            return withdraw;
            break;
        }
        std::cin.clear();
        std::cin.ignore(std::numeric_limits<std::streamsize>::max() , '\n');
        std::cout << "Please enter valid numeric inputs only\n";

    }
    return 0;
}

void show_history(const std::vector<std::string>& history){
    if(history.empty()){
        std::cout << "No transaction record\n";
        return;
    }

    int count = 0;
    std::cout << "\nRecent transaction history\n";
    
    for (int i = history.size() - 1 ; i >= 0 && count < 5 ; i--){
        std::cout << history[i] << std::endl;
        count++;
    }
}