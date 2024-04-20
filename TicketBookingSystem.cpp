#include <iostream>
#include <queue>
#include <vector>
#include <map>
#include <ctime>
#include <string>
#include <sstream>
#include <iomanip>
#include <chrono>


using namespace std;


string getCurrentDate() {
    // Get current date
    time_t now = time(0);
    tm *ltm = localtime(&now);
    stringstream currentDate;
    currentDate << setw(2) << setfill('0') << ltm->tm_mday << "-" << setw(2) << setfill('0') << 1 + ltm->tm_mon << "-" << 1900 + ltm->tm_year;
    return currentDate.str();
}


// Passenger structure to hold passenger details
struct Passenger {
    int age;
    string name;
    string className;
    char berth; // 'L' for lower berth, 'U' for upper berth
    int seatNumber; // Seat number
    int id; // Unique identifier for each passenger
    Passenger(int _age, string _name, string _className, char _berth, int _seatNumber, int _id) : age(_age), name(_name), className(_className), berth(_berth), seatNumber(_seatNumber), id(_id) {}
};


// Comparator for priority queues based on passenger age and class
struct AgeClassComparator {
    bool operator()(const Passenger& p1, const Passenger& p2) {
        // Higher age gets higher priority for lower berth within the same class
        if (p1.className == p2.className) {
            if (p1.berth == 'L' && p2.berth != 'L') return true;
            if (p1.berth != 'L' && p2.berth == 'L') return false;
            if (p1.berth == 'L' && p2.berth == 'L') return p1.age > p2.age; // Higher age gets higher priority for lower berth
        }
        // Within the same class, lower age and lower id gets higher priority
        return p1.className < p2.className || (p1.className == p2.className && (p1.age < p2.age || (p1.age == p2.age && p1.id > p2.id)));
    }
};


// Function to check if the booking date and time are valid
bool isValidBooking(const string& selectedDate, const string& selectedTime) {
    string currentDate = getCurrentDate();
    if (selectedDate < currentDate) {
        cout << "Invalid date! Booking date cannot be in the past." << endl;
        return false;
    } else if (selectedDate == currentDate) {
        // Check if the selected time is in the future
        auto now = chrono::system_clock::now();
        time_t now_time = chrono::system_clock::to_time_t(now);
        tm *ltm = localtime(&now_time);
        stringstream currentTime;
        currentTime << setw(2) << setfill('0') << ltm->tm_hour << ":" << setw(2) << setfill('0') << ltm->tm_min;
        if (selectedTime < currentTime.str()) {
            cout << "Invalid time! Booking time cannot be in the past." << endl;
            return false;
        }
    }
    return true;
}


// Function to allocate berths to passengers
void allocateBerths(int n, map<string, priority_queue<Passenger, vector<Passenger>, AgeClassComparator>>& classPassengers,
                    map<string, int>& availableLowerBerths, map<string, int>& availableUpperBerths) {
    // Map to track allocated seat numbers for each class and berth type
    map<string, int> lowerBerthSeatNumbers;
    map<string, int> upperBerthSeatNumbers;


    // Loop through each class
    for (auto& cp : classPassengers) {
        const string& className = cp.first;
        priority_queue<Passenger, vector<Passenger>, AgeClassComparator>& pq = cp.second;


        // Initialize seat numbers for lower and upper berths
        lowerBerthSeatNumbers[className] = 1;
        upperBerthSeatNumbers[className] = 1;


        // Loop through passengers in the priority queue
        while (!pq.empty() && (availableLowerBerths[className] > 0 || availableUpperBerths[className] > 0)) {
            Passenger passenger = pq.top();
            pq.pop();


            // Allocate lower berth to the passenger if available and priority condition is met
            if (availableLowerBerths[className] > 0) {
                passenger.berth = 'L';
                passenger.seatNumber = lowerBerthSeatNumbers[className]++;
                availableLowerBerths[className]--;
                cout << "Passenger " << passenger.name << " with age " << passenger.age << " and class " << className << " gets a Lower Berth with seat number " << passenger.seatNumber << "." << endl;
            }
            // Allocate upper berth to the passenger if available and priority condition is not met for lower berth
            else if (availableUpperBerths[className] > 0) {
                passenger.berth = 'U';
                passenger.seatNumber = upperBerthSeatNumbers[className]++;
                availableUpperBerths[className]--;
                cout << "Passenger " << passenger.name << " with age " << passenger.age << " and class " << className << " gets an Upper Berth with seat number " << passenger.seatNumber << "." << endl;
            }
            // Output if no berths are available
            else {
                cout << "No berth available for passenger " << passenger.name << " with age " << passenger.age << " and class " << className << endl;
            }
        }
    }
}


