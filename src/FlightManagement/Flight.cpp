#include <iostream>
#include <fstream>
#include <sstream>
#include <algorithm>
#include <cctype>
#include <iomanip>
#include "Flight.hpp"
#include "Aircraft.hpp"

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

	this->flightNumber = flightNumber;

	ui->println("\n--- Add New Flight ---");
	string origin = ui->getString("Enter Origin: ");
	string destination = ui->getString("Enter Destination: ");
	string departureDateTime = ui->getDate("Enter Departure Date and Time (YYYY-MM-DD HH:MM): ", "YYYY-MM-DD HH:MM");
	string arrivalDateTime = ui->getDate("Enter Arrival Date and Time (YYYY-MM-DD HH:MM): ", "YYYY-MM-DD HH:MM");
	
	// Display available aircraft types
	vector<string> aircraftTypes = Aircraft::getAllAircraftTypes();
	
	if (aircraftTypes.empty())
	{
		ui->printError("No aircraft types available. Please add aircraft types first in Aircraft Management.");
		throw FlightException(FlightErrorCode::INVALID_AIRCRAFT_TYPE);
	}

	ui->displayMenu("Available Aircraft Types:",aircraftTypes);
	int typeChoice = ui->getChoice("Select aircraft type: ", 1, aircraftTypes.size()+1);
	string aircraftType = aircraftTypes[typeChoice - 1];
	
	string status = selectFlightStatus();

	json allFlightsData = loadAllFlightsData();

	if (allFlightsData.contains(flightNumber))
	{
		throw FlightException(FlightErrorCode::FLIGHT_EXISTS);
	}

	json flightData;
	flightData["origin"] = origin;
	flightData["destination"] = destination;
	flightData["departureDateTime"] = departureDateTime;
	flightData["arrivalDateTime"] = arrivalDateTime;
	flightData["aircraftType"] = aircraftType;
	flightData["status"] = status;
	flightData["price"] = 0.0;
	flightData["gate"] = "N/A";
	flightData["boardingTime"] = "N/A";
	flightData["reservedSeats"] = json::array();

	allFlightsData[this->flightNumber] = flightData;
	saveAllFlightsData(allFlightsData);

	ui->println("");
	ui->printSuccess("Flight " + flightNumber + " has been successfully added to the schedule.");
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
			"Update Existing Flight",
			"Remove Flight",
			"View All Flights",
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
				updateFlight();
				break;
			case 3:
				removeFlight();
				break;
			case 4:
				viewAllFlights();
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
	ui->printHeader("Add New Flight");
	
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
	ui->printHeader("View All Flights");
	
	json allFlightsData = loadAllFlightsData();
	
	if (allFlightsData.empty())
	{
		ui->printWarning("No flights found in the system.");
	}
	else
	{
		vector<string> headers = {
			"Flight Number", "Origin", "Destination", "Departure", "Status", "Total Seats", "Available Seats"
		};
		
		vector<vector<string>> rows;
		
		for (const auto& [flightNum, flightData] : allFlightsData.items())
		{
			string origin = flightData.value("origin", "N/A");
			string destination = flightData.value("destination", "N/A");
			string departure = flightData.value("departureDateTime", "N/A");
			string status = flightData.value("status", "N/A");
			
			// Get total seats from Aircraft class
			int totalSeats = 0;
			int reserved = 0;
			int available = 0;

			try
			{
				string aircraftType = flightData.value("aircraftType", "");
				if (!aircraftType.empty())
				{
					Aircraft aircraft(aircraftType);
					totalSeats = aircraft.getTotalSeats();
					reserved = flightData.contains("reservedSeats") ? (int)flightData["reservedSeats"].size() : 0;
					available = totalSeats - reserved;
				}
			}
			catch (const AircraftException&)
			{
				totalSeats = 0;
				available = 0;
			}
			
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
		ui->println("\nTotal Flights: " + std::to_string(allFlightsData.size()));
	}
	
	ui->pauseScreen();
}

