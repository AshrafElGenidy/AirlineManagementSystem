#include <iostream>
#include <algorithm>
#include <iomanip>
#include "FlightManager.hpp"
#include "ReservationManager.hpp"
#include "Crew.hpp"
#include "CrewManager.hpp"

// Static member initialization
FlightManager* FlightManager::instance = nullptr;

// ==================== Constructor & Singleton ====================

FlightManager::FlightManager()
{
	db = std::make_unique<Database>("Flights");
	ui = UserInterface::getInstance();
	creator = std::make_unique<FlightCreator>();
}

FlightManager* FlightManager::getInstance()
{
	if (instance == nullptr)
	{
		instance = new FlightManager();
	}
	return instance;
}

FlightManager::~FlightManager() noexcept {}

// ==================== Menu Methods ====================

void FlightManager::manageFlights()
{
	while (true)
	{
		ui->clearScreen();
		
		vector<string> options = {
			"Add New Flight",
			"View All Flights",
			"Update Flight",
			"Remove Flight",
			"Search Flights",
			"Back to Main Menu"
		};
		
		ui->displayMenu("Manage Flights", options);
		
		try
		{
			int choice = ui->getChoice("Enter choice: ", 1, 6);
			
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
					searchFlights();
					break;
				case 6:
					return;
				default:
					ui->printError("Invalid choice.");
					ui->pauseScreen();
					break;
			}
		}
		catch (const UIException& e)
		{
			ui->printError(string(e.what()));
			ui->pauseScreen();
		}
	}
}

void FlightManager::addFlight()
{
	ui->clearScreen();
	ui->printHeader("Add New Flight");
	
	try
	{
		shared_ptr<Flight> newFlight = creator->createNewFlight();
		
		if (!newFlight)
		{
			return;
		}
		
		// Check if flight already exists
		if (db->entryExists(newFlight->getFlightNumber()))
		{
			ui->printError("Flight " + newFlight->getFlightNumber() + " already exists.");
			ui->pauseScreen();
			return;
		}
		
		// Save to database
		saveFlightToDatabase(newFlight);
		ui->printSuccess("Flight " + newFlight->getFlightNumber() + " has been successfully added.");
	}
	catch (const std::exception& e)
	{
		ui->printError(string(e.what()));
	}
	
	ui->pauseScreen();
}

void FlightManager::viewAllFlights()
{
	ui->clearScreen();
	ui->printHeader("View All Flights");
	
	try
	{
		json allFlightsData = db->loadAll();
		
		if (allFlightsData.empty())
		{
			ui->printWarning("No flights found in the system.");
			ui->pauseScreen();
			return;
		}
		
		vector<shared_ptr<Flight>> flights;
		
		for (const auto& [flightNum, flightData] : allFlightsData.items())
		{
			try
			{
				flights.push_back(creator->createFromJson(flightNum, flightData));
			}
			catch (const std::exception& e)
			{
				// Skip flights with errors
				continue;
			}
		}
		
		displayFlightsTable(flights, "All Flights");
		ui->println("\nTotal Flights: " + std::to_string(flights.size()));
	}
	catch (const std::exception& e)
	{
		ui->printError(string(e.what()));
	}
	
	ui->pauseScreen();
}

