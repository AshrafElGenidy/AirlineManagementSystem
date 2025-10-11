#include <iostream>
#include <sstream>
#include <algorithm>
#include <cctype>
#include "SeatMap.hpp"

// ==================== SeatMap Class ====================

// Static member initialization
UserInterface* SeatMap::ui = nullptr;

// ==================== Constructors ====================

SeatMap::SeatMap(const string& seatLayout, int rows)
	: seatLayout(seatLayout), rows(rows)
{
	// Validate rows
	if (rows <= 0)
	{
		throw SeatMapException(SeatMapErrorCode::INVALID_ROWS);
	}
	
	// Validate layout
	if (!validateSeatLayout(seatLayout))
	{
		throw SeatMapException(SeatMapErrorCode::INVALID_LAYOUT);
	}
	
	// Validate that layout produces at least one seat
	if (calculateSeatCount(seatLayout, rows) == 0)
	{
		throw SeatMapException(SeatMapErrorCode::INVALID_LAYOUT, "Layout produces zero seats");
	}
}

SeatMap::SeatMap(const string& seatLayout, int rows, const vector<string>& reservedSeats)
	: seatLayout(seatLayout), rows(rows), reservedSeats(reservedSeats)
{
	// Validate rows
	if (rows <= 0)
	{
		throw SeatMapException(SeatMapErrorCode::INVALID_ROWS);
	}
	
	// Validate layout
	if (!validateSeatLayout(seatLayout))
	{
		throw SeatMapException(SeatMapErrorCode::INVALID_LAYOUT);
	}
	
	// Validate that layout produces at least one seat
	if (calculateSeatCount(seatLayout, rows) == 0)
	{
		throw SeatMapException(SeatMapErrorCode::INVALID_LAYOUT, "Layout produces zero seats");
	}
}

// ==================== Static Validation Methods ====================

bool SeatMap::validateSeatLayout(const string& layout)
{
	// Expected format: "N-N" or "N-N-N" where N is a digit
	// Examples: "3-3", "2-4-2", "3-4-3"
	
	if (layout.empty())
	{
		return false;
	}
	
	// Check that layout contains only digits and hyphens
	for (char c : layout)
	{
		if (!std::isdigit(c) && c != '-')
		{
			return false;
		}
	}
	
	// Check that it starts and ends with a digit
	if (!std::isdigit(layout.front()) || !std::isdigit(layout.back()))
	{
		return false;
	}
	
	// Check for consecutive hyphens
	if (layout.find("--") != string::npos)
	{
		return false;
	}
	
	return true;
}

// ==================== Helper Methods ====================

vector<int> SeatMap::parseSeatLayout(const string& layout)
{
	vector<int> sections;
	std::stringstream ss(layout);
	string section;
	
	while (std::getline(ss, section, '-'))
	{
		sections.push_back(std::stoi(section));
	}
	
	return sections;
}

vector<char> SeatMap::generateSeatLetters(const string& layout)
{
	vector<int> sections = parseSeatLayout(layout);
	vector<char> seatLetters;
	char currentLetter = 'A';
	
	for (size_t i = 0; i < sections.size(); ++i)
	{
		for (int j = 0; j < sections[i]; ++j)
		{
			seatLetters.push_back(currentLetter++);
		}
		
		// Skip a letter for aisle (except after last section)
		if (i < sections.size() - 1)
		{
			currentLetter++;
		}
	}
	
	return seatLetters;
}

// ==================== Static Seat Generation ====================

vector<string> SeatMap::generateSeatMap(const string& seatLayout, int rows)
{
	vector<string> seatMap;
	vector<char> seatLetters = generateSeatLetters(seatLayout);
	
	// Generate all seat numbers (e.g., 1A, 1B, 1C, ..., 30F)
	for (int row = 1; row <= rows; ++row)
	{
		for (char letter : seatLetters)
		{
			seatMap.push_back(std::to_string(row) + letter);
		}
	}
	
	return seatMap;
}