void Flight::updateFlight()
{
	ui->clearScreen();

	try
	{
		string flightNumber = ui->getString("Enter Flight Number to Update: ");
		unique_ptr<Flight> flight = std::make_unique<Flight>(flightNumber);

		ui->println("\nSelect information to update:");
		vector<string> options = {
			"Flight Details",
			"Crew Assignments",   // TODO
			"Status",
			"Back to Manage Flights"
		};

		ui->displayMenu("Update Existing Flight", options);
		int choice = ui->getChoice("Enter choice: ", 1, 4);

		switch (choice)
		{
			case 1:
				flight->updateFlightDetails();
				break;

			case 2:
				// TODO: Implement Crew Assignment logic
				ui->printWarning("TODO: Crew Assignments not yet implemented.");
				break;

			case 3:
			{
				string newStatus = flight->selectFlightStatus();
				flight->setStatus(newStatus);
				ui->printSuccess("Flight status updated successfully.");
				break;
			}

			case 4:
				ui->println("Returning to Manage Flights menu.");
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

// ==================== Update Flight Details ====================

void Flight::updateFlightDetails()
{
	ui->clearScreen();
	ui->printHeader("--- Update Flight Details ---");

	json allFlightsData = loadAllFlightsData();

	if (!allFlightsData.contains(flightNumber))
	{
		throw FlightException(FlightErrorCode::FLIGHT_NOT_FOUND);
	}

	json flightData = allFlightsData[flightNumber];

	ui->println("Current Flight Information:");
	ui->println("1. Origin: " + flightData.value("origin", "N/A"));
	ui->println("2. Destination: " + flightData.value("destination", "N/A"));
	ui->println("3. Departure: " + flightData.value("departureDateTime", "N/A"));
	ui->println("4. Arrival: " + flightData.value("arrivalDateTime", "N/A"));
	ui->println("5. Aircraft Type: " + flightData.value("aircraftType", "N/A"));
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
				string newOrigin = ui->getString("Enter new Origin: ");
				flightData["origin"] = newOrigin;
				break;
			}
			case 2:
			{
				string newDestination = ui->getString("Enter new Destination: ");
				flightData["destination"] = newDestination;
				break;
			}
			case 3:
			{
				string newDeparture = ui->getDate("Enter new Departure Date and Time (YYYY-MM-DD HH:MM): ", "YYYY-MM-DD HH:MM");
				flightData["departureDateTime"] = newDeparture;
				break;
			}
			case 4:
			{
				string newArrival = ui->getDate("Enter new Arrival Date and Time (YYYY-MM-DD HH:MM): ", "YYYY-MM-DD HH:MM");
				flightData["arrivalDateTime"] = newArrival;
				break;
			}
			case 5:
			{
				// Display available aircraft types
				vector<string> aircraftTypes = Aircraft::getAllAircraftTypes();
				
				if (aircraftTypes.empty())
				{
					ui->printError("No aircraft types available. Please add aircraft types first.");
					break;
				}
				
				ui->println("\nAvailable Aircraft Types:");
				for (size_t i = 0; i < aircraftTypes.size(); ++i)
				{
					ui->println(std::to_string(i + 1) + ". " + aircraftTypes[i]);
				}
				
				int typeChoice = ui->getChoice("Select aircraft type: ", 1, aircraftTypes.size());
				string newAircraftType = aircraftTypes[typeChoice - 1];
				
				// TODO: Check if changing aircraft type affects existing reservations
				// Should verify that all reserved seats are valid in new aircraft type
				if (flightData.contains("reservedSeats") && !flightData["reservedSeats"].empty())
				{
					ui->printWarning("Warning: This flight has existing seat reservations.");
					ui->printWarning("Changing aircraft type may invalidate some reservations.");
					bool confirm = ui->getYesNo("Continue with aircraft type change?");
					if (!confirm)
					{
						ui->printWarning("Aircraft type change canceled.");
						break;
					}
				}
				
				flightData["aircraftType"] = newAircraftType;
				break;
			}
			default:
				ui->printError("Invalid choice.");
				return;
		}

		// Save updates
		allFlightsData[flightNumber] = flightData;
		saveAllFlightsData(allFlightsData);
		ui->printSuccess("Flight details updated successfully.");
	}
	catch (const std::exception& e)
	{
		ui->printError("Error updating flight details: " + string(e.what()));
	}

	ui->pauseScreen();
}


