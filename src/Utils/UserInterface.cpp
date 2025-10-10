#include "UserInterface.hpp"
#include <iostream>
#include <iomanip>
#include <algorithm>
#include <cstdlib>

#ifdef _WIN32
#include <conio.h>
#else
#include <termios.h>
#include <unistd.h>
#endif

// Static member initialization
UserInterface* UserInterface::instance = nullptr;

// ==================== Singleton ====================

UserInterface* UserInterface::getInstance()
{
	if (instance == nullptr)
	{
		instance = new UserInterface();
	}
	return instance;
}

// ==================== Basic Output Methods ====================

void UserInterface::print(const string& message)
{
	std::cout << message;
	std::cout.flush();
}

void UserInterface::println(const string& message)
{
	std::cout << message << std::endl;
}

void UserInterface::printError(const string& message)
{
	std::cout << "\n[ERROR] " << message << std::endl;
}

void UserInterface::printSuccess(const string& message)
{
	std::cout << "\n[SUCCESS] " << message << std::endl;
}

void UserInterface::printWarning(const string& message)
{
	std::cout << "\n[WARNING] " << message << std::endl;
}

void UserInterface::printHeader(const string& header)
{
	println("\n" + string(50, '='));
	println("  " + header);
	println(string(50, '='));
}

void UserInterface::printSeparator()
{
	println(string(50, '-'));
}

// ==================== Input Methods ====================

string UserInterface::getString(const string& prompt)
{
	print(prompt);
	string input;
	std::getline(std::cin, input);
	return input;
}

int UserInterface::getInt(const string& prompt)
{
	while (true)
	{
		print(prompt);
		string input;
		std::getline(std::cin, input);
		
		try
		{
			// Remove leading/trailing whitespace
			input.erase(0, input.find_first_not_of(" \t\n\r"));
			input.erase(input.find_last_not_of(" \t\n\r") + 1);
			
			size_t pos;
			int value = std::stoi(input, &pos);
			
			// Check if entire string was converted
			if (pos == input.length())
			{
				return value;
			}
		}
		catch (...)
		{
			// Invalid input, will retry
		}
		
		printError("Invalid input. Please enter a valid integer.");
	}
}

double UserInterface::getDouble(const string& prompt)
{
	while (true)
	{
		print(prompt);
		string input;
		std::getline(std::cin, input);
		
		try
		{
			// Remove leading/trailing whitespace
			input.erase(0, input.find_first_not_of(" \t\n\r"));
			input.erase(input.find_last_not_of(" \t\n\r") + 1);
			
			size_t pos;
			double value = std::stod(input, &pos);
			
			// Check if entire string was converted
			if (pos == input.length())
			{
				return value;
			}
		}
		catch (...)
		{
			// Invalid input, will retry
		}
		
		printError("Invalid input. Please enter a valid number.");
	}
}

string UserInterface::getPassword(const string& prompt)
{
	print(prompt);
	string password;
	
#ifdef _WIN32
	// Windows implementation using _getch()
	char ch;
	while ((ch = _getch()) != '\r')  // '\r' is Enter key
	{
		if (ch == '\b')  // Backspace
		{
			if (!password.empty())
			{
				password.pop_back();
				std::cout << "\b \b";  // Erase character from console
			}
		}
		else
		{
			password += ch;
			std::cout << '*';
		}
	}
#else
	// Unix/Linux implementation using termios
	termios oldSettings, newSettings;
	tcgetattr(STDIN_FILENO, &oldSettings);
	newSettings = oldSettings;
	newSettings.c_lflag &= ~ECHO;  // Disable echo
	tcsetattr(STDIN_FILENO, TCSANOW, &newSettings);
	
	std::getline(std::cin, password);
	
	tcsetattr(STDIN_FILENO, TCSANOW, &oldSettings);  // Restore echo
#endif
	
	std::cout << std::endl;
	return password;
}

int UserInterface::getChoice(const string& prompt, int min, int max)
{
	while (true)
	{
		int choice = getInt(prompt);
		
		if (choice >= min && choice <= max)
		{
			return choice;
		}
		
		printError("Invalid choice. Please enter a number between " + 
		          std::to_string(min) + " and " + std::to_string(max) + ".");
	}
}

bool UserInterface::getYesNo(const string& prompt)
{
	while (true)
	{
		string input = getString(prompt + " (yes/no): ");
		
		// Convert to lowercase
		std::transform(input.begin(), input.end(), input.begin(),
		              [](unsigned char c){ return std::tolower(c); });
		
		if (input == "yes" || input == "y" || input == "Y" || input == "1")
		{
			return true;
		}
		else if (input == "no" || input == "n" || input == "N" || input == "0")
		{
			return false;
		}
		
		printError("Invalid input. Please enter 'yes' or 'no'.");
	}
}

string UserInterface::getDate(const string& prompt, const string& format)
{
	println("\nExpected format: " + format);
	return getString(prompt);
}

// ==================== Screen Control ====================

void UserInterface::clearScreen()
{
#ifdef _WIN32
	system("cls");
#else
	system("clear");
#endif
}

void UserInterface::pauseScreen()
{
	println("\nPress Enter to continue...");
	std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
}

// ==================== Display Utilities ====================

void UserInterface::displayMenu(const string& title, const vector<string>& options)
{
	printHeader(title);
	
	for (size_t i = 0; i < options.size(); ++i)
	{
		println(std::to_string(i + 1) + ". " + options[i]);
	}
	
	printSeparator();
}

void UserInterface::displayTable(const vector<string>& headers, const vector<vector<string>>& rows)
{
	if (headers.empty())
	{
		return;
	}
	
	// Calculate column widths
	vector<size_t> colWidths(headers.size(), 0);
	
	for (size_t i = 0; i < headers.size(); ++i)
	{
		colWidths[i] = headers[i].length();
	}
	
	for (const auto& row : rows)
	{
		for (size_t i = 0; i < row.size() && i < colWidths.size(); ++i)
		{
			colWidths[i] = std::max(colWidths[i], row[i].length());
		}
	}
	
	// Print headers
	println("");
	for (size_t i = 0; i < headers.size(); ++i)
	{
		std::cout << std::left << std::setw(colWidths[i] + 2) << headers[i];
	}
	println("");
	
	// Print separator
	for (size_t i = 0; i < headers.size(); ++i)
	{
		std::cout << string(colWidths[i] + 2, '-');
	}
	println("");
	
	// Print rows
	for (const auto& row : rows)
	{
		for (size_t i = 0; i < row.size() && i < headers.size(); ++i)
		{
			std::cout << std::left << std::setw(colWidths[i] + 2) << row[i];
		}
		println("");
	}
	println("");
}

// ==================== Formatting Utilities ====================

string UserInterface::formatCurrency(double amount)
{
	std::ostringstream oss;
	oss << std::fixed << std::setprecision(2) << "$" << amount;
	return oss.str();
}