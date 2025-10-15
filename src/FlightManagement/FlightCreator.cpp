#include <cctype>
#include <algorithm>
#include <sstream>
#include "FlightCreator.hpp"
#include "AircraftManager.hpp"

// ==================== FlightValidator ====================

bool FlightValidator::isValidFlightNumber(const string& flightNumber)
{
	// 3-10 characters, alphanumeric only
	if (flightNumber.length() < 3 || flightNumber.length() > 10)
		return false;
	
	return std::all_of(flightNumber.begin(), flightNumber.end(),
		[](unsigned char c) { return std::isalnum(c); });
}

bool FlightValidator::isValidOrigin(const string& origin)
{
	// Non-empty, max 50 chars, alphanumeric + space/hyphen/dot
	if (origin.empty() || origin.length() > 50)
		return false;
	
	return std::all_of(origin.begin(), origin.end(),
		[](unsigned char c) { return std::isalnum(c) || c == ' ' || c == '-' || c == '.'; });
}

bool FlightValidator::isValidDestination(const string& destination)
{
	// Same rules as origin
	return isValidOrigin(destination);
}

bool FlightValidator::isValidDateTime(const string& dateTime)
{
	// Format: YYYY-MM-DD HH:MM (16 characters)
	if (dateTime.length() != 16)
		return false;
	
	// Check format: YYYY-MM-DD HH:MM
	if (dateTime[4] != '-' || dateTime[7] != '-' || 
	    dateTime[10] != ' ' || dateTime[13] != ':')
		return false;
	
	// Check YYYY
	for (int i = 0; i < 4; ++i)
		if (!std::isdigit(dateTime[i])) return false;
	
	// Check MM
	for (int i = 5; i < 7; ++i)
		if (!std::isdigit(dateTime[i])) return false;
	
	// Check DD
	for (int i = 8; i < 10; ++i)
		if (!std::isdigit(dateTime[i])) return false;
	
	// Check HH
	for (int i = 11; i < 13; ++i)
		if (!std::isdigit(dateTime[i])) return false;
	
	// Check MM
	for (int i = 14; i < 16; ++i)
		if (!std::isdigit(dateTime[i])) return false;
	
	return true;
}

bool FlightValidator::isValidPrice(double price)
{
	// Price must be positive
	return price > 0.0;
}

// ==================== FlightCreator ====================

FlightCreator::FlightCreator() 
	: ui(UserInterface::getInstance())
{
}

shared_ptr<Flight> FlightCreator::createNewFlight()
{
	ui->clearScreen();
	ui->printHeader("Create New Flight");
	
	try
	{
		string flightNumber = getValidFlightNumber();
		string origin = getValidOrigin();
		string destination = getValidDestination();
		string departureDateTime = getValidDepartureDateTime();
		string arrivalDateTime = getValidArrivalDateTime();
		string aircraftType = getValidAircraftType();
		string status = "Scheduled";
		double price = getValidPrice();
		
		return std::shared_ptr<Flight>(new Flight(flightNumber, origin, destination, departureDateTime, arrivalDateTime,
						aircraftType, status, price, "N/A", "N/A"));
	}
	catch (const UIException& e)
	{
		ui->printError(string(e.what()));
		ui->pauseScreen();
		return nullptr;
	}
}

string FlightCreator::getValidFlightNumber()
{
	while (true)
	{
		try
		{
			string input = ui->getString("Enter Flight Number (e.g., AA123): ");
			
			if (FlightValidator::isValidFlightNumber(input))
				return input;
			
			throw FlightException("Invalid flight number. Must be 3-10 characters, alphanumeric only (e.g., AA123, BA456).");
		}
		catch (const std::exception& e)
		{
			ui->printError(string(e.what()));
		}
	}
}

string FlightCreator::getValidOrigin()
{
	while (true)
	{
		try
		{
			string input = ui->getString("Enter Origin (e.g., New York): ");
			
			if (FlightValidator::isValidOrigin(input))
				return input;
			
			throw FlightException("Invalid origin. Must not be empty, less than 50 characters. Allowed: alphanumeric, space, hyphen, dot.");
		}
		catch (const std::exception& e)
		{
			ui->printError(string(e.what()));
		}
	}
}

string FlightCreator::getValidDestination()
{
	while (true)
	{
		try
		{
			string input = ui->getString("Enter Destination (e.g., Los Angeles): ");
			
			if (FlightValidator::isValidDestination(input))
				return input;
			
			throw FlightException("Invalid destination. Must not be empty, less than 50 characters. Allowed: alphanumeric, space, hyphen, dot.");
		}
		catch (const std::exception& e)
		{
			ui->printError(string(e.what()));
		}
	}
}

