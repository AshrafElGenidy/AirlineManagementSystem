#include <algorithm>
#include <cctype>
#include "Crew.hpp"

// ==================== Crew Constructor ====================

Crew::Crew(const string& crewId, const string& name, CrewRole role, CrewStatus status, double totalFlightHours)
	: crewId(crewId), name(name), role(role), status(status), totalFlightHours(totalFlightHours)
{
}

// ==================== Getters ====================

string Crew::getCrewId() const noexcept
{
	return crewId;
}

string Crew::getName() const noexcept
{
	return name;
}

CrewRole Crew::getRole() const noexcept
{
	return role;
}

string Crew::getRoleString() const noexcept
{
	if (role == CrewRole::PILOT) 			return "PILOT";
	if (role == CrewRole::COPILOT) 			return "COPILOT";
	if (role == CrewRole::FLIGHT_ATTENDANT) return "FLIGHT_ATTENDANT";
	return "PILOT";
}

CrewStatus Crew::getStatus() const noexcept
{
	return status;
}

string Crew::getStatusString() const noexcept
{
	if (status == CrewStatus::ASSIGNED) 	return "ASSIGNED";
	if (status == CrewStatus::AVAILABLE) 	return "AVAILABLE";
	if (status == CrewStatus::ON_LEAVE) 	return "ON_LEAVE";
	return "AVAILABLE";
}

double Crew::getTotalFlightHours() const noexcept
{
	return totalFlightHours;
}

// ==================== Setters ====================

void Crew::setName(const string& name) noexcept
{
	this->name = name;
}

void Crew::setRole(CrewRole role) noexcept
{
	this->role = role;
}

void Crew::setStatus(CrewStatus status) noexcept
{
	this->status = status;
}

// ==================== Flight Hours ====================

void Crew::addFlightHours(double hours) noexcept
{
	if (hours > 0)
	{
		totalFlightHours += hours;
	}
}

// ==================== Validation and Conversion Methods ====================

bool Crew::isValidCrewId(const string& crewId)
{
	// Format: CRW followed by digits (e.g., CRW001)
	if (crewId.length() < 6 || crewId.length() > 10)
		return false;
	
	if (crewId.substr(0, 3) != "CRW")
		return false;
	
	return std::all_of(crewId.begin() + 3, crewId.end(),
		[](unsigned char c) { return std::isdigit(c); });
}

bool Crew::isValidName(const string& name)
{
	// Non-empty, max 50 chars, alphanumeric + space/hyphen
	if (name.empty() || name.length() > 50)
		return false;
	
	return std::all_of(name.begin(), name.end(),
		[](unsigned char c) { return std::isalnum(c) || c == ' ' || c == '-'; });
}

string Crew::roleToString(CrewRole role) noexcept
{
	switch (role)
	{
		case CrewRole::PILOT:
			return "Pilot";
		case CrewRole::COPILOT:
			return "Copilot";
		case CrewRole::FLIGHT_ATTENDANT:
			return "Flight Attendant";
		default:
			return "Unknown";
	}
}

CrewRole Crew::stringToRole(const string& roleStr)
{
	string role = roleStr;
	std::transform(role.begin(), role.end(), role.begin(),
		[](unsigned char c) { return std::tolower(c); });
	
	if (role == "pilot")
		return CrewRole::PILOT;
	if (role == "copilot")
		return CrewRole::COPILOT;
	if (role == "flight attendant")
		return CrewRole::FLIGHT_ATTENDANT;
	
	throw CrewException("Invalid role. Must be Pilot, Copilot, or Flight Attendant.");
}

string Crew::statusToString(CrewStatus status) noexcept
{
	switch (status)
	{
		case CrewStatus::AVAILABLE:
			return "Available";
		case CrewStatus::ASSIGNED:
			return "Assigned";
		case CrewStatus::ON_LEAVE:
			return "On Leave";
		default:
			return "Unknown";
	}
}

CrewStatus Crew::stringToStatus(const string& statusStr)
{
	string status = statusStr;
	std::transform(status.begin(), status.end(), status.begin(),
		[](unsigned char c) { return std::tolower(c); });
	
	if (status == "available")
		return CrewStatus::AVAILABLE;
	if (status == "assigned")
		return CrewStatus::ASSIGNED;
	if (status == "on leave")
		return CrewStatus::ON_LEAVE;
	
	throw CrewException("Invalid status. Must be Available, Assigned, or On Leave.");
}

// ==================== CrewException Class ====================

CrewException::CrewException(const string& message) : message(message) {}

const char* CrewException::what() const noexcept
{
	return message.c_str();
}