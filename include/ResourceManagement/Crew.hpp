#ifndef CREW_HPP
#define CREW_HPP

#include <string>
#include <vector>
#include <exception>
#include <memory>
#include "json.hpp"

using nlohmann::json;
using std::string;
using std::vector;

// ==================== Crew Enums ====================

enum class CrewRole
{
	PILOT,
	COPILOT,
	FLIGHT_ATTENDANT
};

enum class CrewStatus
{
	AVAILABLE,
	ASSIGNED,
	ON_LEAVE
};

// ==================== Crew Class (Pure Data) ====================

class Crew
{
private:
	string crewId;
	string name;
	CrewRole role;
	CrewStatus status;
	double totalFlightHours;
	
	// Private constructor - only CrewManager can create
	Crew(const string& crewId, const string& name, CrewRole role, CrewStatus status, double totalFlightHours);
	
	friend class CrewManager;
	
public:
	// Getters
	string getCrewId() const noexcept;
	string getName() const noexcept;
	CrewRole getRole() const noexcept;
	CrewStatus getStatus() const noexcept;
	double getTotalFlightHours() const noexcept;
	
	// Setters
	void setName(const string& name) noexcept;
	void setRole(CrewRole role) noexcept;
	void setStatus(CrewStatus status) noexcept;
	void addFlightHours(double hours) noexcept;
	
	// Validation methods
	static bool isValidCrewId(const string& crewId);
	static bool isValidName(const string& name);
	static string roleToString(CrewRole role) noexcept;
	static CrewRole stringToRole(const string& roleStr);
	static string statusToString(CrewStatus status) noexcept;
	static CrewStatus stringToStatus(const string& statusStr);
	
	// Destructor
	virtual ~Crew() noexcept = default;
};

// ==================== Crew Exception Class ====================

enum class CrewErrorCode
{
	CREW_NOT_FOUND,
	CREW_EXISTS,
	INVALID_CREW_ID,
	INVALID_NAME,
	INVALID_ROLE,
	INVALID_STATUS,
	FLIGHT_HOURS_EXCEEDED,
	DATABASE_ERROR
};

class CrewException : public std::exception
{
private:
	CrewErrorCode errorCode;
	string message;
	string getErrorMessage() const noexcept;

public:
	CrewException(CrewErrorCode code);
	CrewException(CrewErrorCode code, const string& customMessage);
	const char* what() const noexcept override;
	virtual ~CrewException() noexcept = default;
	CrewErrorCode getErrorCode() const noexcept;
};

#endif // CREW_HPP