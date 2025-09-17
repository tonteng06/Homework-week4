// Public Transportation Station Management System for a city's bus and train network.cpp.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <iostream>
#include <string>
#include <vector>
#include <memory>
#include <algorithm>
#include <iomanip>

using namespace std;

// Forward declarations
class Station;
class Passenger;

// ----------------------
// Schedule struct
// ----------------------
struct Schedule {
    string time; // simple representation e.g., "09:30"
    string type; // "Arrival" or "Departure"
    int vehicleId; // reference by vehicle id
    Schedule(const string& t, const string& ty, int vid) : time(t), type(ty), vehicleId(vid) {}
};

// ----------------------
// Vehicle (base class)
// ----------------------
class Vehicle {
protected:
    int id;
    string route;
    int capacity;
    int bookedCount;
    string status; // "On-time", "Delayed", ...
    int assignedStationId; // -1 if none

public:
    Vehicle(int id_, const string& route_, int capacity_)
        : id(id_), route(route_), capacity(capacity_), bookedCount(0), status("On-time"), assignedStationId(-1) {}

    virtual ~Vehicle() {}

    int getId() const { return id; }
    string getRoute() const { return route; }
    int getCapacity() const { return capacity; }
    int getBooked() const { return bookedCount; }
    string getStatus() const { return status; }
    int getAssignedStation() const { return assignedStationId; }

    void setStatus(const string& s) { status = s; }
    void assignToStation(int stationId) { assignedStationId = stationId; }
    void unassignStation() { assignedStationId = -1; }

    // Attempt to book one seat; return true if successful
    virtual bool bookSeat() {
        if (bookedCount >= capacity) {
            cout << "Vehicle " << id << " is full (capacity " << capacity << ").\n";
            return false;
        }
        bookedCount++;
        return true;
    }

    // Cancel one booking
    virtual bool cancelSeat() {
        if (bookedCount <= 0) {
            cout << "Vehicle " << id << " has no bookings to cancel.\n";
            return false;
        }
        bookedCount--;
        return true;
    }

    // Virtual method to calculate travel time (hours) given distance (km)
    // Base vehicle uses a default average speed (e.g., 40 km/h)
    virtual double calculateTravelTime(double distanceKm) const {
        double avgSpeed = 40.0; // km/h
        return distanceKm / avgSpeed;
    }

    virtual void displayInfo() const {
        cout << "Vehicle ID: " << id << ", Route: " << route
            << ", Capacity: " << capacity << ", Booked: " << bookedCount
            << ", Status: " << status
            << ", AssignedStation: " << (assignedStationId == -1 ? string("None") : to_string(assignedStationId))
            << "\n";
    }
};

// ----------------------
// ExpressBus (derived)
// ----------------------
class ExpressBus : public Vehicle {
private:
    double speedMultiplier; // e.g., 1.2 for 20% faster
    int fewerStops; // qualitative attribute

public:
    ExpressBus(int id_, const string& route_, int capacity_, double speedMult = 1.2, int fewerStops_ = 3)
        : Vehicle(id_, route_, capacity_), speedMultiplier(speedMult), fewerStops(fewerStops_) {}

    // Override travel time: base time reduced by speedMultiplier
    double calculateTravelTime(double distanceKm) const override {
        double baseSpeed = 40.0; // base km/h used by Vehicle
        double expressSpeed = baseSpeed * speedMultiplier;
        return distanceKm / expressSpeed;
    }

    void displayInfo() const override {
        cout << "ExpressBus ID: " << id << ", Route: " << route
            << ", Capacity: " << capacity << ", Booked: " << bookedCount
            << ", SpeedMult: " << speedMultiplier << ", FewerStops: " << fewerStops
            << ", Status: " << status << "\n";
    }
};

// ----------------------
// Passenger
// ----------------------
class Passenger {
private:
    int id;
    string name;
    vector<int> bookedVehicleIds;

public:
    Passenger(int id_, const string& name_) : id(id_), name(name_) {}

    int getId() const { return id; }
    string getName() const { return name; }

    void addBooking(int vehicleId) {
        bookedVehicleIds.push_back(vehicleId);
    }

    bool removeBooking(int vehicleId) {
        auto it = find(bookedVehicleIds.begin(), bookedVehicleIds.end(), vehicleId);
        if (it == bookedVehicleIds.end()) return false;
        bookedVehicleIds.erase(it);
        return true;
    }

    void displayInfo() const {
        cout << "Passenger ID: " << id << ", Name: " << name << ", Booked Vehicles: ";
        if (bookedVehicleIds.empty()) cout << "None";
        else {
            for (size_t i = 0; i < bookedVehicleIds.size(); ++i) {
                if (i) cout << ", ";
                cout << bookedVehicleIds[i];
            }
        }
        cout << "\n";
    }
};

// ----------------------
// Station
// ----------------------
class Station {
private:
    int id;
    string name;
    string location;
    string type; // "Bus" or "Train"
    vector<Schedule> schedules;
    static const size_t MAX_SCHEDULES = 10;

public:
    Station(int id_, const string& name_, const string& location_, const string& type_)
        : id(id_), name(name_), location(location_), type(type_) {}

    int getId() const { return id; }
    string getName() const { return name; }

    bool addSchedule(const Schedule& s) {
        if (schedules.size() >= MAX_SCHEDULES) {
            cout << "Station " << name << " has reached max schedules (" << MAX_SCHEDULES << "). Cannot add more.\n";
            return false;
        }
        schedules.push_back(s);
        return true;
    }

