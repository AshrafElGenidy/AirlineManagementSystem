#include <iostream>
#include <fstream>
#include <sstream>
#include <algorithm>
#include <cctype>
#include <iomanip>
#include "Aircraft.hpp"

// ==================== Aircraft Class ====================

// Static member initialization
UserInterface* Aircraft::ui = nullptr;
std::unique_ptr<Database> Aircraft::db = nullptr;

// ==================== Constructors ====================

Aircraft::Aircraft()
{
	string aircraftType = ui->getString("Enter Aircraft Type (e.g., Boeing737, AirbusA320): ");

	// Validate aircraft type
	if (!validateAircraftType(aircraftType))
	{
		throw AircraftException(AircraftErrorCode::INVALID_AIRCRAFT_TYPE);
	}

	this->aircraftType = aircraftType;

	ui->println("\n--- Add New Aircraft Type ---");
	string manufacturer = ui->getString("Enter Manufacturer: ");
	string model = ui->getString("Enter Model: ");
	int totalSeats = ui->getInt("Enter Total Seats (" + std::to_string(MIN_SEATS) + "-" + std::to_string(MAX_SEATS) + "): ");
	
	// Validate seat count
	if (totalSeats < MIN_SEATS || totalSeats > MAX_SEATS)
	{
		throw AircraftException(AircraftErrorCode::INVALID_SEAT_NUMBER);
	}
	
	string seatLayout = ui->getString("Enter Seat Layout (e.g., 3-3 for 3 seats-aisle-3 seats): ");
	
	// Validate seat layout
	if (!validateSeatLayout(seatLayout))
	{
		throw AircraftException(AircraftErrorCode::INVALID_SEAT_LAYOUT);
	}
	
	int rows = ui->getInt("Enter Number of Rows: ");
	int fleetCount = ui->getInt("Enter Fleet Count (number of aircraft owned): ");
	string status = selectAircraftStatus();

	if (db->entryExists(aircraftType))
	{
		throw AircraftException(AircraftErrorCode::AIRCRAFT_EXISTS);
	}

	json aircraftData;
	aircraftData["manufacturer"] = manufacturer;
	aircraftData["model"] = model;
	aircraftData["totalSeats"] = totalSeats;
	aircraftData["seatLayout"] = seatLayout;
	aircraftData["rows"] = rows;
	aircraftData["fleetCount"] = fleetCount;
	aircraftData["status"] = status;

	db->addEntry(this->aircraftType, aircraftData);

	ui->println("");
	ui->printSuccess("Aircraft type '" + aircraftType + "' has been successfully added to the fleet.");
}

Aircraft::Aircraft(const string& aircraftType) : aircraftType(aircraftType)
{
	// Load aircraft data to verify aircraft exists
	if (!db->entryExists(aircraftType))
	{
		throw AircraftException(AircraftErrorCode::AIRCRAFT_NOT_FOUND);
	}
}

// ==================== Validation Methods ====================

bool Aircraft::validateAircraftType(const string& aircraftType)
{
	// Check length
	if (aircraftType.length() < MIN_AIRCRAFT_TYPE_LENGTH || 
		aircraftType.length() > MAX_AIRCRAFT_TYPE_LENGTH)
	{
		return false;
	}
	
	// Check characters (alphanumeric only, no spaces)
	return std::all_of(aircraftType.begin(), aircraftType.end(), 
		[](unsigned char c) { return std::isalnum(c); });
}

bool Aircraft::validateSeatLayout(const string& layout)
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

// ==================== Aircraft Management Menu ====================

void Aircraft::manageAircraft()
{
	while (true)
	{
		ui->clearScreen();
		
		vector<string> options = {
			"Add New Aircraft Type",
			"Update Existing Aircraft Type",
			"Remove Aircraft Type",
			"View All Aircraft Types",
			"Back to Main Menu"
		};
		
		ui->displayMenu("Manage Aircraft", options);
		int choice = ui->getChoice("Enter choice: ", 1, 5);
		
		switch (choice)
		{
			case 1:
				addAircraft();
				break;
			case 2:
				updateAircraft();
				break;
			case 3:
				removeAircraft();
				break;
			case 4:
				viewAllAircraft();
				break;
			case 5:
				return; // Back to main menu
			default:
				ui->printError("Invalid choice.");
				ui->pauseScreen();
				break;
		}
	}
}

void Aircraft::addAircraft()
{
	ui->clearScreen();
	ui->printHeader("Add New Aircraft Type");
	
	try
	{
		Aircraft newAircraft;
	}
	catch (const std::exception& e)
	{
		ui->printError(string(e.what()));
	}
	
	ui->pauseScreen();
}