int main() {
    // Seed random number generator for demo purposes
    srand(time(0));


    int n; // Number of passengers
    cout << "Enter the number of passengers: ";
    cin >> n;
    string selectedTime;
    string selectedDate;


    // Take the selected date input
    cout << "Enter the date at which you wish to travel (DD-MM-YYYY): ";
    cin >> selectedDate;


    // Validate the selected date against the current date
    string currentDate = getCurrentDate();
    if (selectedDate < currentDate) {
        cout << "Invalid date! Booking date cannot be in the past." << endl;
        return 1; // Exit the program if the date is invalid
    }


    // Priority queues for each class
    map<string, priority_queue<Passenger, vector<Passenger>, AgeClassComparator>> classPassengers;
    map<string, int> availableLowerBerths; // Available lower berths for each class
    map<string, int> availableUpperBerths; // Available upper berths for each class


    // Take details of each passenger
    for (int i = 0; i < n; ++i) {
        int age;
        string Name;
        string className;
        cout << "Enter age for passenger " << i + 1 << ": ";
        cin >> age;
        cout << "Enter passenger name " << i + 1 << ": ";
        cin >> Name;


        int timeChoice;
        do {
            cout << "Select the time:\n";
            cout << "1. 07:00\n";
            cout << "2. 11:00\n";
            cout << "3. 17:00\n";
            cout << "4. 22:00\n";
            cout << "Enter your choice: ";
            cin >> timeChoice;
            if (timeChoice < 1 || timeChoice > 4)
                cout << "Invalid choice. Please try again." << endl;
        } while (timeChoice < 1 || timeChoice > 4);


        switch (timeChoice) {
            case 1:
                selectedTime = "07:00";
                if (!isValidBooking(selectedDate, selectedTime)) {
                    continue;
                }
                break;
            case 2:
                selectedTime = "11:00";
                if (!isValidBooking(selectedDate, selectedTime)) {
                    continue;
                }
                break;
            case 3:
                selectedTime = "17:00";
                if (!isValidBooking(selectedDate, selectedTime)) {
                    continue;
                }
                break;
            case 4:
                selectedTime = "22:00";
                if (!isValidBooking(selectedDate, selectedTime)) {
                    continue;
                }
                break;
            default:
                cout << "Invalid choice" << endl;
                return 1;
        }
        int choice;
        do {
            cout << "Select the class:\n";
            cout << "1. Sleeper Class\n";
            cout << "2. Third Tier AC (3A)\n";
            cout << "3. Second Tier AC (2A)\n";
            cout << "4. First Tier AC (1A)\n";
            cout << "Enter your choice: ";
            cin >> choice;
            if (choice < 1 || choice > 4)
                cout << "Invalid choice. Please try again." << endl;
        } while (choice < 1 || choice > 4);
        // Convert class choice to class name
        switch (choice) {
            case 1:
                className = "Sleeper";
                break;
            case 2:
                className = "Third Tier AC (3A)";
                break;
            case 3:
                className = "Second Tier AC (2A)";
                break;
            case 4:
                className = "First Tier AC (1A)";
                break;
        }
        // Add passenger to respective priority queue based on class name
        classPassengers[className].push(Passenger(age, Name, className, ' ', 0, i + 1)); // Use passenger index as unique id
    }
    // Initialize available berths for each class
    for (const auto& cp : classPassengers) {
        const string& className = cp.first;
        availableLowerBerths[className] = 5; // 5 lower berths or available passengers, whichever is smaller
        availableUpperBerths[className] = 5; // Remaining seats are upper berths
    }
    // Allocate berths to passengers
    allocateBerths(n, classPassengers, availableLowerBerths, availableUpperBerths);
    return 0;
}