int SeatMap::calculateSeatCount(const string& seatLayout, int rows)
{
	return generateSeatMap(seatLayout, rows).size();
}

int SeatMap::getSeatsPerRow(const string& seatLayout)
{
	vector<int> sections = parseSeatLayout(seatLayout);
	int seatsPerRow = 0;
	
	for (int section : sections)
	{
		seatsPerRow += section;
	}
	
	return seatsPerRow;
}

// ==================== Instance Methods ====================

vector<string> SeatMap::getAllSeats() const
{
	return generateSeatMap(seatLayout, rows);
}

bool SeatMap::isValidSeat(const string& seatNumber) const
{
	if (seatNumber.empty())
	{
		return false;
	}
	
	// Extract row number and seat letter
	size_t letterPos = 0;
	while (letterPos < seatNumber.length() && std::isdigit(seatNumber[letterPos]))
	{
		++letterPos;
	}
	
	if (letterPos == 0 || letterPos == seatNumber.length())
	{
		return false; // No row number or no seat letter
	}
	
	string rowStr = seatNumber.substr(0, letterPos);
	string seatLetter = seatNumber.substr(letterPos);
	
	// Validate row number
	int row = std::stoi(rowStr);
	if (row < 1 || row > rows)
	{
		return false;
	}
	
	// Validate seat letter exists in this layout
	vector<string> allSeats = getAllSeats();
	return std::find(allSeats.begin(), allSeats.end(), seatNumber) != allSeats.end();
}

// ==================== Reservation Management ====================

bool SeatMap::reserveSeat(const string& seatNumber)
{
	// Validate seat exists
	if (!isValidSeat(seatNumber))
	{
		throw SeatMapException(SeatMapErrorCode::INVALID_SEAT, seatNumber);
	}
	
	// Check if seat is already reserved
	if (!isSeatAvailable(seatNumber))
	{
		throw SeatMapException(SeatMapErrorCode::SEAT_ALREADY_RESERVED, seatNumber);
	}
	
	// Add seat to reserved list
	reservedSeats.push_back(seatNumber);
	return true;
}

bool SeatMap::releaseSeat(const string& seatNumber)
{
	// Find and remove the seat
	auto it = std::find(reservedSeats.begin(), reservedSeats.end(), seatNumber);
	
	if (it != reservedSeats.end())
	{
		reservedSeats.erase(it);
		return true;
	}
	
	throw SeatMapException(SeatMapErrorCode::SEAT_NOT_FOUND, seatNumber);
}

bool SeatMap::isSeatAvailable(const string& seatNumber) const
{
	// Check if seat is in reserved list
	return std::find(reservedSeats.begin(), reservedSeats.end(), seatNumber) == reservedSeats.end();
}

vector<string> SeatMap::getReservedSeats() const
{
	return reservedSeats;
}

int SeatMap::getAvailableSeatsCount() const
{
	return getTotalSeatsCount() - reservedSeats.size();
}

int SeatMap::getTotalSeatsCount() const
{
	return calculateSeatCount(seatLayout, rows);
}

// ==================== Display Methods ====================