void FlightManager::updateFlight()
{
	ui->clearScreen();
	ui->printHeader("Update Flight");
	
	try
	{
		string flightNumber = ui->getString("Enter Flight Number to Update: ");
		
		shared_ptr<Flight> flight = loadFlightFromDatabase(flightNumber);
		if (!flight)
		{
			ui->printError("Flight not found.");
			ui->pauseScreen();
			return;
		}
		
		vector<string> options = {
			"Flight Details",
			"Assign Crew to Flight",
			"Status",
			"Price",
			"Back to Manage Flights"
		};
		
		ui->displayMenu("Update Flight", options);
		
		int choice = ui->getChoice("Enter choice: ", 1, 4);
		
		switch (choice)
		{
			case 1:
				updateFlightDetails(flight);
				break;
			case 2:
				assignCrewToFlight(flight);
				saveFlightToDatabase(flight);
				break;
			case 3:
			{
				string newStatus = selectFlightStatus();
				flight->setStatus(newStatus);
				ui->printSuccess("Flight status updated successfully.");
				saveFlightToDatabase(flight);
				if (newStatus == "Arrived")
				{
					updateCrewFlightHours(flight);
				}
				break;
			}
			case 4:
			{
				double newPrice = ui->getDouble("Enter new Price: ");
				if (newPrice > 0)
				{
					flight->setPrice(newPrice);
					ui->printSuccess("Flight price updated successfully.");
					saveFlightToDatabase(flight);
				}
				else
				{
					ui->printError("Price must be positive.");
				}
				break;
			}
			case 5:
				ui->println("Returning to Manage Flights menu.");
				ui->pauseScreen();
				return;
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

void FlightManager::removeFlight()
{
	ui->clearScreen();
	ui->printHeader("Remove Flight");
	
	try
	{
		string flightNumber = ui->getString("Enter Flight Number to Remove: ");
		
		if (!db->entryExists(flightNumber))
		{
			ui->printError("Flight not found.");
			ui->pauseScreen();
			return;
		}
		
		// Check for active reservations
		if (ReservationManager::hasActiveReservations(flightNumber))
		{
			ui->printError("Cannot delete flight with active reservations.");
			ui->println("Please cancel all reservations before deleting the flight.");
			ui->pauseScreen();
			return;
		}

		bool confirm = ui->getYesNo("Are you sure you want to remove flight '" + flightNumber + "'?");
		if (confirm)
		{
			deleteFlightFromDatabase(flightNumber);
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

void FlightManager::searchFlights()
{
	ui->clearScreen();
	ui->printHeader("Search Flights");
	
	try
	{
		string origin = ui->getString("Enter Origin: ");
		string destination = ui->getString("Enter Destination: ");
		string departureDate = ui->getDate("Enter Departure Date: ", "YYYY-MM-DD");
		
		vector<shared_ptr<Flight>> results = searchFlightsByRoute(origin, destination, departureDate);
		
		if (results.empty())
		{
			ui->printWarning("No flights found matching your search criteria.");
		}
		else
		{
			displayFlightsTable(results, "Search Results");
		}
	}
	catch (const std::exception& e)
	{
		ui->printError(string(e.what()));
	}
	
	ui->pauseScreen();
}

void FlightManager::assignCrewToFlight(const shared_ptr<Flight>& flight)
{
	ui->clearScreen();
	ui->printHeader("Assign Crew to Flight");
	
	try
	{
		vector<string> roleOptions = {
			"Pilot",
			"Copilot",
			"Flight Attendant"
		};
		
		ui->displayMenu("Select Crew Role", roleOptions);
		int roleChoice = ui->getChoice("Enter role: ", 1, static_cast<int>(roleOptions.size()));
		CrewRole selectedRole = Crew::stringToRole(roleOptions[roleChoice - 1]);
		
		CrewManager* crewMgr = CrewManager::getInstance();
		vector<shared_ptr<Crew>> availableCrew = crewMgr->getAvailableCrew(selectedRole);
		crewMgr->displayCrewTable(availableCrew, "Available crew for selection");
		
		string crewId = ui->getString("Enter Crew ID to assign: ");
		
		shared_ptr<Crew> selectedCrew = crewMgr->getCrew(crewId);
		if (!selectedCrew)
		{
			ui->printError("Crew member not found.");
			ui->pauseScreen();
			return;
		}
		
		flight->addCrewMember(crewId);
		saveFlightToDatabase(flight);
		crewMgr->markCrewAsAssigned(crewId);
		
		ui->printSuccess("Crew member " + crewId + " has been assigned to flight " + flight->getFlightNumber());
	}
	catch (const std::exception& e)
	{
		ui->printError(string(e.what()));
	}
	
	ui->pauseScreen();
}

// ==================== Helper Methods ====================

shared_ptr<Flight> FlightManager::loadFlightFromDatabase(const string& flightNumber)
{
	if (!db->entryExists(flightNumber))
	{
		return nullptr;
	}
	
	try
	{
		json flightData = db->getEntry(flightNumber);
		return creator->createFromJson(flightNumber, flightData);
	}
	catch (const std::exception& e)
	{
		ui->printError(string(e.what()));
		return nullptr;
	}
}

void FlightManager::saveFlightToDatabase(const shared_ptr<Flight>& flight)
{
	if (!flight)
	{
		throw FlightException("An error occurred while accessing the database.");
	}
	
	try
	{
		json flightData = creator->toJson(flight);
		if (db->entryExists(flight->getFlightNumber()))
		{
			db->updateEntry(flight->getFlightNumber(), flightData);
		}
		else
		{
			db->addEntry(flight->getFlightNumber(), flightData);
		}
	}
	catch (const DatabaseException& e)
	{
		throw FlightException("An error occurred while accessing the database." + string(e.what()));
	}
}

void FlightManager::deleteFlightFromDatabase(const string& flightNumber)
{
	try
	{
		db->deleteEntry(flightNumber);
	}
	catch (const DatabaseException& e)
	{
		throw FlightException("An error occurred while accessing the database." + string(e.what()));
	}
}

void FlightManager::updateFlightDetails(const shared_ptr<Flight>& flight)
{
	ui->clearScreen();
	ui->printHeader("Update Flight Details");
	
	ui->println("Current Flight Information:");
	ui->println("1. Origin: " + flight->getOrigin());
	ui->println("2. Destination: " + flight->getDestination());
	ui->println("3. Departure: " + flight->getDepartureDateTime());
	ui->println("4. Arrival: " + flight->getArrivalDateTime());
	ui->println("5. Aircraft Type: " + flight->getAircraftType());
	ui->println("6. Gate: " + flight->getGate());
	ui->println("7. Boarding Time: " + flight->getBoardingTime());
	ui->println("8. Back to Previous Menu\n");
	
	try
	{
		int choice = ui->getChoice("Select field to update (1-8): ", 1, 8);
		
		switch (choice)
		{
			case 1:  // Update Origin
			{
				if (hasActiveReservations(flight->getFlightNumber()))
				{
					ui->printError("Cannot modify origin for flights with active reservations.");
					break;
				}
				
				string newOrigin = creator->getValidOrigin();
				flight->setOrigin(newOrigin);
				saveFlightToDatabase(flight);
				ui->printSuccess("Origin updated successfully.");
				break;
			}
			case 2:  // Update Destination
			{
				if (hasActiveReservations(flight->getFlightNumber()))
				{
					ui->printError("Cannot modify destination for flights with active reservations.");
					break;
				}
				
				string newDestination = creator->getValidDestination();
				flight->setDestination(newDestination);
				saveFlightToDatabase(flight);
				ui->printSuccess("Destination updated successfully.");
				break;
			}
			case 3:  // Update Departure DateTime
			{
				if (hasActiveReservations(flight->getFlightNumber()))
				{
					ui->printWarning("This flight has active reservations.");
					bool confirm = ui->getYesNo("Continue with departure time update?");
					if (!confirm)
					{
						ui->printWarning("Departure time update canceled.");
						break;
					}
				}
				
				string newDeparture = creator->getValidDepartureDateTime();
				flight->setDepartureDateTime(newDeparture);
				saveFlightToDatabase(flight);
				ui->printSuccess("Departure date and time updated successfully.");
				break;
			}
			case 4:  // Update Arrival DateTime
			{
				string newArrival = creator->getValidArrivalDateTime();
				flight->setArrivalDateTime(newArrival);
				saveFlightToDatabase(flight);
				ui->printSuccess("Arrival date and time updated successfully.");
				break;
			}
			case 5:  // Update Aircraft Type
			{
				if (hasActiveReservations(flight->getFlightNumber()))
				{
					ui->printError("Cannot change aircraft type for flights with active reservations.");
					ui->println("Reason: Seat reservations are tied to the current aircraft configuration.");
					break;
				}
				
				string newAircraftType = creator->getValidAircraftType();
				flight->setAircraftType(newAircraftType);
				saveFlightToDatabase(flight);
				ui->printSuccess("Aircraft type updated successfully.");
				break;
			}
			case 6:  // Update Gate
			{
				string newGate = ui->getString("Enter new Gate (e.g., A12, B5): ");
				flight->setGate(newGate);
				saveFlightToDatabase(flight);
				ui->printSuccess("Gate updated successfully.");
				break;
			}
			case 7:  // Update Boarding Time
			{
				string newBoardingTime = creator->getValidDepartureDateTime();
				flight->setBoardingTime(newBoardingTime);
				saveFlightToDatabase(flight);
				ui->printSuccess("Boarding time updated successfully.");
				break;
			}
			case 8:  // Back
				ui->printWarning("Returning to previous menu.");
				return;
			default:
				ui->printError("Invalid choice.");
				break;
		}
	}
	catch (const FlightException& e)
	{
		ui->printError(string(e.what()));
	}
	
	ui->pauseScreen();
}

string FlightManager::selectFlightStatus()
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
	
	try
	{
		int choice = ui->getChoice("Enter status: ", 1, static_cast<int>(statusOptions.size()));
		return statusOptions[choice - 1];
	}
	catch (const UIException& e)
	{
		ui->printError(string(e.what()));
		return "Scheduled";
	}
}

void FlightManager::displayFlightsTable(const vector<shared_ptr<Flight>>& flights, const string& title)
{
	if (flights.empty())
	{
		ui->printWarning("No flights to display.");
		return;
	}
	
	vector<string> headers = {
		"Flight Number", "Origin", "Destination", "Departure", "Status", "Total Seats", "Available", "Price"
	};
	
	vector<vector<string>> rows;
	
	for (const auto& flight : flights)
	{
		if (flight)
		{
			try
			{
				rows.push_back({
					flight->getFlightNumber(),
					flight->getOrigin(),
					flight->getDestination(),
					flight->getDepartureDateTime(),
					flight->getStatus(),
					std::to_string(flight->getTotalSeats()),
					std::to_string(flight->getAvailableSeats()),
					ui->formatCurrency(flight->getPrice())
				});
			}
			catch (const std::exception& e)
			{
				// Skip flights with display errors
				continue;
			}
		}
	}
	
	if (rows.empty())
	{
		ui->printWarning("No valid flights to display.");
		return;
	}
	
	if (!title.empty())
	{
		ui->println("\n" + title);
		ui->printSeparator();
	}
	
	ui->displayTable(headers, rows);
}

void FlightManager::updateCrewFlightHours(const shared_ptr<Flight>& flight)
{
	double flightDuration = flight->getFlightDuration();
    vector<string> crewIds = flight->getAssignedCrew();
    
    for (const string& crewId : crewIds)
    {
        CrewManager::getInstance()->addFlightHoursForCrew(crewId, flightDuration);
        CrewManager::getInstance()->markCrewAsAvailable(crewId);
    }
}

// ==================== Query Methods ====================

shared_ptr<Flight> FlightManager::getFlight(const string& flightNumber)
{
	return loadFlightFromDatabase(flightNumber);
}

vector<shared_ptr<Flight>> FlightManager::searchFlightsByRoute(const string& origin, const string& destination, const string& departureDate)
{
	vector<shared_ptr<Flight>> results;
	
	try
	{
		json allFlightsData = db->loadAll();
		
		// Convert input to lowercase for case-insensitive comparison
		auto toLower = [](const string& str) {
			string result = str;
			std::transform(result.begin(), result.end(), result.begin(),
						[](unsigned char c) { return std::tolower(c); });
			return result;
		};
		
		string originLower = toLower(origin);
		string destLower = toLower(destination);
		
		for (const auto& [flightNum, flightData] : allFlightsData.items())
		{
			try
			{
				shared_ptr<Flight> flight = creator->createFromJson(flightNum, flightData);
				
				string flightOrigin = toLower(flight->getOrigin());
				string flightDest = toLower(flight->getDestination());
				string flightDate = flight->getDepartureDateTime().substr(0, 10);
				
				if (flightOrigin.find(originLower) != string::npos &&
					flightDest.find(destLower) != string::npos &&
					flightDate == departureDate)
				{
					results.push_back(flight);
				}
			}
			catch (const std::exception& e)
			{
				// Skip flights with errors
				continue;
			}
		}
	}
	catch (const std::exception& e)
	{
		ui->printError(string(e.what()));
	}
	
	return results;
}

vector<string> FlightManager::getAllFlightNumbers()
{
	vector<string> flightNumbers;
	
	try
	{
		json allFlightsData = db->loadAll();
		for (const auto& [flightNum, flightData] : allFlightsData.items())
		{
			flightNumbers.push_back(flightNum);
		}
	}
	catch (const std::exception& e)
	{
		ui->printError(string(e.what()));
	}
	
	return flightNumbers;
}

vector<shared_ptr<Flight>> FlightManager::getAllFlights()
{
	vector<shared_ptr<Flight>> allFlights;
	auto flightNumbers = getAllFlightNumbers();
	for (const auto& flightNumber : flightNumbers)
	{
		allFlights.push_back(getFlight(flightNumber));
	}
	return allFlights;
}

bool FlightManager::flightExists(const string& flightNumber)
{
	try
	{
		return db->entryExists(flightNumber);
	}
	catch (const std::exception& e)
	{
		return false;
	}
}

bool FlightManager::hasActiveReservations(const string& flightNumber)
{
	return ReservationManager::hasActiveReservations(flightNumber);
}

bool FlightManager::reserveSeatForFlight(const string& flightNumber, const string& seatNumber)
{
	try
	{
		shared_ptr<Flight> flight = loadFlightFromDatabase(flightNumber);
		if (!flight)
		{
			throw FlightException("Flight does not exist.");
		}
		
		// Reserve the seat (this will throw if seat is invalid or already reserved)
		bool success = flight->reserveSeat(seatNumber);
		
		if (success)
		{
			// Persist the change to database
			saveFlightToDatabase(flight);
		}
		
		return success;
	}
	catch (const std::exception& e)
	{
		ui->printError(string(e.what()));
		return false;
	}
}

bool FlightManager::releaseSeatForFlight(const string& flightNumber, const string& seatNumber)
{
	try
	{
		shared_ptr<Flight> flight = loadFlightFromDatabase(flightNumber);
		if (!flight)
		{
			throw FlightException("Flight does not exist.");
		}
		
		// Release the seat
		bool success = flight->releaseSeat(seatNumber);
		
		if (success)
		{
			// Persist the change to database
			saveFlightToDatabase(flight);
		}
		
		return success;
	}
	catch (const std::exception& e)
	{
		ui->printError(string(e.what()));
		return false;
	}
}