string FlightCreator::getValidDepartureDateTime()
{
	while (true)
	{
		try
		{
			string input = ui->getDate("Enter Departure Date and Time: ", "YYYY-MM-DD HH:MM");
			
			if (FlightValidator::isValidDateTime(input))
				return input;
			
			throw FlightException("Invalid destination. Must not be empty, less than 50 characters. Allowed: alphanumeric, space, hyphen, dot.");
		}
		catch (const std::exception& e)
		{
			ui->printError(string(e.what()));
		}
	}
}

string FlightCreator::getValidArrivalDateTime()
{
	while (true)
	{
		try
		{
			string input = ui->getDate("Enter Arrival Date and Time: ", "YYYY-MM-DD HH:MM");
			
			if (FlightValidator::isValidDateTime(input))
				return input;
			
			throw FlightException("Invalid destination. Must not be empty, less than 50 characters. Allowed: alphanumeric, space, hyphen, dot.");
		}
		catch (const std::exception& e)
		{
			ui->printError(string(e.what()));
		}
	}
}

string FlightCreator::getValidAircraftType()
{
	while (true)
	{
		try
		{
			vector<string> aircraftTypes = AircraftManager::getInstance()->getAllAircraftTypes();
			
			if (aircraftTypes.empty())
			{
				throw FlightException("No aircraft types available. Please add aircraft types first in Aircraft Management.");
			}
			
			ui->displayMenu("Available Aircraft Types", aircraftTypes);
			int typeChoice = ui->getChoice("Select aircraft type: ", 1, static_cast<int>(aircraftTypes.size()));
			
			return aircraftTypes[typeChoice - 1];
		}
		catch (const std::exception& e)
		{
			ui->printError(string(e.what()));
			ui->pauseScreen();
			throw;  // Re-throw to caller
		}
	}
}

string FlightCreator::getValidStatus()
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
		return "Scheduled";  // Default
	}
}

double FlightCreator::getValidPrice()
{
	while (true)
	{
		try
		{
			double price = ui->getDouble("Enter Price (e.g., 299.99): ");
			
			if (FlightValidator::isValidPrice(price))
				return price;
			
			throw FlightException("Invalid price. Must be a positive number (e.g., 299.99).");
		}
		catch (const std::exception& e)
		{
			ui->printError(string(e.what()));
		}
	}
}

bool FlightCreator::validateDateTimeFormat(const string& dateTime)
{
	return FlightValidator::isValidDateTime(dateTime);
}

// ==================== JSON Serialization ====================

json FlightCreator::toJson(const shared_ptr<Flight>& flight)
{
	if (!flight)
	{
		throw FlightException("An error occurred while accessing the database.");
	}
	
	json flightData;
	flightData["origin"] = flight->getOrigin();
	flightData["destination"] = flight->getDestination();
	flightData["departureDateTime"] = flight->getDepartureDateTime();
	flightData["arrivalDateTime"] = flight->getArrivalDateTime();
	flightData["aircraftType"] = flight->getAircraftType();
	flightData["status"] = flight->getStatus();
	flightData["price"] = flight->getPrice();
	flightData["gate"] = flight->getGate();
	flightData["boardingTime"] = flight->getBoardingTime();
	flightData["reservedSeats"] = flight->getReservedSeats();
	flightData["assignedCrewIds"] = flight->getAssignedCrew();
	
	return flightData;
}

shared_ptr<Flight> FlightCreator::createFromJson(const string &flightNumber, const json& data)
{
	// Extract all fields from JSON
	string origin = data.value("origin", "");
	string destination = data.value("destination", "");
	string departureDateTime = data.value("departureDateTime", "");
	string arrivalDateTime = data.value("arrivalDateTime", "");
	string aircraftType = data.value("aircraftType", "");
	string status = data.value("status", "");
	double price = data.value("price", 0.0);
	string gate = data.value("gate", "N/A");
	string boardingTime = data.value("boardingTime", "N/A");
	
	vector<string> reservedSeats;
	if (data.contains("reservedSeats") && data["reservedSeats"].is_array())
	{
		for (const auto& seat : data["reservedSeats"])
		{
			reservedSeats.push_back(seat.get<string>());
		}
	}

	vector<string> assignedCrewIds;
    if (data.contains("assignedCrewIds") && data["assignedCrewIds"].is_array())
    {
        for (const auto& crewId : data["assignedCrewIds"])
        {
            assignedCrewIds.push_back(crewId.get<string>());
        }
    }
	
	// Create and return Flight
	return std::shared_ptr<Flight>(new Flight(flightNumber, origin, destination, departureDateTime, arrivalDateTime,
					aircraftType, status, price, gate, boardingTime, reservedSeats, assignedCrewIds));
}
