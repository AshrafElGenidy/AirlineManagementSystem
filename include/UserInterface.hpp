#ifndef USERINTERFACE_HPP
#define USERINTERFACE_HPP

#include <string>
#include <vector>
#include <functional>
#include <sstream>
#include <limits>

using std::string;
using std::vector;

class UserInterface
{
private:
	static UserInterface* instance;
	
	// Private constructor for singleton
	UserInterface() = default;
	
	// Delete copy constructor and assignment operator
	UserInterface(const UserInterface&) = delete;
	UserInterface& operator=(const UserInterface&) = delete;

public:
	// Singleton accessor
	static UserInterface* getInstance();
	
	// Basic output methods
	void print(const string& message);
	void println(const string& message);
	void printError(const string& message);
	void printSuccess(const string& message);
	void printWarning(const string& message);
	void printHeader(const string& header);
	void printSeparator();
	
	// Input methods
	string getString(const string& prompt);
	int getInt(const string& prompt);
	double getDouble(const string& prompt);
	string getPassword(const string& prompt);
	int getChoice(const string& prompt, int min, int max);
	bool getYesNo(const string& prompt);
	string getDate(const string& prompt, const string& format = "YYYY-MM-DD HH:MM");
	
	// Screen control
	void clearScreen();
	void pauseScreen();
	
	// Display utilities
	void displayMenu(const string& title, const vector<string>& options);
	void displayTable(const vector<string>& headers, const vector<vector<string>>& rows);
	
	// Formatting utilities
	string formatCurrency(double amount);	
};

#endif // USERINTERFACE_HPP