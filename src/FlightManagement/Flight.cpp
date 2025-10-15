#include <algorithm>
#include "Flight.hpp"
#include "Aircraft.hpp"
#include "AircraftManager.hpp"
#include "SeatMap.hpp"

// ==================== Flight Constructor ====================

Flight::Flight(const string& flightNumber, const string& origin, const string& destination,
              const string& departureDateTime, const string& arrivalDateTime,
              const string& aircraftType, const string& status, double price,
              const string& gate, const string& boardingTime,
              const vector<string>& reservedSeats,
			const vector<string>& assignedCrewIds)
	: flightNumber(flightNumber), origin(origin), destination(destination),
	  departureDateTime(departureDateTime), arrivalDateTime(arrivalDateTime),
	  aircraftType(aircraftType), status(status), price(price),
	  gate(gate), boardingTime(boardingTime), reservedSeats(reservedSeats), assignedCrewIds(assignedCrewIds)
{
}

// ==================== Getters ====================

string Flight::getFlightNumber() const noexcept
{
	return flightNumber;
}

string Flight::getOrigin() const noexcept
{
	return origin;
}

string Flight::getDestination() const noexcept
{
	return destination;
}

string Flight::getDepartureDateTime() const noexcept
{
	return departureDateTime;
}

string Flight::getArrivalDateTime() const noexcept
{
	return arrivalDateTime;
}

string Flight::getAircraftType() const noexcept
{
	return aircraftType;
}

string Flight::getStatus() const noexcept
{
	return status;
}

double Flight::getPrice() const noexcept
{
	return price;
}

int Flight::getTotalSeats() const
{
	// Query AircraftManager for seat count based on aircraft type
	try
	{
		shared_ptr<Aircraft> aircraft = AircraftManager::getInstance()->getAircraft(aircraftType);
		if (!aircraft)
		{
			throw FlightException("Invalid or missing aircraft type.");
		}
		
		string seatLayout = aircraft->getSeatLayout();
		int rows = aircraft->getRows();
		return SeatMap::calculateSeatCount(seatLayout, rows);
	}
	catch (const SeatMapException& e)
	{
		throw FlightException("An error occurred while accessing the database.");
	}
}

int Flight::getAvailableSeats() const
{
	int totalSeats = getTotalSeats();
	return totalSeats - static_cast<int>(reservedSeats.size());
}

string Flight::getGate() const noexcept
{
	return gate;
}

string Flight::getBoardingTime() const noexcept
{
	return boardingTime;
}

vector<string> Flight::getReservedSeats() const noexcept
{
	return reservedSeats;
}
vector<string> Flight::getAssignedCrew() const noexcept
{
	return assignedCrewIds;
}

// ==================== Setters ====================

void Flight::setStatus(const string& status) noexcept
{
	this->status = status;
}

void Flight::setPrice(double price) noexcept
{
	this->price = price;
}

void Flight::setGate(const string& gate) noexcept
{
	this->gate = gate;
}

void Flight::setBoardingTime(const string& boardingTime) noexcept
{
	this->boardingTime = boardingTime;
}

void Flight::setOrigin(const string& origin) noexcept
{
	this->origin = origin;
}

void Flight::setDestination(const string& destination) noexcept
{
	this->destination = destination;
}

void Flight::setDepartureDateTime(const string& departureDateTime) noexcept
{
	this->departureDateTime = departureDateTime;
}

void Flight::setArrivalDateTime(const string& arrivalDateTime) noexcept
{
	this->arrivalDateTime = arrivalDateTime;
}

void Flight::setAircraftType(const string& aircraftType) noexcept
{
	this->aircraftType = aircraftType;
}

// ==================== Seat Management ====================

bool Flight::reserveSeat(const string& seatNumber)
{
	// Check if seat already reserved
	if (!isSeatAvailable(seatNumber))
	{
		throw FlightException("Seat " + seatNumber + " is already reserved.");
	}
	
	// Check if seat is valid for this aircraft
	try
	{
		shared_ptr<Aircraft> aircraft = AircraftManager::getInstance()->getAircraft(aircraftType);
		if (!aircraft)
		{
			throw FlightException("Invalid or missing aircraft type.");
		}
		
		string seatLayout = aircraft->getSeatLayout();
		int rows = aircraft->getRows();
		SeatMap seatMap(seatLayout, rows);
		
		if (!seatMap.isValidSeat(seatNumber))
		{
			throw FlightException("Seat " + seatNumber + " is invalid for this aircraft.");
		}
	}
	catch (const SeatMapException& e)
	{
		throw FlightException("Seat operation failed." + string(e.what()));
	}
	
	reservedSeats.push_back(seatNumber);
	return true;
}

bool Flight::releaseSeat(const string& seatNumber)
{
	auto it = std::find(reservedSeats.begin(), reservedSeats.end(), seatNumber);
	
	if (it != reservedSeats.end())
	{
		reservedSeats.erase(it);
		return true;
	}
	
	return false;
}

bool Flight::isSeatAvailable(const string& seatNumber) const noexcept
{
	return std::find(reservedSeats.begin(), reservedSeats.end(), seatNumber) == reservedSeats.end();
}

// ==================== Crew Management Methods ====================

void Flight::addCrewMember(const string& crewId)
{
	if (!hasCrewMember(crewId))
	{
		assignedCrewIds.push_back(crewId);
	}
}

void Flight::removeCrewMember(const string& crewId)
{
	auto it = std::find(assignedCrewIds.begin(), assignedCrewIds.end(), crewId);
	if (it != assignedCrewIds.end())
	{
		assignedCrewIds.erase(it);
	}
}

bool Flight::hasCrewMember(const string& crewId) const noexcept
{
	return std::find(assignedCrewIds.begin(), assignedCrewIds.end(), crewId) != assignedCrewIds.end();
}

double Flight::getFlightDuration() const
{
	// Parse departure and arrival times to calculate duration in hours
	// Format: "YYYY-MM-DD HH:MM"
	
	if (departureDateTime.length() < 16 || arrivalDateTime.length() < 16)
	{
		return 0.0;
	}
	
	try
	{
		// Extract hours and minutes from both times
		int depHour = std::stoi(departureDateTime.substr(11, 2));
		int depMin = std::stoi(departureDateTime.substr(14, 2));
		int arrHour = std::stoi(arrivalDateTime.substr(11, 2));
		int arrMin = std::stoi(arrivalDateTime.substr(14, 2));
		
		// Convert to total minutes
		int depTotalMin = depHour * 60 + depMin;
		int arrTotalMin = arrHour * 60 + arrMin;
		
		// Calculate duration in minutes (handle day wrap-around)
		int durationMin = arrTotalMin - depTotalMin;
		if (durationMin < 0)
		{
			durationMin += 24 * 60;  // Add 24 hours if arrival is next day
		}
		
		// Convert to hours
		return durationMin / 60.0;
	}
	catch (const std::exception&)
	{
		return 0.0;
	}
}

// ==================== FlightException Class ====================

FlightException::FlightException(const string& message) : message(message) {}

const char* FlightException::what() const noexcept
{
	return message.c_str();
}