void Aircraft::viewAllAircraft()
{
	ui->clearScreen();
	ui->printHeader("View All Aircraft Types");
	
	json allAircraftData = db->loadAll();
	
	if (allAircraftData.empty())
	{
		ui->printWarning("No aircraft types found in the system.");
	}
	else
	{
		vector<string> headers = {
			"Aircraft Type", "Manufacturer", "Model", "Total Seats", "Layout", "Rows", "Fleet Count", "Status"
		};
		
		vector<vector<string>> rows;
		
		for (const auto& [type, aircraftData] : allAircraftData.items())
		{
			string manufacturer = aircraftData.value("manufacturer", "N/A");
			string model = aircraftData.value("model", "N/A");
			int totalSeats = aircraftData.value("totalSeats", 0);
			string layout = aircraftData.value("seatLayout", "N/A");
			int rowCount = aircraftData.value("rows", 0);
			int fleetCount = aircraftData.value("fleetCount", 0);
			string status = aircraftData.value("status", "N/A");
			
			rows.push_back({
				type,
				manufacturer,
				model,
				std::to_string(totalSeats),
				layout,
				std::to_string(rowCount),
				std::to_string(fleetCount),
				status
			});
		}
		
		ui->displayTable(headers, rows);
		ui->println("\nTotal Aircraft Types: " + std::to_string(allAircraftData.size()));
	}
	
	ui->pauseScreen();
}

void Aircraft::updateAircraft()
{
	ui->clearScreen();

	try
	{
		string aircraftType = ui->getString("Enter Aircraft Type to Update: ");
		std::unique_ptr<Aircraft> aircraft = std::make_unique<Aircraft>(aircraftType);

		ui->println("\nSelect information to update:");
		vector<string> options = {
			"Aircraft Details",
			"Fleet Count",
			"Status",
			"Back to Manage Aircraft"
		};

		ui->displayMenu("Update Existing Aircraft Type", options);
		int choice = ui->getChoice("Enter choice: ", 1, 4);

		switch (choice)
		{
			case 1:
				aircraft->updateAircraftDetails();
				break;

			case 2:
			{
				int newFleetCount = ui->getInt("Enter new Fleet Count: ");
				aircraft->setFleetCount(newFleetCount);
				ui->printSuccess("Fleet count updated successfully.");
				break;
			}

			case 3:
			{
				string newStatus = aircraft->selectAircraftStatus();
				aircraft->setStatus(newStatus);
				ui->printSuccess("Aircraft status updated successfully.");
				break;
			}

			case 4:
				ui->println("Returning to Manage Aircraft menu.");
				break;

			default:
				ui->printError("Invalid choice.");
				break;
		}
	}
	catch (const std::exception& e)
	{
		ui->printError(string(e.what()));
	}

	ui->pauseScreen();
}

// ==================== Update Aircraft Details ====================

void Aircraft::updateAircraftDetails()
{
	ui->clearScreen();
	ui->printHeader("--- Update Aircraft Details ---");

	if (!db->entryExists(aircraftType))
	{
		throw AircraftException(AircraftErrorCode::AIRCRAFT_NOT_FOUND);
	}

	json aircraftData = db->getEntry(aircraftType);

	ui->println("Current Aircraft Information:");
	ui->println("1. Manufacturer: " + aircraftData.value("manufacturer", "N/A"));
	ui->println("2. Model: " + aircraftData.value("model", "N/A"));
	ui->println("3. Total Seats: " + std::to_string(aircraftData.value("totalSeats", 0)));
	ui->println("4. Seat Layout: " + aircraftData.value("seatLayout", "N/A"));
	ui->println("5. Rows: " + std::to_string(aircraftData.value("rows", 0)));
	ui->println("6. Back to Previous Menu\n");

	int choice = ui->getChoice("Select field to update (1-6): ", 1, 6);

	if (choice == 6)
	{
		ui->printWarning("Returning to previous menu.");
		return;
	}

	try
	{
		switch (choice)
		{
			case 1:
			{
				string newManufacturer = ui->getString("Enter new Manufacturer: ");
				db->setAttribute(aircraftType, "manufacturer", newManufacturer);
				break;
			}
			case 2:
			{
				string newModel = ui->getString("Enter new Model: ");
				db->setAttribute(aircraftType, "model", newModel);
				break;
			}
			case 3:
			{
				int newTotalSeats = ui->getInt("Enter new Total Seats: ");
				if (newTotalSeats < MIN_SEATS || newTotalSeats > MAX_SEATS)
				{
					throw AircraftException(AircraftErrorCode::INVALID_SEAT_NUMBER);
				}
				db->setAttribute(aircraftType, "totalSeats", newTotalSeats);
				break;
			}
			case 4:
			{
				string newLayout = ui->getString("Enter new Seat Layout (e.g., 3-3): ");
				if (!validateSeatLayout(newLayout))
				{
					throw AircraftException(AircraftErrorCode::INVALID_SEAT_LAYOUT);
				}
				db->setAttribute(aircraftType, "seatLayout", newLayout);
				break;
			}
			case 5:
			{
				int newRows = ui->getInt("Enter new Number of Rows: ");
				db->setAttribute(aircraftType, "rows", newRows);
				break;
			}
			default:
				ui->printError("Invalid choice.");
				return;
		}

		ui->printSuccess("Aircraft details updated successfully.");
	}
	catch (const std::exception& e)
	{
		ui->printError("Error updating aircraft details: " + string(e.what()));
	}

	ui->pauseScreen();
}

