#ifndef MAINTENANCE_LOG_HPP
#define MAINTENANCE_LOG_HPP

#include <string>
#include <vector>

enum class MaintenanceType {
    SCHEDULED,
    UNSCHEDULED,
    INSPECTION,
    REPAIR,
    OVERHAUL
};

class Aircraft;

class MaintenanceLog {
private:
    std::string logId;
    std::string aircraftId;
    std::string maintenanceDate;
    MaintenanceType maintenanceType;
    std::string description;
    std::vector<std::string> partsReplaced;
    std::string technician;
    double cost;

public:
    MaintenanceLog(Aircraft* aircraft, MaintenanceType type, const std::string& description);
    
    void displayLog();
};

#endif // MAINTENANCE_LOG_HPP