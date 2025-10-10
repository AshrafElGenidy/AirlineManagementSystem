#ifndef FLIGHT_HPP
#define FLIGHT_HPP

#include <string>
#include <vector>
#include <exception>
#include <memory>
#include "json.hpp"
#include "UserInterface.hpp"
#include "Database.hpp"

using nlohmann::json;
using std::string;
using std::vector;
using std::unique_ptr;

static constexpr int MIN_FLIGHT_NUMBER_LENGTH = 3;
static constexpr int MAX_FLIGHT_NUMBER_LENGTH = 10;

// ==================== Flight Class ====================

class Flight
{
private:
	string flightNumber;
	
	// Static data members
	static UserInterface* ui;
	static std::unique_ptr<Database> db;
	
	// Helpers
	string selectFlightStatus();
	static bool validateFlightNumber(const string& flightNumber);

	// Flight Management
	static void addFlight();
	static void viewAllFlights();
	static void updateFlight();
	static void removeFlight();
	void updateFlightDetails();
	
	// Constructors
	Flight();										// For new flights
	explicit Flight(const string& flightNumber);	// For existing flights
	
public:
	// Static system initialization
	static void initializeFlightSystem();
	
	// Flight Management
	static void manageFlights();
	
	// Operational methods
	static vector<unique_ptr<Flight>> searchFlights(const string& origin, const string& destination, const string& departureDate);
	
	// Getters
	string getFlightNumber() const noexcept;
	string getOrigin() const;
	string getDestination() const;
	string getDepartureDateTime() const;
	string getArrivalDateTime() const;
	string getAircraftType() const;
	string getStatus() const;
	double getPrice() const;
	int getTotalSeats() const;
	int getAvailableSeats() const;
	string getGate() const;
	string getBoardingTime() const;
	
	// Setters
	void setStatus(const string& status);
	void setPrice(double price);
	void setGate(const string& gate);
	void setBoardingTime(const string& boardingTime);
	
	// Seat Management
	vector<string> getReservedSeats() const;
	bool reserveSeat(const string& seatNumber);
	bool releaseSeat(const string& seatNumber);
	bool isSeatAvailable(const string& seatNumber) const;
	void displaySeatMap() const;
	
	// Utility
	void displayFlightInfo() const;
	
	virtual ~Flight() noexcept = default;
};

// ==================== Flight Exception Class ====================

enum class FlightErrorCode
{
	FLIGHT_NOT_FOUND,
	FLIGHT_EXISTS,
	INVALID_FLIGHT_NUMBER,
	INVALID_AIRCRAFT_TYPE,
	DATABASE_ERROR
};

class FlightException : public std::exception
{
private:
	FlightErrorCode errorCode;
	string getErrorMessage() const noexcept;

public:
	FlightException(FlightErrorCode code);
	const char* what() const noexcept override;
	virtual ~FlightException() noexcept = default;
	FlightErrorCode getErrorCode() const noexcept;
};

#endif // FLIGHT_HPP