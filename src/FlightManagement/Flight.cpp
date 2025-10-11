#include <iostream>
#include <fstream>
#include <sstream>
#include <algorithm>
#include <cctype>
#include <iomanip>
#include "Flight.hpp"
#include "Aircraft.hpp"
#include "SeatMap.hpp"

// ==================== Flight Class ====================

// Static member initialization
UserInterface* Flight::ui = nullptr;
std::unique_ptr<Database> Flight::db = nullptr;

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

	if (db->entryExists(flightNumber))
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

	db->addEntry(this->flightNumber, flightData);

	ui->println("");
	ui->printSuccess("Flight " + flightNumber + " has been successfully added to the schedule.");
}

Flight::Flight(const string& flightNumber) : flightNumber(flightNumber)
{
	// Load flight data to verify flight exists
	if (!db->entryExists(flightNumber))
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
	
	json allFlightsData = db->loadAll();
	
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
					string seatLayout = aircraft.getSeatLayout();
					int rows = aircraft.getRows();
					totalSeats = SeatMap::calculateSeatCount(seatLayout, rows);
					reserved = flightData.contains("reservedSeats") ? (int)flightData["reservedSeats"].size() : 0;
					available = totalSeats - reserved;
				}
			}
			catch (const AircraftException& e)
			{
				// Aircraft not found, continue with zeros
				totalSeats = 0;
				available = 0;
			}
			catch (const SeatMapException& e)
			{
				// Invalid seat configuration, continue with zeros
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

	json flightData = db->getEntry(flightNumber);

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
				db->setAttribute(flightNumber, "origin", newOrigin);
				break;
			}
			case 2:
			{
				string newDestination = ui->getString("Enter new Destination: ");
				db->setAttribute(flightNumber, "destination", newDestination);
				break;
			}
			case 3:
			{
				string newDeparture = ui->getDate("Enter new Departure Date and Time (YYYY-MM-DD HH:MM): ", "YYYY-MM-DD HH:MM");
				db->setAttribute(flightNumber, "departureDateTime", newDeparture);
				break;
			}
			case 4:
			{
				string newArrival = ui->getDate("Enter new Arrival Date and Time (YYYY-MM-DD HH:MM): ", "YYYY-MM-DD HH:MM");
				db->setAttribute(flightNumber, "arrivalDateTime", newArrival);
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
				
				// Validate reserved seats against new aircraft type
				if (flightData.contains("reservedSeats") && !flightData["reservedSeats"].empty())
				{
					ui->printWarning("Warning: This flight has existing seat reservations.");
					
					try
					{
						// Get new aircraft seat configuration
						Aircraft newAircraft(newAircraftType);
						string newLayout = newAircraft.getSeatLayout();
						int newRows = newAircraft.getRows();
						
						// Create SeatMap for new aircraft
						SeatMap newSeatMap(newLayout, newRows);
						
						// Validate each reserved seat against new aircraft
						vector<string> invalidSeats;
						for (const auto& seat : flightData["reservedSeats"])
						{
							string seatNumber = seat.get<string>();
							if (!newSeatMap.isValidSeat(seatNumber))
							{
								invalidSeats.push_back(seatNumber);
							}
						}
						
						if (!invalidSeats.empty())
						{
							ui->printError("The following reserved seats are invalid for the new aircraft type:");
							for (const auto& seat : invalidSeats)
							{
								ui->println("  - " + seat);
							}
							ui->printWarning("Changing aircraft type will invalidate these reservations.");
							
							bool confirm = ui->getYesNo("Continue with aircraft type change?");
							if (!confirm)
							{
								ui->printWarning("Aircraft type change canceled.");
								break;
							}
						}
						else
						{
							ui->printSuccess("All reserved seats are valid for the new aircraft type.");
							bool confirm = ui->getYesNo("Continue with aircraft type change?");
							if (!confirm)
							{
								ui->printWarning("Aircraft type change canceled.");
								break;
							}
						}
					}
					catch (const AircraftException& e)
					{
						ui->printError("Error validating new aircraft type: " + string(e.what()));
						break;
					}
					catch (const SeatMapException& e)
					{
						ui->printError("Error validating seats: " + string(e.what()));
						break;
					}
				}
				
				db->setAttribute(flightNumber, "aircraftType", newAircraftType);
				break;
			}
			default:
				ui->printError("Invalid choice.");
				return;
		}

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
		if (!db->entryExists(flightNumber))
		{
			throw FlightException(FlightErrorCode::FLIGHT_NOT_FOUND);
		}
		
		// TODO: Check if flight has active reservations before removal
		
		bool confirm = ui->getYesNo("Are you sure you want to remove flight '" + flightNumber + "'?");
		if (confirm)
		{
			db->deleteEntry(flightNumber);
			
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
	return db->getAttribute(flightNumber, "origin");
}

string Flight::getDestination() const
{
	return db->getAttribute(flightNumber, "destination");
}

string Flight::getDepartureDateTime() const
{
	return db->getAttribute(flightNumber, "departureDateTime");
}

string Flight::getArrivalDateTime() const
{
	return db->getAttribute(flightNumber, "arrivalDateTime");
}

string Flight::getAircraftType() const
{
	return db->getAttribute(flightNumber, "aircraftType");
}

string Flight::getStatus() const
{
	return db->getAttribute(flightNumber, "status");
}

double Flight::getPrice() const
{
	return db->getAttribute(flightNumber, "price");
}

