#ifndef AIRCRAFT_HPP
#define AIRCRAFT_HPP

#include <string>
#include <vector>
#include "MaintenanceLog.hpp"

enum class AircraftStatus {
    AVAILABLE,
    IN_FLIGHT,
    MAINTENANCE,
    OUT_OF_SERVICE
};

class Aircraft {
private:
    std::string aircraftId;
    std::string aircraftType;
    std::string manufacturer;
    std::string model;
    int totalSeats;
    AircraftStatus status;
    std::string lastMaintenanceDate;
    std::string nextMaintenanceDate;
    double flightHours;
    std::vector<MaintenanceLog*> maintenanceLogs;

public:
    Aircraft(const std::string& aircraftId, const std::string& type, int totalSeats);
    
    std::string getAircraftType() const;
    int getTotalSeats() const;
    bool isAvailable() const;
    void scheduleMaintenance(const std::string& date);
    void addMaintenanceLog(MaintenanceLog* log);
    bool needsMaintenance() const;
};

#endif // AIRCRAFT_HPP