#include <iostream>
#include <sstream>
#include <algorithm>
#include <cctype>
#include "SeatMap.hpp"

// ==================== SeatMap Class ====================

// ==================== Constructors ====================

SeatMap::SeatMap(const string& seatLayout, int rows) : seatLayout(seatLayout), rows(rows)
{
	// Validate rows
	if (rows <= 0)
	{
		throw SeatMapException("Invalid number of rows. Must be greater than 0");
	}
	
	// Validate layout
	if (!validateSeatLayout(seatLayout))
	{
		throw SeatMapException("Invalid seat layout");
	}
	
	// Validate that layout produces at least one seat
	if (calculateSeatCount(seatLayout, rows) == 0)
	{
		throw SeatMapException("Invalid seat layout: Layout produces zero seats");
	}
}

SeatMap::SeatMap(const string& seatLayout, int rows, const vector<string>& reservedSeats)
	: seatLayout(seatLayout), rows(rows), reservedSeats(reservedSeats)
{
	// Validate rows
	if (rows <= 0)
	{
		throw SeatMapException("Invalid number of rows. Must be greater than 0");
	}
	
	// Validate layout
	if (!validateSeatLayout(seatLayout))
	{
		throw SeatMapException("Invalid seat layout");
	}
	
	// Validate that layout produces at least one seat
	if (calculateSeatCount(seatLayout, rows) == 0)
	{
		throw SeatMapException("Invalid seat layout: Layout produces zero seats");
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
		throw SeatMapException("Seat" + seatNumber + " is an invalid seat number");
	}
	
	// Check if seat is already reserved
	if (!isSeatAvailable(seatNumber))
	{
		throw SeatMapException("Seat" + seatNumber + " is already reserved");
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
	
	throw SeatMapException("Seat" + seatNumber + " not found in reserved list");
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

// ==================== Display Data Generation Methods ====================

void SeatMap::getSeatMapDisplayData(vector<string>& rowLabels, 
                                    vector<vector<string>>& gridData) const
{
	rowLabels.clear();
	gridData.clear();
	
	vector<string> allSeats = getAllSeats();
	vector<int> sections = parseSeatLayout(seatLayout);
	int seatsPerRow = getSeatsPerRow(seatLayout);
	
	// Build row by row
	for (int row = 1; row <= rows; ++row)
	{
		// Create row label
		string rowLabel = "Row ";
		if (row < 10) rowLabel += " ";
		rowLabel += std::to_string(row) + ":";
		rowLabels.push_back(rowLabel);
		
		// Create grid row with aisle spacing
		vector<string> gridRow;
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
						gridRow.push_back("[X]");
					}
					else
					{
						gridRow.push_back("[" + seatNum + "]");
					}
				}
				seatIndex++;
			}
			
			// Add aisle spacing (except after last section)
			if (sectionIdx < sections.size() - 1)
			{
				gridRow.push_back("  ");  // Aisle marker
			}
		}
		
		gridData.push_back(gridRow);
	}
}

vector<string> SeatMap::getSeatMapHeader(const string& flightNumber, const string& origin,
                                         const string& destination, const string& aircraftType) const
{
	vector<string> header;
	header.push_back("Aircraft Type: " + aircraftType);
	header.push_back("Route: " + origin + " to " + destination);
	return header;
}

vector<string> SeatMap::getSeatMapLegend() const
{
	vector<string> legend;
	legend.push_back("Legend: [Available] [X Reserved]");
	return legend;
}

vector<string> SeatMap::getSeatMapFooter() const
{
	vector<string> footer;
	footer.push_back("Total Seats: " + std::to_string(getTotalSeatsCount()));
	footer.push_back("Available: " + std::to_string(getAvailableSeatsCount()));
	footer.push_back("Reserved: " + std::to_string(reservedSeats.size()));
	return footer;
}

void SeatMap::getSampleSeatMapDisplayData(const string& seatLayout, int rows,
                                          vector<string>& rowLabels,
                                          vector<vector<string>>& gridData,
                                          int displayRows)
{
	rowLabels.clear();
	gridData.clear();
	
	vector<string> seatMap = generateSeatMap(seatLayout, rows);
	int seatsPerRow = getSeatsPerRow(seatLayout);
	
	// Display specified number of rows
	int rowsToDisplay = std::min(displayRows, rows);
	for (int row = 0; row < rowsToDisplay; ++row)
	{
		// Create row label
		rowLabels.push_back("Row " + std::to_string(row + 1) + ":");
		
		// Create grid row
		vector<string> gridRow;
		for (int seat = 0; seat < seatsPerRow; ++seat)
		{
			int index = row * seatsPerRow + seat;
			if (index < static_cast<int>(seatMap.size()))
			{
				gridRow.push_back(seatMap[index]);
			}
		}
		gridData.push_back(gridRow);
	}
}

vector<string> SeatMap::getSampleSeatMapFooter(const string& seatLayout, int rows)
{
	vector<string> footer;
	int totalSeats = calculateSeatCount(seatLayout, rows);
	footer.push_back("... (" + std::to_string(totalSeats) + " total seats)");
	return footer;
}

// ==================== SeatMapException Class ====================

SeatMapException::SeatMapException(const string& message) : message(message) {}

const char* SeatMapException::what() const noexcept
{
	return message.c_str();
}