int Flight::getTotalSeats() const
{
	// Query Aircraft class for this flight's aircraft type
	string aircraftType = getAircraftType();
	
	try
	{
		Aircraft aircraft(aircraftType);
		string seatLayout = aircraft.getSeatLayout();
		int rows = aircraft.getRows();
		return SeatMap::calculateSeatCount(seatLayout, rows);
	}
	catch (const AircraftException& e)
	{
		throw FlightException(FlightErrorCode::INVALID_AIRCRAFT_TYPE);
	}
	catch (const SeatMapException& e)
	{
		throw FlightException(FlightErrorCode::DATABASE_ERROR);
	}
}

int Flight::getAvailableSeats() const
{
	int totalSeats = getTotalSeats();
	
	// Count reserved seats
	json reservedSeats = db->getAttribute(flightNumber, "reservedSeats");
	int reservedCount = 0;
	
	if (reservedSeats.is_array())
	{
		reservedCount = reservedSeats.size();
	}
	
	return totalSeats - reservedCount;
}

string Flight::getGate() const
{
	return db->getAttribute(flightNumber, "gate");
}

string Flight::getBoardingTime() const
{
	return db->getAttribute(flightNumber, "boardingTime");
}

// ==================== Setters ====================

void Flight::setStatus(const string& status)
{
	db->setAttribute(flightNumber, "status", status);
}

void Flight::setPrice(double price)
{
	db->setAttribute(flightNumber, "price", price);
}

void Flight::setGate(const string& gate)
{
	db->setAttribute(flightNumber, "gate", gate);
}

void Flight::setBoardingTime(const string& boardingTime)
{
	db->setAttribute(flightNumber, "boardingTime", boardingTime);
}

// ==================== Seat Management ====================

vector<string> Flight::getReservedSeats() const
{
	vector<string> reservedSeats;
	
	json seats = db->getAttribute(flightNumber, "reservedSeats");
	
	if (seats.is_array())
	{
		for (const auto& seat : seats)
		{
			reservedSeats.push_back(seat.get<string>());
		}
	}
	
	return reservedSeats;
}

bool Flight::reserveSeat(const string& seatNumber)
{
	// Get aircraft information
	string aircraftType = getAircraftType();
	
	try
	{
		Aircraft aircraft(aircraftType);
		string seatLayout = aircraft.getSeatLayout();
		int rows = aircraft.getRows();
		
		// Get current reserved seats from database
		vector<string> currentReservedSeats = getReservedSeats();
		
		// Create SeatMap instance with current reservations
		SeatMap seatMap(seatLayout, rows, currentReservedSeats);
		
		// Try to reserve the seat (will throw exception if invalid or already reserved)
		seatMap.reserveSeat(seatNumber);
		
		// Update database with new reservation
		json reservedSeats = db->getAttribute(flightNumber, "reservedSeats");
		
		if (!reservedSeats.is_array())
		{
			reservedSeats = json::array();
		}
		
		reservedSeats.push_back(seatNumber);
		db->setAttribute(flightNumber, "reservedSeats", reservedSeats);
		
		return true;
	}
	catch (const SeatMapException& e)
	{
		ui->printError(string(e.what()));
		return false;
	}
	catch (const AircraftException& e)
	{
		ui->printError(string(e.what()));
		return false;
	}
	catch (const std::exception& e)
	{
		ui->printError("Error reserving seat: " + string(e.what()));
		return false;
	}
}

bool Flight::releaseSeat(const string& seatNumber)
{
	json reservedSeats = db->getAttribute(flightNumber, "reservedSeats");
	
	if (!reservedSeats.is_array())
	{
		return false; // No reserved seats
	}
	
	// Find and remove the seat
	for (auto it = reservedSeats.begin(); it != reservedSeats.end(); ++it)
	{
		if (*it == seatNumber)
		{
			reservedSeats.erase(it);
			db->setAttribute(flightNumber, "reservedSeats", reservedSeats);
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
	try
	{
		string aircraftType = getAircraftType();
		Aircraft aircraft(aircraftType);
		
		string seatLayout = aircraft.getSeatLayout();
		int rows = aircraft.getRows();
		vector<string> reservedSeats = getReservedSeats();
		
		// Create SeatMap instance with current reservations
		SeatMap seatMap(seatLayout, rows, reservedSeats);
		
		// Display the seat map
		seatMap.displaySeatMap(flightNumber, getOrigin(), getDestination(), aircraftType);
	}
	catch (const std::exception& e)
	{
		ui->printError("Error displaying seat map: " + string(e.what()));
	}
}

// ==================== Utility ====================

void Flight::displayFlightInfo() const
{
	ui->clearScreen();
	ui->printHeader("FLIGHT INFORMATION");
	
	json flightData = db->getEntry(flightNumber);
	
	ui->println("Flight Number: " + getFlightNumber());
	ui->println("Origin: " + flightData.value("origin", "N/A"));
	ui->println("Destination: " + flightData.value("destination", "N/A"));
	ui->println("Departure: " + flightData.value("departureDateTime", "N/A"));
	ui->println("Arrival: " + flightData.value("arrivalDateTime", "N/A"));
	ui->println("Aircraft Type: " + flightData.value("aircraftType", "N/A"));
	ui->println("Status: " + flightData.value("status", "N/A"));
	ui->println("Price: $" + std::to_string(flightData.value("price", 0.0)));
	ui->println("Available Seats: " + std::to_string(getAvailableSeats()) + " / " + std::to_string(getTotalSeats()));
	ui->println("Gate: " + flightData.value("gate", "N/A"));
	ui->println("Boarding Time: " + flightData.value("boardingTime", "N/A"));
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
	db = std::make_unique<Database>("Databases/Flights.json");
}

vector<unique_ptr<Flight>> Flight::searchFlights(const string& origin,
													  const string& destination,
													  const string& departureDate)
{
	vector<unique_ptr<Flight>> results;
	json allFlightsData = db->loadAll();
	
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