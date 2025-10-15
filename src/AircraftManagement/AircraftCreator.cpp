#include <cctype>
#include <algorithm>
#include "AircraftCreator.hpp"
#include "SeatMap.hpp"

// ==================== AircraftValidator ====================

bool AircraftValidator::isValidAircraftType(const string& aircraftType)
{
	// 2-30 characters, alphanumeric only
	if (aircraftType.length() < 2 || aircraftType.length() > 30)
		return false;
	
	return std::all_of(aircraftType.begin(), aircraftType.end(),
		[](unsigned char c) { return std::isalnum(c); });
}

bool AircraftValidator::isValidManufacturer(const string& manufacturer)
{
	// Non-empty, max 50 chars, alphanumeric + space/hyphen/dot
	if (manufacturer.empty() || manufacturer.length() > 50)
		return false;
	
	return std::all_of(manufacturer.begin(), manufacturer.end(),
		[](unsigned char c) { return std::isalnum(c) || c == ' ' || c == '-' || c == '.'; });
}

bool AircraftValidator::isValidModel(const string& model)
{
	// Same rules as manufacturer
	return isValidManufacturer(model);
}

bool AircraftValidator::isValidSeatLayout(const string& seatLayout)
{
	// Use SeatMap validator
	return SeatMap::validateSeatLayout(seatLayout);
}

bool AircraftValidator::isValidSeatCount(int totalSeats)
{
	// Between 50 and 500
	return totalSeats >= 50 && totalSeats <= 500;
}

bool AircraftValidator::isValidRows(int rows)
{
	// Must be positive
	return rows > 0;
}

bool AircraftValidator::isValidFleetCount(int fleetCount)
{
	// Must be positive
	return fleetCount > 0;
}

// ==================== AircraftCreator ====================

AircraftCreator::AircraftCreator()
	: ui(UserInterface::getInstance())
{
}

shared_ptr<Aircraft> AircraftCreator::createNewAircraft()
{
	ui->clearScreen();
	ui->printHeader("Create New Aircraft Type");
	
	try
	{
		string aircraftType = getValidAircraftType();
		string manufacturer = getValidManufacturer();
		string model = getValidModel();
		int totalSeats = getValidSeatCount();
		string seatLayout = getValidSeatLayout();
		int rows = getValidRows();
		int fleetCount = getValidFleetCount();
		string status = "Available";
		
		return std::shared_ptr<Aircraft>(new Aircraft(aircraftType, manufacturer, model, totalSeats, seatLayout, rows, fleetCount, status));
	}
	catch (const std::exception& e)
	{
		ui->printError(string(e.what()));
		ui->pauseScreen();
		return nullptr;
	}
}

string AircraftCreator::getValidAircraftType()
{
	while (true)
	{
		try
		{
			string input = ui->getString("Enter Aircraft Type (e.g., Boeing737): ");
			
			if (AircraftValidator::isValidAircraftType(input))
				return input;
			
			throw AircraftException("Invalid aircraft type. Must be 2-30 characters, alphanumeric only (e.g., Boeing737, AirbusA320).");
		}
		catch (const std::exception& e)
		{
			ui->printError(string(e.what()));
		}
	}
}

string AircraftCreator::getValidManufacturer()
{
	while (true)
	{
		try
		{
			string input = ui->getString("Enter Manufacturer (e.g., Boeing): ");
			
			if (AircraftValidator::isValidManufacturer(input))
				return input;
			
			throw AircraftException("Invalid manufacturer. Must not be empty, less than 50 characters. Allowed: alphanumeric, space, hyphen, dot.");
		}
		catch (const std::exception& e)
		{
			ui->printError(string(e.what()));
		}
	}
}

string AircraftCreator::getValidModel()
{
	while (true)
	{
		try
		{
			string input = ui->getString("Enter Model (e.g., 737-800): ");
			
			if (AircraftValidator::isValidModel(input))
				return input;
			
			throw AircraftException("Invalid model. Must not be empty, less than 50 characters. Allowed: alphanumeric, space, hyphen, dot.");
		}
		catch (const std::exception& e)
		{
			ui->printError(string(e.what()));
		}
	}
}

int AircraftCreator::getValidSeatCount()
{
	while (true)
	{
		try
		{
			int input = ui->getInt("Enter Total Seats (50-500): ");
			
			if (AircraftValidator::isValidSeatCount(input))
				return input;
			
			throw AircraftException("Invalid seat count. Must be between 50 and 500.");
		}
		catch (const std::exception& e)
		{
			ui->printError(string(e.what()));
		}
	}
}

string AircraftCreator::getValidSeatLayout()
{
	while (true)
	{
		try
		{
			string input = ui->getString("Enter Seat Layout (e.g., 3-3 or 2-4-2): ");
			
			if (AircraftValidator::isValidSeatLayout(input))
				return input;
			
			throw AircraftException("Invalid seat layout. Expected format: N-N or N-N-N (e.g., 3-3 or 2-4-2).");
		}
		catch (const std::exception& e)
		{
			ui->printError(string(e.what()));
		}
	}
}

int AircraftCreator::getValidRows()
{
	while (true)
	{
		try
		{
			int input = ui->getInt("Enter Number of Rows: ");
			
			if (AircraftValidator::isValidRows(input))
				return input;
			
			throw AircraftException("Invalid number of rows. Must be positive.");
		}
		catch (const std::exception& e)
		{
			ui->printError(string(e.what()));
		}
	}
}

int AircraftCreator::getValidFleetCount()
{
	while (true)
	{
		try
		{
			int input = ui->getInt("Enter Fleet Count (number of aircraft owned): ");
			
			if (AircraftValidator::isValidFleetCount(input))
				return input;
			
			throw AircraftException("Invalid fleet count. Must be positive.");
		}
		catch (const std::exception& e)
		{
			ui->printError(string(e.what()));
		}
	}
}

// ==================== JSON Serialization ====================

json AircraftCreator::toJson(const shared_ptr<Aircraft>& aircraft)
{
	if (!aircraft)
	{
		throw AircraftException("An error occurred while accessing the database.");
	}
	
	json aircraftData;
	aircraftData["manufacturer"] = aircraft->getManufacturer();
	aircraftData["model"] = aircraft->getModel();
	aircraftData["totalSeats"] = aircraft->getTotalSeats();
	aircraftData["seatLayout"] = aircraft->getSeatLayout();
	aircraftData["rows"] = aircraft->getRows();
	aircraftData["fleetCount"] = aircraft->getFleetCount();
	aircraftData["status"] = aircraft->getStatus();
	
	return aircraftData;
}

shared_ptr<Aircraft> AircraftCreator::createFromJson(const string& aircraftType, const json& data)
{
	// Extract aircraftType from the key, manufacturer from data
	string manufacturer = data.value("manufacturer", "");
	string model = data.value("model", "");
	int totalSeats = data.value("totalSeats", 0);
	string seatLayout = data.value("seatLayout", "");
	int rows = data.value("rows", 0);
	int fleetCount = data.value("fleetCount", 0);
	string status = data.value("status", "");
	
	return std::shared_ptr<Aircraft>(new Aircraft(aircraftType, manufacturer, model, totalSeats, seatLayout, rows, fleetCount, status));
}