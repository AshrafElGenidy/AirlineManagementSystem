#ifndef MAINTENANCE_REPORT_HPP
#define MAINTENANCE_REPORT_HPP

#include <map>
#include <string>

class MaintenanceReport {
private:
    int totalMaintenanceActivities;
    double totalCost;
    std::map<std::string, double> aircraftDowntime;

public:
    void displayReport();
};

#endif // MAINTENANCE_REPORT_HPP