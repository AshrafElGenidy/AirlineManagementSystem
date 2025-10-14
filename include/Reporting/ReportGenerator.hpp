#ifndef REPORTGENERATOR_HPP
#define REPORTGENERATOR_HPP

#include <string>
#include <vector>
#include <memory>
#include "Database.hpp"
#include "UserInterface.hpp"
#include "Flight.hpp"
#include "Reservation.hpp"
#include "Crew.hpp"
#include "User.hpp"
#include "json.hpp"

using std::string;
using std::vector;
using std::unique_ptr;
using std::shared_ptr;
using nlohmann::json;

// ==================== ReportGenerator ====================

class ReportGenerator
{
private:
	UserInterface* ui;
	
	// Report generation methods
	void generateFlightPerformanceReport();
	void generateRevenueReport();
	void generateOccupancyReport();
	void generateReservationStatisticsReport();
	void generateCrewSummaryReport();
	void generateMaintenanceSummaryReport();
	void generateUsersReport();
	
	// Export functionality
	void exportToFile(const string& reportName, const string& content);
	string getCurrentDateTime() const;
	
public:
	ReportGenerator();
	
	// Main menu
	void generateReports();
	
	// Destructor
	~ReportGenerator() noexcept = default;
};

#endif // REPORTGENERATOR_HPP