    bool removeScheduleAtTime(const string& time, int vehicleId) {
        auto it = remove_if(schedules.begin(), schedules.end(),
            [&](const Schedule& sc) { return sc.time == time && sc.vehicleId == vehicleId; });
        if (it == schedules.end() || it == schedules.begin() && schedules.size() == 0) {
            // nothing found
        }
        if (it != schedules.end()) {
            schedules.erase(it, schedules.end());
            return true;
        }
        return false;
    }

    void displaySchedules() const {
        cout << "Schedules for Station [" << id << "] " << name << " (" << type << ", " << location << "):\n";
        if (schedules.empty()) {
            cout << "  No schedules.\n";
            return;
        }
        cout << left << setw(10) << "Time" << setw(12) << "Type" << setw(10) << "VehicleID" << "\n";
        for (const auto& s : schedules) {
            cout << left << setw(10) << s.time << setw(12) << s.type << setw(10) << s.vehicleId << "\n";
        }
    }
};

// ----------------------
// Simple management 'database' using vectors
// ----------------------
int main() {
    cout << "=== Public Transportation Station Management System (Demo) ===\n\n";

    // Create some stations
    Station s1(1, "Central Bus Station", "Downtown", "Bus");
    Station s2(2, "North Train Station", "Uptown", "Train");

    // Create vehicles
    // Use pointers to base class so polymorphism works
    vector<shared_ptr<Vehicle>> vehicles;
    vehicles.push_back(make_shared<Vehicle>(101, "Route A", 40)); // regular vehicle
    vehicles.push_back(make_shared<ExpressBus>(102, "Express A", 30, 1.2, 2)); // express bus
    vehicles.push_back(make_shared<Vehicle>(201, "Train X", 200)); // train as generic vehicle

    // Create passengers
    vector<Passenger> passengers;
    passengers.emplace_back(1, "Anh");
    passengers.emplace_back(2, "Binh");

    // Assign vehicles to station
    vehicles[0]->assignToStation(s1.getId());
    vehicles[1]->assignToStation(s1.getId());
    vehicles[2]->assignToStation(s2.getId());

    // Add schedules to stations (observe MAX_SCHEDULES limit)
    s1.addSchedule(Schedule("08:00", "Departure", vehicles[0]->getId()));
    s1.addSchedule(Schedule("08:30", "Arrival", vehicles[1]->getId()));
    s1.addSchedule(Schedule("09:00", "Departure", vehicles[1]->getId()));

    s2.addSchedule(Schedule("10:00", "Arrival", vehicles[2]->getId()));

    // Display initial state
    s1.displaySchedules();
    cout << "\n";
    s2.displaySchedules();
    cout << "\n";

    for (auto& v : vehicles) {
        v->displayInfo();
    }
    cout << "\n";

    // Demonstrate booking: try to book passenger 1 into vehicle 102 (ExpressBus)
    cout << "Attempting booking passenger 1 on vehicle 102 (ExpressBus)...\n";
    int targetVehicleId = 102;
    auto findVehicle = [&](int vid)->shared_ptr<Vehicle> {
        for (auto& vv : vehicles) if (vv->getId() == vid) return vv;
        return nullptr;
        };
    auto vehicle102 = findVehicle(targetVehicleId);
    if (vehicle102) {
        if (vehicle102->bookSeat()) {
            // record booking in passenger record
            passengers[0].addBooking(vehicle102->getId());
            cout << "Booking successful.\n";
        }
        else {
            cout << "Booking failed: vehicle full or error.\n";
        }
    }
    else cout << "Vehicle not found.\n";

    // Show passenger info and vehicle booked count
    cout << "\nPassenger & Vehicle state after booking:\n";
    passengers[0].displayInfo();
    vehicle102->displayInfo();

    // Try to fill up a small vehicle quickly to test capacity blocking
    cout << "\nFilling up ExpressBus (simulate)...\n";
    // ExpressBus capacity is 30; we already have 1 booked; try booking 31 times more to exceed
    for (int i = 0; i < 31; ++i) {
        vehicle102->bookSeat();
    }
    cout << "After mass booking attempts:\n";
    vehicle102->displayInfo();

    // Demonstrate overridden travel time
    double distance = 120.0; // km
    double timeVehicle = vehicles[0]->calculateTravelTime(distance);
    double timeExpress = vehicle102->calculateTravelTime(distance);
    cout << fixed << setprecision(2);
    cout << "\nTravel time for vehicle " << vehicles[0]->getId() << " over " << distance << " km: " << timeVehicle << " hours\n";
    cout << "Travel time for express " << vehicle102->getId() << " over " << distance << " km: " << timeExpress << " hours\n";

    // Attempt cancel booking
    cout << "\nCanceling one booking on express bus for passenger 1...\n";
    if (passengers[0].removeBooking(vehicle102->getId())) {
        if (vehicle102->cancelSeat()) {
            cout << "Cancellation successful.\n";
        }
        else {
            cout << "Vehicle cancel failed.\n";
        }
    }
    else {
        cout << "Passenger had no booking for that vehicle.\n";
    }
    passengers[0].displayInfo();
    vehicle102->displayInfo();

    // Test station schedule limit (try to add many schedules)
    cout << "\nTesting station schedule limit (adding schedules to s1)...\n";
    for (int i = 0; i < 12; ++i) {
        string t = to_string(11 + i) + ":00";
        bool ok = s1.addSchedule(Schedule(t, "Arrival", vehicles[0]->getId()));
        if (!ok) cout << "Failed to add schedule at " << t << "\n";
    }
    cout << "\nFinal s1 schedules:\n";
    s1.displaySchedules();

    cout << "\n=== Demo finished ===\n";
    return 0;
}
