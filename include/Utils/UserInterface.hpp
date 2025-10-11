#ifndef USERINTERFACE_HPP
#define USERINTERFACE_HPP

#include <string>
#include <vector>
#include <functional>
#include <sstream>
#include <limits>
#include <exception>

using std::string;
using std::vector;

// ==================== Grid Display Configuration ====================

struct GridDisplayConfig
{
	string title;
	vector<string> headerLines;  // Multiple lines above grid
	vector<string> legend;       // Legend items
	vector<string> footerLines;  // Summary info below grid
	bool showSeparator = true;
};

// ==================== UserInterface Class ====================

class UserInterface
{
private:
	static UserInterface* instance;
	
	// Private constructor for singleton
	UserInterface() = default;
	
	// Delete copy, move constructors and assignment operators
	UserInterface(const UserInterface&) = delete;
	UserInterface(UserInterface&&) = delete;
	UserInterface& operator=(const UserInterface&) = delete;
	UserInterface& operator=(UserInterface&&) = delete;

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
	
	// Input methods - now throw UIException on invalid input
	// Callers should catch and handle, or retry themselves
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
	void displayGrid(const vector<string>& rowLabels,
	                 const vector<vector<string>>& gridData,
	                 const GridDisplayConfig& config = {});
	
	// Formatting utilities
	string formatCurrency(double amount);	
};

// ==================== UI Exception Class ====================

enum class UIErrorCode
{
	INVALID_INTEGER_INPUT,
	INVALID_DOUBLE_INPUT,
	INVALID_CHOICE,
	INVALID_YES_NO_INPUT,
	USER_CANCELED,
	INVALID_PASSWORD_INPUT
};

class UIException : public std::exception
{
private:
	UIErrorCode errorCode;
	string getErrorMessage() const noexcept;

public:
	UIException(UIErrorCode code);
	const char* what() const noexcept override;
	virtual ~UIException() noexcept = default;
	UIErrorCode getErrorCode() const noexcept;
};

#endif // USERINTERFACE_HPP