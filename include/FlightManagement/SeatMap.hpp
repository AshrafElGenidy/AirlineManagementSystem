#ifndef SEATMAP_HPP
#define SEATMAP_HPP

#include <string>
#include <vector>
#include <exception>
#include "json.hpp"
#include "UserInterface.hpp"

using nlohmann::json;
using std::string;
using std::vector;

static constexpr int DEFAULT_DISPLAY_ROWS = 3;

// ==================== SeatMap Class ====================

class SeatMap
{
private:
	string seatLayout;
	int rows;
	vector<string> reservedSeats;
	
	static UserInterface* ui;
	
	// Helper methods
	static vector<int> parseSeatLayout(const string& layout);
	static vector<char> generateSeatLetters(const string& layout);

public:
	// Constructors
	SeatMap(const string& seatLayout, int rows);
	SeatMap(const string& seatLayout, int rows, const vector<string>& reservedSeats);
	
	// Static system initialization
	static void initializeSeatMapSystem();
	
	// Static validation methods (used by Aircraft and Flight)
	static bool validateSeatLayout(const string& layout);
	
	// Static seat generation (used by Aircraft for display purposes)
	static vector<string> generateSeatMap(const string& seatLayout, int rows);
	static int calculateSeatCount(const string& seatLayout, int rows);
	static int getSeatsPerRow(const string& seatLayout);
	
	// Instance methods for seat validation
	bool isValidSeat(const string& seatNumber) const;
	vector<string> getAllSeats() const;
	
	// Reservation management
	bool reserveSeat(const string& seatNumber);
	bool releaseSeat(const string& seatNumber);
	bool isSeatAvailable(const string& seatNumber) const;
	vector<string> getReservedSeats() const;
	int getAvailableSeatsCount() const;
	int getTotalSeatsCount() const;
	
	// Display methods
	void displaySeatMap(const string& flightNumber, const string& origin, 
						const string& destination, const string& aircraftType) const;
	static void displaySampleSeatMap(const string& seatLayout, int rows, int displayRows = DEFAULT_DISPLAY_ROWS);
	
	virtual ~SeatMap() noexcept = default;
};

// ==================== SeatMap Exception Class ====================

enum class SeatMapErrorCode
{
	INVALID_SEAT,
	SEAT_ALREADY_RESERVED,
	SEAT_NOT_FOUND,
	INVALID_LAYOUT,
	INVALID_ROWS
};

class SeatMapException : public std::exception
{
private:
	SeatMapErrorCode errorCode;
	string additionalInfo;
	string getErrorMessage() const noexcept;

public:
	SeatMapException(SeatMapErrorCode code);
	SeatMapException(SeatMapErrorCode code, const string& info);
	const char* what() const noexcept override;
	virtual ~SeatMapException() noexcept = default;
	SeatMapErrorCode getErrorCode() const noexcept;
};

#endif // SEATMAP_HPP