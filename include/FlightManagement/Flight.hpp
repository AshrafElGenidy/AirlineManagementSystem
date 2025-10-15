#ifndef FLIGHT_HPP
#define FLIGHT_HPP

#include <string>
#include <vector>
#include <exception>
#include <memory>
#include "json.hpp"

using nlohmann::json;
using std::string;
using std::vector;

// ==================== Flight Class (Pure Data) ====================

class Flight
{
private:
	string flightNumber;
	string origin;
	string destination;
	string departureDateTime;
	string arrivalDateTime;
	string aircraftType;
	string status;
	double price;
	string gate;
	string boardingTime;
	vector<string> reservedSeats;
	vector<string> assignedCrewIds;
	
	// Private constructors - only FlightManager and FlightCreator can create
	Flight(const string& flightNumber, const string& origin, const string& destination,
	       const string& departureDateTime, const string& arrivalDateTime,
	       const string& aircraftType, const string& status, double price,
	       const string& gate, const string& boardingTime,
	       const vector<string>& reservedSeats = {},
	       const vector<string>& assignedCrewIds = {});
	
	friend class FlightManager;
	friend class FlightCreator;
	
public:
	// Getters
	string getFlightNumber() const noexcept;
	string getOrigin() const noexcept;
	string getDestination() const noexcept;
	string getDepartureDateTime() const noexcept;
	string getArrivalDateTime() const noexcept;
	string getAircraftType() const noexcept;
	string getStatus() const noexcept;
	double getPrice() const noexcept;
	int getTotalSeats() const;
	int getAvailableSeats() const;
	string getGate() const noexcept;
	string getBoardingTime() const noexcept;
	vector<string> getReservedSeats() const noexcept;
	vector<string> getAssignedCrew() const noexcept;
	
	// Setters
	void setOrigin(const string& origin) noexcept;
	void setDestination(const string& destination) noexcept;
	void setDepartureDateTime(const string& departureDateTime) noexcept;
	void setArrivalDateTime(const string& arrivalDateTime) noexcept;
	void setAircraftType(const string& aircraftType) noexcept;
	void setStatus(const string& status) noexcept;
	void setPrice(double price) noexcept;
	void setGate(const string& gate) noexcept;
	void setBoardingTime(const string& boardingTime) noexcept;
	
	// Seat management
	bool reserveSeat(const string& seatNumber);
	bool releaseSeat(const string& seatNumber);
	bool isSeatAvailable(const string& seatNumber) const noexcept;

	// Crew management
	void addCrewMember(const string& crewId);
	void removeCrewMember(const string& crewId);
	bool hasCrewMember(const string& crewId) const noexcept;
	double getFlightDuration() const;
	
	// Destructor
	virtual ~Flight() noexcept = default;
};

// ==================== Flight Exception Class ====================

class FlightException : public std::exception
{
private:
	string message;
public:
	FlightException(const string& message);
	const char* what() const noexcept override;
	virtual ~FlightException() noexcept = default;
};

#endif // FLIGHT_HPP