#include "Aircraft.hpp"

// ==================== Aircraft Class ====================

Aircraft::Aircraft(const string& aircraftType, const string& manufacturer, const string& model,
                   int totalSeats, const string& seatLayout, int rows, int fleetCount,
                   const string& status)
	: aircraftType(aircraftType), manufacturer(manufacturer), model(model),
	  totalSeats(totalSeats), seatLayout(seatLayout), rows(rows),
	  fleetCount(fleetCount), status(status)
{}

// ==================== Getters ====================

string Aircraft::getAircraftType() const noexcept
{
	return aircraftType;
}

string Aircraft::getManufacturer() const noexcept
{
	return manufacturer;
}

string Aircraft::getModel() const noexcept
{
	return model;
}

int Aircraft::getTotalSeats() const noexcept
{
	return totalSeats;
}

string Aircraft::getSeatLayout() const noexcept
{
	return seatLayout;
}

int Aircraft::getRows() const noexcept
{
	return rows;
}

int Aircraft::getFleetCount() const noexcept
{
	return fleetCount;
}

string Aircraft::getStatus() const noexcept
{
	return status;
}

// ==================== Setters ====================

void Aircraft::setManufacturer(const string& manufacturer) noexcept
{
	this->manufacturer = manufacturer;
}

void Aircraft::setModel(const string& model) noexcept
{
	this->model = model;
}

void Aircraft::setTotalSeats(int totalSeats) noexcept
{
	this->totalSeats = totalSeats;
}

void Aircraft::setSeatLayout(const string& seatLayout) noexcept
{
	this->seatLayout = seatLayout;
}

void Aircraft::setRows(int rows) noexcept
{
	this->rows = rows;
}

void Aircraft::setFleetCount(int fleetCount) noexcept
{
	this->fleetCount = fleetCount;
}

void Aircraft::setStatus(const string& status) noexcept
{
	this->status = status;
}

// ==================== AircraftException Class ====================

AircraftException::AircraftException(const string& message) : message(message) {}

const char* AircraftException::what() const noexcept
{
	return message.c_str();
}