#include <iostream>
#include <fstream>
#include <sstream>
#include <algorithm>
#include <cctype>
#include <iomanip>
#include "Flight.hpp"

// ==================== Flight Class ====================

// Static member initialization
UserInterface* Flight::ui = nullptr;
string Flight::flightsFilePath = "Databases/Flights.json";

// ==================== Constructors ====================

Flight::Flight()
{
	string flightNumber = ui->getString("Enter Flight Number: ");

	// Validate flight number
	if (!validateFlightNumber(flightNumber))
	{
		throw FlightException(FlightErrorCode::INVALID_FLIGHT_NUMBER);
	}
	
	// Set the flight number
	this->flightNumber = flightNumber;
	
	// Get flight details from user
	ui->println("\n--- Enter Flight Details ---");
	string origin = ui->getString("Enter Origin: ");
	string destination = ui->getString("Enter Destination: ");
	string departureDateTime = ui->getDate("Enter Departure Date and Time (YYYY-MM-DD HH:MM): ", "YYYY-MM-DD HH:MM");
	string arrivalDateTime = ui->getDate("Enter Arrival Date and Time (YYYY-MM-DD HH:MM): ",  "YYYY-MM-DD HH:MM");
	string aircraftType = ui->getString("Enter Aircraft Type: ");
	int totalSeats = ui->getInt("Enter Total Seats: ");
	
	// Select flight status
	string status = selectFlightStatus();
	
	// Load existing flights data
	json flightsData = loadAllFlightsData();
	
	// Check if flight number already exists
	if (flightsData.contains(flightNumber))
	{
		throw FlightException(FlightErrorCode::FLIGHT_EXISTS);
	}
	
	// Create flight entry in JSON
	json flightData;
	flightData["origin"] = origin;
	flightData["destination"] = destination;
	flightData["departureDateTime"] = departureDateTime;
	flightData["arrivalDateTime"] = arrivalDateTime;
	flightData["aircraftType"] = aircraftType;
	flightData["totalSeats"] = totalSeats;
	flightData["status"] = status;
	flightData["price"] = 0.0;
	flightData["gate"] = "N/A";
	flightData["boardingTime"] = "N/A";
	flightData["reservedSeats"] = json::array();
	
	// Save to file
	flightsData[this->flightNumber] = flightData;
	saveAllFlightsData(flightsData);
	
	ui->printSuccess("Flight '" + flightNumber + "' created successfully.");
}

Flight::Flight(const string& flightNumber) : flightNumber(flightNumber)
{
	// Load flight data to verify flight exists
	json flightData = getFlightData();
	if (flightData.empty())
	{
		throw FlightException(FlightErrorCode::FLIGHT_NOT_FOUND);
	}
}

// ==================== Validation Methods ====================

bool Flight::validateFlightNumber(const string& flightNumber)
{
	// Check length
	if (flightNumber.length() < MIN_FLIGHT_NUMBER_LENGTH || 
		flightNumber.length() > MAX_FLIGHT_NUMBER_LENGTH)
	{
		return false;
	}
	
	// Check characters (alphanumeric only)
	return std::all_of(flightNumber.begin(), flightNumber.end(), 
		[](unsigned char c) { return std::isalnum(c); });
}

// ==================== Flight Management Menu ====================

