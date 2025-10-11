#ifndef AIRCRAFT_HPP
#define AIRCRAFT_HPP

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

static constexpr int MIN_AIRCRAFT_TYPE_LENGTH = 2;
static constexpr int MAX_AIRCRAFT_TYPE_LENGTH = 30;
static constexpr int MIN_SEATS = 50;
static constexpr int MAX_SEATS = 500;

// ==================== Aircraft Class ====================

class Aircraft
{
private:
	string aircraftType;
	
	// Static data members
	static UserInterface* ui;
	static std::unique_ptr<Database> db;
	
	// Helpers
	static bool validateAircraftType(const string& aircraftType);
	static string selectAircraftStatus();
	
	// Aircraft Management
	static void addAircraft();
	static void viewAllAircraft();
	static void updateAircraft();
	static void removeAircraft();
	void updateAircraftDetails();
	
	// Constructors
	Aircraft();										// For new aircraft types
	
public:
	// Constructors
	explicit Aircraft(const string& aircraftType);	// For existing aircraft types
	
	// Static system initialization
	static void initializeAircraftSystem();
	
	// Aircraft Management
	static void manageAircraft();
	
	// Operational methods (for use by other classes)
	static vector<string> getAllAircraftTypes();
	static bool aircraftTypeExists(const string& aircraftType);
	
	// Getters
	string getAircraftType() const noexcept;
	string getManufacturer() const;
	string getModel() const;
	int getTotalSeats() const;
	string getSeatLayout() const;
	int getRows() const;
	int getFleetCount() const;
	string getStatus() const;
	
	// Setters
	void setManufacturer(const string& manufacturer);
	void setModel(const string& model);
	void setTotalSeats(int seats);
	void setSeatLayout(const string& layout);
	void setRows(int rows);
	void setFleetCount(int count);
	void setStatus(const string& status);
	
	// Utility
	void displayAircraftInfo() const;
	
	virtual ~Aircraft() noexcept = default;
};

// ==================== Aircraft Exception Class ====================

enum class AircraftErrorCode
{
	AIRCRAFT_NOT_FOUND,
	AIRCRAFT_EXISTS,
	INVALID_AIRCRAFT_TYPE,
	INVALID_SEAT_LAYOUT,
	INVALID_SEAT_NUMBER,
	DATABASE_ERROR
};

class AircraftException : public std::exception
{
private:
	AircraftErrorCode errorCode;
	string getErrorMessage() const noexcept;

public:
	AircraftException(AircraftErrorCode code);
	const char* what() const noexcept override;
	virtual ~AircraftException() noexcept = default;
	AircraftErrorCode getErrorCode() const noexcept;
};

#endif // AIRCRAFT_HPP