void Flight::removeFlight()
{
	ui->clearScreen();
	ui->printHeader("Remove Flight");
	
	try
	{
		string flightNumber = ui->getString("Enter Flight Number to Remove: ");
		
		// Check if flight exists
		json allFlightsData = loadAllFlightsData();
		if (!allFlightsData.contains(flightNumber))
		{
			throw FlightException(FlightErrorCode::FLIGHT_NOT_FOUND);
		}
		
		// TODO: Check if flight has active reservations before removal
		
		bool confirm = ui->getYesNo("Are you sure you want to remove flight '" + flightNumber + "'?");
		if (confirm)
		{
			allFlightsData.erase(flightNumber);
			saveAllFlightsData(allFlightsData);
			
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
	// Query Aircraft class for this flight's aircraft type
	string aircraftType = getAircraftType();
	
	try
	{
		Aircraft aircraft(aircraftType);
		return aircraft.getTotalSeats();
	}
	catch (const AircraftException& e)
	{
		ui->printError("Aircraft type not found: " + aircraftType);
		return 0;
	}
}

int Flight::getAvailableSeats() const
{
	int totalSeats = getTotalSeats();
	const json& flightData = getFlightData();
	
	// Count reserved seats
	int reservedCount = 0;
	if (flightData.contains("reservedSeats") && flightData["reservedSeats"].is_array())
	{
		reservedCount = flightData["reservedSeats"].size();
	}
	
	return totalSeats - reservedCount;
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

// ==================== Seat Management ====================

vector<string> Flight::getReservedSeats() const
{
	const json& flightData = getFlightData();
	vector<string> reservedSeats;
	
	if (flightData.contains("reservedSeats") && flightData["reservedSeats"].is_array())
	{
		for (const auto& seat : flightData["reservedSeats"])
		{
			reservedSeats.push_back(seat.get<string>());
		}
	}
	
	return reservedSeats;
}

bool Flight::reserveSeat(const string& seatNumber)
{
	// First, validate seat exists in aircraft type
	string aircraftType = getAircraftType();
	
	try
	{
		Aircraft aircraft(aircraftType);
		
		if (!aircraft.isValidSeat(seatNumber))
		{
			ui->printError("Invalid seat number '" + seatNumber + "' for aircraft type " + aircraftType);
			return false;
		}
	}
	catch (const AircraftException& e)
	{
		ui->printError("Error validating seat: " + string(e.what()));
		return false;
	}
	
	// Check if seat is already reserved
	if (!isSeatAvailable(seatNumber))
	{
		ui->printError("Seat " + seatNumber + " is already reserved.");
		return false;
	}
	
	// Add seat to reservedSeats array
	json allFlightsData = loadAllFlightsData();
	
	if (!allFlightsData.contains(flightNumber))
	{
		throw FlightException(FlightErrorCode::FLIGHT_NOT_FOUND);
	}
	
	if (!allFlightsData[flightNumber].contains("reservedSeats"))
	{
		allFlightsData[flightNumber]["reservedSeats"] = json::array();
	}
	
	allFlightsData[flightNumber]["reservedSeats"].push_back(seatNumber);
	saveAllFlightsData(allFlightsData);
	
	return true;
}

bool Flight::releaseSeat(const string& seatNumber)
{
	json allFlightsData = loadAllFlightsData();
	
	if (!allFlightsData.contains(flightNumber))
	{
		throw FlightException(FlightErrorCode::FLIGHT_NOT_FOUND);
	}
	
	if (!allFlightsData[flightNumber].contains("reservedSeats") || 
		!allFlightsData[flightNumber]["reservedSeats"].is_array())
	{
		return false; // No reserved seats
	}
	
	auto& reservedSeats = allFlightsData[flightNumber]["reservedSeats"];
	
	// Find and remove the seat
	for (auto it = reservedSeats.begin(); it != reservedSeats.end(); ++it)
	{
		if (*it == seatNumber)
		{
			reservedSeats.erase(it);
			saveAllFlightsData(allFlightsData);
			return true;
		}
	}
	
	return false; // Seat not found in reserved list
}

bool Flight::isSeatAvailable(const string& seatNumber) const
{
	vector<string> reservedSeats = getReservedSeats();
	
	// Check if seat is in reserved list
	return std::find(reservedSeats.begin(), reservedSeats.end(), seatNumber) == reservedSeats.end();
}

void Flight::displaySeatMap() const
{
	ui->clearScreen();
	ui->printHeader("SEAT MAP - Flight " + flightNumber);
	
	string aircraftType = getAircraftType();
	ui->println("Aircraft Type: " + aircraftType);
	ui->println("Route: " + getOrigin() + " → " + getDestination());
	ui->println("");
	
	try
	{
		Aircraft aircraft(aircraftType);
		vector<string> allSeats = aircraft.generateSeatMap();
		vector<string> reservedSeats = getReservedSeats();
		
		string layout = aircraft.getSeatLayout();
		
		// Parse layout to determine seats per row
		int seatsPerRow = 0;
		std::stringstream ss(layout);
		string section;
		vector<int> sections;
		
		while (std::getline(ss, section, '-'))
		{
			int sectionSize = std::stoi(section);
			sections.push_back(sectionSize);
			seatsPerRow += sectionSize;
		}
		
		ui->println("Legend: [Available] [X Reserved]");
		ui->printSeparator();
		
		// Display seats row by row
		int rows = aircraft.getRows();
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
		ui->println("Total Seats: " + std::to_string(getTotalSeats()));
		ui->println("Available: " + std::to_string(getAvailableSeats()));
		ui->println("Reserved: " + std::to_string(reservedSeats.size()));
	}
	catch (const AircraftException& e)
	{
		ui->printError("Error displaying seat map: " + string(e.what()));
	}
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
	ui->println("Available Seats: " + std::to_string(getAvailableSeats()) + " / " + std::to_string(getTotalSeats()));
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
	json allFlightsData = loadAllFlightsData();
	
	if (!allFlightsData.contains(flightNumber))
	{
		throw FlightException(FlightErrorCode::FLIGHT_NOT_FOUND);
	}
	
	return allFlightsData[flightNumber];
}

void Flight::updateFlightData(const json& updates)
{
	json allFlightsData = loadAllFlightsData();
	
	if (allFlightsData.contains(flightNumber))
	{
		for (const auto& [key, value] : updates.items())
		{
			allFlightsData[flightNumber][key] = value;
		}
		
		saveAllFlightsData(allFlightsData);
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
	json allFlightsData = loadAllFlightsData();
	
	for (const auto& [flightNum, flightData] : allFlightsData.items())
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
			auto lowerCaseChar = [](unsigned char c) { return std::tolower(c); };
			std::transform(result.begin(), result.end(), result.begin(), lowerCaseChar);
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

FlightException::FlightException(FlightErrorCode code): errorCode(code) {}

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
		case FlightErrorCode::INVALID_AIRCRAFT_TYPE:
			return "Invalid or missing aircraft type. Please add aircraft types first.";
		case FlightErrorCode::DATABASE_ERROR:
			return "An error occurred while accessing the flights database.";
		default:
			return "An unknown error occurred.";
	}
}