void Flight::manageFlights()
{
	while (true)
	{
		ui->clearScreen();
		
		vector<string> options = {
			"Add New Flight",
			"View All Flights",
			"Update Existing Flight",
			"Remove Flight",
			"Back to Main Menu"
		};
		
		ui->displayMenu("Manage Flights", options);
		int choice = ui->getChoice("Enter choice: ", 1, 5);
		
		switch (choice)
		{
			case 1:
				addFlight();
				break;
			case 2:
				viewAllFlights();
				break;
			case 3:
				updateFlight();
				break;
			case 4:
				removeFlight();
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

void Flight::addFlight()
{
	ui->clearScreen();
	ui->printHeader("ADD NEW FLIGHT");
	
	try
	{
		Flight newFlight;
	}
	catch (const std::exception& e)
	{
		ui->printError(string(e.what()));
	}
	
	ui->pauseScreen();
}

void Flight::viewAllFlights()
{
	ui->clearScreen();
	ui->printHeader("ALL FLIGHTS");
	
	json flightsData = loadAllFlightsData();
	
	if (flightsData.empty())
	{
		ui->printWarning("No flights found in the system.");
	}
	else
	{
		vector<string> headers = {
			"Flight Number", "Origin", "Destination", "Departure", "Status", "Total Seats", "Available"
		};
		
		vector<vector<string>> rows;
		
		for (const auto& [flightNum, flightData] : flightsData.items())
		{
			string origin = flightData.value("origin", "N/A");
			string destination = flightData.value("destination", "N/A");
			string departure = flightData.value("departureDateTime", "N/A");
			string status = flightData.value("status", "N/A");
			int totalSeats = flightData.value("totalSeats", 0);
			
			// Calculate available seats
			int reserved = 0;
			if (flightData.contains("reservedSeats"))
			{
				reserved = flightData["reservedSeats"].size();
			}
			int available = totalSeats - reserved;
			
			rows.push_back({
				flightNum,
				origin,
				destination,
				departure,
				status,
				std::to_string(totalSeats),
				std::to_string(available)
			});
		}
		
		ui->displayTable(headers, rows);
		ui->println("\nTotal Flights: " + std::to_string(flightsData.size()));
	}
	
	ui->pauseScreen();
}

void Flight::updateFlight()
{
	ui->clearScreen();
	ui->printHeader("UPDATE EXISTING FLIGHT");
	
	try
	{
		string flightNumber = ui->getString("Enter Flight Number to Update: ");
		unique_ptr<Flight> flight = std::make_unique<Flight>(flightNumber);
		
		// Display current information
		ui->println("\nCurrent Flight Information:");
		ui->println("Origin: " + flight->getOrigin());
		ui->println("Destination: " + flight->getDestination());
		ui->println("Departure: " + flight->getDepartureDateTime());
		ui->println("Arrival: " + flight->getArrivalDateTime());
		ui->println("Aircraft Type: " + flight->getAircraftType());
		ui->println("Status: " + flight->getStatus());
		ui->println("Price: $" + std::to_string(flight->getPrice()));
		ui->println("Gate: " + flight->getGate());
		ui->println("Boarding Time: " + flight->getBoardingTime());
		
		vector<string> options = {
			"Status",
			"Price",
			"Gate",
			"Boarding Time",
			"Cancel"
		};
		
		ui->displayMenu("\nWhat would you like to update?", options);
		int choice = ui->getChoice("Enter choice: ", 1, 5);
		
		switch (choice)
		{
			case 1:
			{
				string newStatus = flight->selectFlightStatus();
				flight->setStatus(newStatus);
				ui->printSuccess("Flight status updated successfully.");
				break;
			}
			case 2:
			{
				double newPrice = ui->getDouble("Enter new price: ");
				flight->setPrice(newPrice);
				ui->printSuccess("Flight price updated successfully.");
				break;
			}
			case 3:
			{
				string newGate = ui->getString("Enter new gate: ");
				flight->setGate(newGate);
				ui->printSuccess("Gate updated successfully.");
				break;
			}
			case 4:
			{
				string newBoardingTime = ui->getDate("Enter new boarding time (HH:MM): ", "HH:MM");
				flight->setBoardingTime(newBoardingTime);
				ui->printSuccess("Boarding time updated successfully.");
				break;
			}
			case 5:
				ui->printWarning("Update canceled.");
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

void Flight::removeFlight()
{
	ui->clearScreen();
	ui->printHeader("REMOVE FLIGHT");
	
	try
	{
		string flightNumber = ui->getString("Enter Flight Number to Remove: ");
		
		// Check if flight exists
		json flightsData = loadAllFlightsData();
		if (!flightsData.contains(flightNumber))
		{
			throw FlightException(FlightErrorCode::FLIGHT_NOT_FOUND);
		}
		
		bool confirm = ui->getYesNo("Are you sure you want to remove flight '" + flightNumber + "'?");
		if (confirm)
		{
			flightsData.erase(flightNumber);
			saveAllFlightsData(flightsData);
			
			ui->printSuccess("Flight '" + flightNumber + "' has been removed successfully.");
		}
		else
		{
			ui->printWarning("Flight removal canceled.");
		}
	}
	catch (const std::exception& e)
	{
		ui->printError(string(e.what()));
	}
	
	ui->pauseScreen();
}

// ==================== Getters ====================

string Flight::getFlightNumber() const noexcept
{
	return flightNumber;
}

string Flight::getOrigin() const
{
	const json& flightData = getFlightData();
	return flightData["origin"];
}

string Flight::getDestination() const
{
	const json& flightData = getFlightData();
	return flightData["destination"];
}

string Flight::getDepartureDateTime() const
{
	const json& flightData = getFlightData();
	return flightData["departureDateTime"];
}

string Flight::getArrivalDateTime() const
{
	const json& flightData = getFlightData();
	return flightData["arrivalDateTime"];
}

string Flight::getAircraftType() const
{
	const json& flightData = getFlightData();
	return flightData["aircraftType"];
}

string Flight::getStatus() const
{
	const json& flightData = getFlightData();
	return flightData.value("status", "Scheduled");
}

double Flight::getPrice() const
{
	const json& flightData = getFlightData();
	return flightData.value("price", 0.0);
}

int Flight::getTotalSeats() const
{
	const json& flightData = getFlightData();
	return flightData["totalSeats"];
}

string Flight::getGate() const
{
	const json& flightData = getFlightData();
	return flightData.value("gate", "N/A");
}

string Flight::getBoardingTime() const
{
	const json& flightData = getFlightData();
	return flightData.value("boardingTime", "N/A");
}

// ==================== Setters ====================

void Flight::setStatus(const string& status)
{
	json updates;
	updates["status"] = status;
	updateFlightData(updates);
}

void Flight::setPrice(double price)
{
	json updates;
	updates["price"] = price;
	updateFlightData(updates);
}

void Flight::setGate(const string& gate)
{
	json updates;
	updates["gate"] = gate;
	updateFlightData(updates);
}

void Flight::setBoardingTime(const string& boardingTime)
{
	json updates;
	updates["boardingTime"] = boardingTime;
	updateFlightData(updates);
}

// ==================== Utility ====================

void Flight::displayFlightInfo() const
{
	ui->clearScreen();
	ui->printHeader("FLIGHT INFORMATION");
	ui->println("Flight Number: " + getFlightNumber());
	ui->println("Origin: " + getOrigin());
	ui->println("Destination: " + getDestination());
	ui->println("Departure: " + getDepartureDateTime());
	ui->println("Arrival: " + getArrivalDateTime());
	ui->println("Aircraft Type: " + getAircraftType());
	ui->println("Status: " + getStatus());
	ui->println("Price: $" + std::to_string(getPrice()));
	ui->println("Total Seats: " + std::to_string(getTotalSeats()));
	ui->println("Gate: " + getGate());
	ui->println("Boarding Time: " + getBoardingTime());
}

// ==================== JSON Operations ====================

json Flight::loadAllFlightsData()
{
	std::ifstream file(flightsFilePath);
	
	if (!file.is_open())
	{
		return json::object();
	}
	
	json data;
	try
	{
		file >> data;
	}
	catch (const json::exception& e)
	{
		throw FlightException(FlightErrorCode::DATABASE_ERROR);
	}
	
	file.close();
	return data;
}

void Flight::saveAllFlightsData(const json& data)
{
	std::ofstream file(flightsFilePath);
	
	if (!file.is_open())
	{
		throw FlightException(FlightErrorCode::DATABASE_ERROR);
	}
	
	try
	{
		file << data.dump(4);
	}
	catch (const json::exception& e)
	{
		throw FlightException(FlightErrorCode::DATABASE_ERROR);
	}
	
	file.close();
}

json Flight::getFlightData() const
{
	json flightsData = loadAllFlightsData();
	
	if (!flightsData.contains(flightNumber))
	{
		throw FlightException(FlightErrorCode::FLIGHT_NOT_FOUND);
	}
	
	return flightsData[flightNumber];
}

void Flight::updateFlightData(const json& updates)
{
	json flightsData = loadAllFlightsData();
	
	if (flightsData.contains(flightNumber))
	{
		for (const auto& [key, value] : updates.items())
		{
			flightsData[flightNumber][key] = value;
		}
		
		saveAllFlightsData(flightsData);
	}
	else
	{
		throw FlightException(FlightErrorCode::FLIGHT_NOT_FOUND);
	}
}

// ==================== Helper Functions ====================

string Flight::selectFlightStatus()
{
	vector<string> statusOptions = {
		"Scheduled",
		"Delayed",
		"Boarding",
		"Departed",
		"Arrived",
		"Canceled"
	};
	
	ui->displayMenu("Select Flight Status", statusOptions);
	int choice = ui->getChoice("Enter status: ", 1, 6);
	
	return statusOptions[choice - 1];
}

// ==================== Static initialization ====================

void Flight::initializeFlightSystem()
{
	ui = UserInterface::getInstance();
	
	// Create JSON file if it doesn't exist
	std::ifstream testFile(flightsFilePath);
	if (!testFile.is_open())
	{
		json emptyData = json::object();
		saveAllFlightsData(emptyData);
	}
	else
	{
		testFile.close();
	}
}

vector<unique_ptr<Flight>> Flight::searchFlights(const string& origin,
													  const string& destination,
													  const string& departureDate)
{
	vector<unique_ptr<Flight>> results;
	json flightsData = loadAllFlightsData();
	
	for (const auto& [flightNum, flightData] : flightsData.items())
	{
		string flightOrigin = flightData.value("origin", "");
		string flightDestination = flightData.value("destination", "");
		string departureDateTime = flightData.value("departureDateTime", "");
		
		// Extract date from departureDateTime (format: YYYY-MM-DD HH:MM)
		string flightDate = departureDateTime.substr(0, 10);
		
		// Case-insensitive comparison for origin and destination
		auto toLower = [](const string& str)
		{
			string result = str;
			std::transform(result.begin(), result.end(), result.begin(),
						 [](unsigned char c) { return std::tolower(c); });
			return result;
		};
		
		if (toLower(flightOrigin).find(toLower(origin)) != string::npos &&
			toLower(flightDestination).find(toLower(destination)) != string::npos &&
			flightDate == departureDate)
		{
			results.push_back(std::make_unique<Flight>(flightNum));
		}
	}
	
	return results;
}

// ==================== FlightException Class ====================

FlightException::FlightException(FlightErrorCode code)
	: errorCode(code)
{
}

const char* FlightException::what() const noexcept
{
	return getErrorMessage().c_str();
}

FlightErrorCode FlightException::getErrorCode() const noexcept
{
	return errorCode;
}

string FlightException::getErrorMessage() const noexcept
{
	switch (errorCode)
	{
		case FlightErrorCode::FLIGHT_NOT_FOUND:
			return "Flight does not exist.";
		case FlightErrorCode::FLIGHT_EXISTS:
			return "Flight already exists in the system.";
		case FlightErrorCode::INVALID_FLIGHT_NUMBER:
			return "Invalid flight number. Must be " + std::to_string(MIN_FLIGHT_NUMBER_LENGTH) + "-" + 
				   std::to_string(MAX_FLIGHT_NUMBER_LENGTH) + " characters, alphanumeric only.";
		case FlightErrorCode::DATABASE_ERROR:
			return "An error occurred while accessing the flights database.";
		default:
			return "An unknown error occurred.";
	}
}