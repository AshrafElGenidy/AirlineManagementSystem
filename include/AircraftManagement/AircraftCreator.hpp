#ifndef AIRCRAFTCREATOR_HPP
#define AIRCRAFTCREATOR_HPP

#include <string>
#include <memory>
#include "Aircraft.hpp"
#include "UserInterface.hpp"

using std::string;
using std::unique_ptr;
using std::shared_ptr;

// ==================== Aircraft Validator ====================

class AircraftValidator
{
public:
	// Validation methods
	static bool isValidAircraftType(const string& aircraftType);
	static bool isValidManufacturer(const string& manufacturer);
	static bool isValidModel(const string& model);
	static bool isValidSeatLayout(const string& seatLayout);
	static bool isValidSeatCount(int totalSeats);
	static bool isValidRows(int rows);
	static bool isValidFleetCount(int fleetCount);
};

// ==================== AircraftCreator ====================

class AircraftCreator
{
private:
	UserInterface* ui;
	
	// Prompts user for each field with validation
	string getValidAircraftType();
	string getValidManufacturer();
	string getValidModel();
	string getValidSeatLayout();
	int getValidSeatCount();
	int getValidRows();
	int getValidFleetCount();
	
public:
	AircraftCreator();
	
	// Create new aircraft by prompting user for all details
	shared_ptr<Aircraft> createNewAircraft();
	
	// Create aircraft from JSON data (deserialization)
	shared_ptr<Aircraft> createFromJson(const string& aircraftType, const json& data);
	
	// Serialize aircraft to JSON
	json toJson(const shared_ptr<Aircraft>& aircraft);
	
	// Destructor
	~AircraftCreator() noexcept = default;
	
	friend class AircraftManager; // For AircraftManager to use getValid methods
};

#endif // AIRCRAFTCREATOR_HPP