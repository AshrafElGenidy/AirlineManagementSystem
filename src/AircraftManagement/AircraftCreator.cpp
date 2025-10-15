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

// ==================== AircraftValidationException ====================

AircraftValidationException::AircraftValidationException(AircraftValidationErrorCode code)
	: errorCode(code)
{
}

const char* AircraftValidationException::what() const noexcept
{
	static string message;
	message = getErrorMessage();
	return message.c_str();
}

AircraftValidationErrorCode AircraftValidationException::getErrorCode() const noexcept
{
	return errorCode;
}

string AircraftValidationException::getErrorMessage() const noexcept
{
	switch (errorCode)
	{
		case AircraftValidationErrorCode::INVALID_AIRCRAFT_TYPE:
			return "Invalid aircraft type. Must be 2-30 characters, alphanumeric only (e.g., Boeing737, AirbusA320).";
		case AircraftValidationErrorCode::INVALID_MANUFACTURER:
			return "Invalid manufacturer. Must not be empty, less than 50 characters. Allowed: alphanumeric, space, hyphen, dot.";
		case AircraftValidationErrorCode::INVALID_MODEL:
			return "Invalid model. Must not be empty, less than 50 characters. Allowed: alphanumeric, space, hyphen, dot.";
		case AircraftValidationErrorCode::INVALID_SEAT_LAYOUT:
			return "Invalid seat layout. Expected format: N-N or N-N-N (e.g., 3-3 or 2-4-2).";
		case AircraftValidationErrorCode::INVALID_SEAT_COUNT:
			return "Invalid seat count. Must be between 50 and 500.";
		case AircraftValidationErrorCode::INVALID_ROWS:
			return "Invalid number of rows. Must be positive.";
		case AircraftValidationErrorCode::INVALID_FLEET_COUNT:
			return "Invalid fleet count. Must be positive.";
		default:
			return "An unknown aircraft validation error occurred.";
	}
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
	catch (const UIException& e)
	{
		ui->printError(e.what());
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
			
			throw AircraftValidationException(AircraftValidationErrorCode::INVALID_AIRCRAFT_TYPE);
		}
		catch (const AircraftValidationException& e)
		{
			ui->printError(e.what());
		}
		catch (const UIException& e)
		{
			ui->printError(e.what());
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
			
			throw AircraftValidationException(AircraftValidationErrorCode::INVALID_MANUFACTURER);
		}
		catch (const AircraftValidationException& e)
		{
			ui->printError(e.what());
		}
		catch (const UIException& e)
		{
			ui->printError(e.what());
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
			
			throw AircraftValidationException(AircraftValidationErrorCode::INVALID_MODEL);
		}
		catch (const AircraftValidationException& e)
		{
			ui->printError(e.what());
		}
		catch (const UIException& e)
		{
			ui->printError(e.what());
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
			
			throw AircraftValidationException(AircraftValidationErrorCode::INVALID_SEAT_COUNT);
		}
		catch (const AircraftValidationException& e)
		{
			ui->printError(e.what());
		}
		catch (const UIException& e)
		{
			ui->printError(e.what());
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
			
			throw AircraftValidationException(AircraftValidationErrorCode::INVALID_SEAT_LAYOUT);
		}
		catch (const AircraftValidationException& e)
		{
			ui->printError(e.what());
		}
		catch (const UIException& e)
		{
			ui->printError(e.what());
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
			
			throw AircraftValidationException(AircraftValidationErrorCode::INVALID_ROWS);
		}
		catch (const AircraftValidationException& e)
		{
			ui->printError(e.what());
		}
		catch (const UIException& e)
		{
			ui->printError(e.what());
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
			
			throw AircraftValidationException(AircraftValidationErrorCode::INVALID_FLEET_COUNT);
		}
		catch (const AircraftValidationException& e)
		{
			ui->printError(e.what());
		}
		catch (const UIException& e)
		{
			ui->printError(e.what());
		}
	}
}

// ==================== JSON Serialization ====================

json AircraftCreator::toJson(const shared_ptr<Aircraft>& aircraft)
{
	if (!aircraft)
	{
		throw AircraftException(AircraftErrorCode::DATABASE_ERROR, "Cannot serialize null aircraft.");
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