void Aircraft::removeAircraft()
{
	ui->clearScreen();
	ui->printHeader("Remove Aircraft Type");
	
	try
	{
		string aircraftType = ui->getString("Enter Aircraft Type to Remove: ");
		
		// Check if aircraft exists
		if (!db->entryExists(aircraftType))
		{
			throw AircraftException(AircraftErrorCode::AIRCRAFT_NOT_FOUND);
		}
		
		// TODO: Check if aircraft type is being used by any flights
		// Should query Flight database to ensure no active flights use this type
		
		bool confirm = ui->getYesNo("Are you sure you want to remove aircraft type '" + aircraftType + "'?");
		if (confirm)
		{
			db->deleteEntry(aircraftType);
			
			ui->printSuccess("Aircraft type '" + aircraftType + "' has been removed successfully.");
		}
		else
		{
			ui->printWarning("Aircraft removal canceled.");
		}
	}
	catch (const std::exception& e)
	{
		ui->printError(string(e.what()));
	}
	
	ui->pauseScreen();
}

// ==================== Getters ====================

string Aircraft::getAircraftType() const noexcept
{
	return aircraftType;
}

string Aircraft::getManufacturer() const
{
	return db->getAttribute(aircraftType, "manufacturer");
}

string Aircraft::getModel() const
{
	return db->getAttribute(aircraftType, "model");
}

int Aircraft::getTotalSeats() const
{
	return db->getAttribute(aircraftType, "totalSeats");
}

string Aircraft::getSeatLayout() const
{
	return db->getAttribute(aircraftType, "seatLayout");
}

int Aircraft::getRows() const
{
	return db->getAttribute(aircraftType, "rows");
}

int Aircraft::getFleetCount() const
{
	return db->getAttribute(aircraftType, "fleetCount");
}

string Aircraft::getStatus() const
{
	return db->getAttribute(aircraftType, "status");
}

// ==================== Setters ====================

void Aircraft::setManufacturer(const string& manufacturer)
{
	db->setAttribute(aircraftType, "manufacturer", manufacturer);
}

void Aircraft::setModel(const string& model)
{
	db->setAttribute(aircraftType, "model", model);
}

void Aircraft::setTotalSeats(int seats)
{
	if (seats < MIN_SEATS || seats > MAX_SEATS)
	{
		throw AircraftException(AircraftErrorCode::INVALID_SEAT_NUMBER);
	}
	
	db->setAttribute(aircraftType, "totalSeats", seats);
}

void Aircraft::setSeatLayout(const string& layout)
{
	if (!validateSeatLayout(layout))
	{
		throw AircraftException(AircraftErrorCode::INVALID_SEAT_LAYOUT);
	}
	
	db->setAttribute(aircraftType, "seatLayout", layout);
}

void Aircraft::setRows(int rows)
{
	db->setAttribute(aircraftType, "rows", rows);
}

void Aircraft::setFleetCount(int count)
{
	db->setAttribute(aircraftType, "fleetCount", count);
}

void Aircraft::setStatus(const string& status)
{
	db->setAttribute(aircraftType, "status", status);
}

// ==================== Seat Management ====================