void SeatMap::displaySeatMap(const string& flightNumber, const string& origin, 
							 const string& destination, const string& aircraftType) const
{
	ui->clearScreen();
	ui->printHeader("SEAT MAP - Flight " + flightNumber);
	
	ui->println("Aircraft Type: " + aircraftType);
	ui->println("Route: " + origin + " → " + destination);
	ui->println("");
	
	vector<string> allSeats = getAllSeats();
	vector<int> sections = parseSeatLayout(seatLayout);
	int seatsPerRow = getSeatsPerRow(seatLayout);
	
	ui->println("Legend: [Available] [X Reserved]");
	ui->printSeparator();
	
	// Display seats row by row
	for (int row = 1; row <= rows; ++row)
	{
		string rowDisplay = "Row ";
		if (row < 10) rowDisplay += " ";
		rowDisplay += std::to_string(row) + ": ";
		
		int seatIndex = 0;
		for (size_t sectionIdx = 0; sectionIdx < sections.size(); ++sectionIdx)
		{
			for (int seatInSection = 0; seatInSection < sections[sectionIdx]; ++seatInSection)
			{
				int globalIndex = (row - 1) * seatsPerRow + seatIndex;
				if (globalIndex < static_cast<int>(allSeats.size()))
				{
					string seatNum = allSeats[globalIndex];
					
					// Check if seat is reserved
					bool isReserved = std::find(reservedSeats.begin(), reservedSeats.end(), seatNum) 
									 != reservedSeats.end();
					
					if (isReserved)
					{
						rowDisplay += "[X]";
					}
					else
					{
						rowDisplay += "[" + seatNum + "]";
					}
					rowDisplay += " ";
				}
				seatIndex++;
			}
			
			// Add aisle spacing (except after last section)
			if (sectionIdx < sections.size() - 1)
			{
				rowDisplay += "  ";
			}
		}
		
		ui->println(rowDisplay);
	}
	
	ui->println("");
	ui->println("Total Seats: " + std::to_string(getTotalSeatsCount()));
	ui->println("Available: " + std::to_string(getAvailableSeatsCount()));
	ui->println("Reserved: " + std::to_string(reservedSeats.size()));
}

void SeatMap::displaySampleSeatMap(const string& seatLayout, int rows, int displayRows)
{
	ui->println("\nSample Seat Map (first " + std::to_string(displayRows) + " rows):");
	
	vector<string> seatMap = generateSeatMap(seatLayout, rows);
	int seatsPerRow = getSeatsPerRow(seatLayout);
	
	// Display specified number of rows
	int rowsToDisplay = std::min(displayRows, rows);
	for (int row = 0; row < rowsToDisplay; ++row)
	{
		string rowDisplay = "Row " + std::to_string(row + 1) + ": ";
		for (int seat = 0; seat < seatsPerRow; ++seat)
		{
			int index = row * seatsPerRow + seat;
			if (index < static_cast<int>(seatMap.size()))
			{
				rowDisplay += seatMap[index] + " ";
			}
		}
		ui->println(rowDisplay);
	}
	
	int totalSeats = calculateSeatCount(seatLayout, rows);
	ui->println("... (" + std::to_string(totalSeats) + " total seats)");
}

// ==================== Static Initialization ====================

void SeatMap::initializeSeatMapSystem()
{
	ui = UserInterface::getInstance();
}

// ==================== SeatMapException Class ====================

SeatMapException::SeatMapException(SeatMapErrorCode code)
	: errorCode(code), additionalInfo("")
{
}

SeatMapException::SeatMapException(SeatMapErrorCode code, const string& info)
	: errorCode(code), additionalInfo(info)
{
}

const char* SeatMapException::what() const noexcept
{
	return getErrorMessage().c_str();
}

SeatMapErrorCode SeatMapException::getErrorCode() const noexcept
{
	return errorCode;
}

string SeatMapException::getErrorMessage() const noexcept
{
	string message;
	
	switch (errorCode)
	{
		case SeatMapErrorCode::INVALID_SEAT:
			message = "Invalid seat number";
			break;
		case SeatMapErrorCode::SEAT_ALREADY_RESERVED:
			message = "Seat is already reserved";
			break;
		case SeatMapErrorCode::SEAT_NOT_FOUND:
			message = "Seat not found in reserved list";
			break;
		case SeatMapErrorCode::INVALID_LAYOUT:
			message = "Invalid seat layout";
			break;
		case SeatMapErrorCode::INVALID_ROWS:
			message = "Invalid number of rows. Must be greater than 0";
			break;
		default:
			message = "An unknown seat map error occurred";
			break;
	}
	
	if (!additionalInfo.empty())
	{
		message += ": " + additionalInfo;
	}
	
	return message;
}