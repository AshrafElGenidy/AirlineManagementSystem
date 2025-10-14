#ifndef CREWMANAGER_HPP
#define CREWMANAGER_HPP

#include <string>
#include <vector>
#include <memory>
#include "Crew.hpp"
#include "Database.hpp"
#include "UserInterface.hpp"

using std::string;
using std::vector;
using std::unique_ptr;
using std::shared_ptr;

// ==================== CrewManager ====================

class CrewManager
{
private:
	static CrewManager* instance;
	
	unique_ptr<Database> db;
	UserInterface* ui;
	
	// Private constructor for singleton
	CrewManager();
	
	// Private menu methods
	void addCrewMember();
	void viewAllCrew();
	void viewCrewDetails();
	void updateCrewMember();
	void removeCrewMember();
	
	// Helper methods
	shared_ptr<Crew> loadCrewFromDatabase(const string& crewId);
	void saveCrewToDatabase(const shared_ptr<Crew>& crew);
	void deleteCrewFromDatabase(const string& crewId);
	
	// Helper for updating crew details
	void updateCrewDetails(const shared_ptr<Crew>& crew);
	
	// Helper to get crew role choice from user
	CrewRole getValidRole();
	
	// Helper to get crew status choice from user
	CrewStatus getValidStatus();
	
	// Helper to get certifications from user
	vector<string> getValidCertifications();
	
public:
	// Singleton accessor
	static CrewManager* getInstance();
	
	// Main menu
	void manageCrew();
	
	// Query methods (for other classes to use)
	shared_ptr<Crew> getCrew(const string& crewId);
	vector<shared_ptr<Crew>> getAvailableCrew(CrewRole role);
	vector<string> getAllCrewIds();
	bool crewExists(const string& crewId);
	
	// Status update methods (for FlightManager to use)
	void markCrewAsAssigned(const string& crewId);
	void markCrewAsAvailable(const string& crewId);
	void addFlightHoursForCrew(const string& crewId, double hours);
	
	// Cascade check (for deletion operations)
	static bool isCrewAssignedToFlights(const string& crewId);
	
	// Helper to display crew in a table format
	void displayCrewTable(const vector<shared_ptr<Crew>>& crew, const string& title = "");
	
	// Destructor
	~CrewManager() noexcept;
	
	// Delete copy and move constructors/assignments
	CrewManager(const CrewManager&) = delete;
	CrewManager(CrewManager&&) = delete;
	CrewManager& operator=(const CrewManager&) = delete;
	CrewManager& operator=(CrewManager&&) = delete;
};

#endif // CREWMANAGER_HPP