vector<string> Aircraft::generateSeatMap() const
{
	vector<string> seatMap;
	
	string layout = db->getAttribute(aircraftType, "seatLayout");
	int rows = db->getAttribute(aircraftType, "rows");
	
	// Parse seat layout (e.g., "3-3" means 3 seats, aisle, 3 seats)
	vector<int> sections;
	std::stringstream ss(layout);
	string section;
	
	while (std::getline(ss, section, '-'))
	{
		sections.push_back(std::stoi(section));
	}
	
	// Generate seat letters based on layout
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

bool Aircraft::isValidSeat(const string& seatNumber) const
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
	int rows = db->getAttribute(aircraftType, "rows");
	if (row < 1 || row > rows)
	{
		return false;
	}
	
	// Validate seat letter exists in this aircraft's layout
	vector<string> allSeats = generateSeatMap();
	return std::find(allSeats.begin(), allSeats.end(), seatNumber) != allSeats.end();
}

int Aircraft::getSeatCount() const
{
	return generateSeatMap().size();
}

// ==================== Utility ====================

void Aircraft::displayAircraftInfo() const
{
	ui->clearScreen();
	ui->printHeader("AIRCRAFT INFORMATION");
	ui->println("Aircraft Type: " + getAircraftType());
	ui->println("Manufacturer: " + getManufacturer());
	ui->println("Model: " + getModel());
	ui->println("Total Seats: " + std::to_string(getTotalSeats()));
	ui->println("Seat Layout: " + getSeatLayout());
	ui->println("Rows: " + std::to_string(getRows()));
	ui->println("Fleet Count: " + std::to_string(getFleetCount()));
	ui->println("Status: " + getStatus());
	
	// Display sample seat map
	ui->println("\nSample Seat Map (first 3 rows):");
	vector<string> seatMap = generateSeatMap();
	string layout = getSeatLayout();
	
	// Parse layout to determine seats per row
	int seatsPerRow = 0;
	std::stringstream ss(layout);
	string section;
	while (std::getline(ss, section, '-'))
	{
		seatsPerRow += std::stoi(section);
	}
	
	// Display first 3 rows
	int displayRows = std::min(3, getRows());
	for (int row = 0; row < displayRows; ++row)
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
	ui->println("... (" + std::to_string(getSeatCount()) + " total seats)");
}

// ==================== Helper Functions ====================

string Aircraft::selectAircraftStatus()
{
	vector<string> statusOptions = {
		"Available",
		"In Flight",
		"Maintenance",
		"Out of Service"
	};
	
	ui->displayMenu("Select Aircraft Status", statusOptions);
	int choice = ui->getChoice("Enter status: ", 1, 4);
	
	return statusOptions[choice - 1];
}

// ==================== Operational Methods ====================

vector<string> Aircraft::getAllAircraftTypes()
{
	vector<string> types;
	json allAircraftData = db->loadAll();
	
	for (const auto& [type, data] : allAircraftData.items())
	{
		types.push_back(type);
	}
	
	return types;
}

bool Aircraft::aircraftTypeExists(const string& aircraftType)
{
	return db->entryExists(aircraftType);
}

// ==================== Static initialization ====================

void Aircraft::initializeAircraftSystem()
{
	ui = UserInterface::getInstance();
	db = std::make_unique<Database>("Databases/Aircraft.json");
}

// ==================== AircraftException Class ====================

AircraftException::AircraftException(AircraftErrorCode code): errorCode(code) {}

const char* AircraftException::what() const noexcept
{
	return getErrorMessage().c_str();
}

AircraftErrorCode AircraftException::getErrorCode() const noexcept
{
	return errorCode;
}

string AircraftException::getErrorMessage() const noexcept
{
	switch (errorCode)
	{
		case AircraftErrorCode::AIRCRAFT_NOT_FOUND:
			return "Aircraft type does not exist.";
		case AircraftErrorCode::AIRCRAFT_EXISTS:
			return "Aircraft type already exists in the system.";
		case AircraftErrorCode::INVALID_AIRCRAFT_TYPE:
			return "Invalid aircraft type. Must be " + std::to_string(MIN_AIRCRAFT_TYPE_LENGTH) + "-" + 
				   std::to_string(MAX_AIRCRAFT_TYPE_LENGTH) + " characters, alphanumeric only, no spaces.";
		case AircraftErrorCode::INVALID_SEAT_LAYOUT:
			return "Invalid seat layout. Expected format: N-N or N-N-N (e.g., 3-3 or 2-4-2).";
		case AircraftErrorCode::INVALID_SEAT_NUMBER:
			return "Invalid seat number or count. Total seats must be between " + 
				   std::to_string(MIN_SEATS) + " and " + std::to_string(MAX_SEATS) + ".";
		case AircraftErrorCode::DATABASE_ERROR:
			return "An error occurred while accessing the aircraft database.";
		default:
			return "An unknown error occurred.";
	}
}