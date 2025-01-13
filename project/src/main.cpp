#include <iostream>
#include <fstream>
#include <iomanip>
#include <sstream>
#include <string>
#include <ctime>

#include <planner.hpp>

using namespace std;

int main() {
    Planner planner;
    ifstream file("D:/_DSA/Lab2/project/data/flight-data.csv");
    string line;
    getline(file, line);  

    while (getline(file, line)) {
        istringstream ss(line);
        string field;

        getline(ss, field, ','); 
        int flight_id   = stoi(field);

        getline(ss, field, ','); 
        getline(ss, field, ','); 
        getline(ss, field, ',');  

        getline(ss, field, ',');
        int departure_airport = stoi(field);

        getline(ss, field, ',');
        int arrival_airport = stoi(field);

        getline(ss, field, ',');
        tm departure_time = {};
        istringstream dt_ss(field);
        dt_ss >> get_time(&departure_time, "%m/%d/%Y %H:%M");

        getline(ss, field, ',');
        tm arrival_time = {};
        istringstream at_ss(field);
        at_ss >> get_time(&arrival_time, "%m/%d/%Y %H:%M");

        getline(ss, field, ',');
        getline(ss, field, ',');

        getline(ss, field, ',');
        int cost = stoi(field);

        Trip trip = {flight_id, arrival_airport, departure_time, arrival_time, cost};
        planner.stations[departure_airport].journeys.push_back(trip);
        
    }
    planner.sortFlights();


    cout << "***Question 1***" << endl;
    planner.query_dfs(48, "5/5/2017 12:20");
    planner.query_bfs(48, "5/5/2017 12:20");

    cout << "***Question 2***" << endl;
    cout << "Output_1:" << endl;
    planner.query_connectivity(20, 16);
    cout << "Output_2:" << endl;
    planner.query_connectivity(18, 5);
    cout << "Output_3:" << endl;
    planner.query_connectivity(1, 4);

    cout << "***Question 3***" << endl;
    planner.query_shortest_path(39, 10, "5/6/2017 0:00", "5/8/2017 0:00");

    cout << "***Question 4***" << endl;
    cout << "Output_1:" << endl;
    planner.query_minimum_cost_path(28, 74, "5/5/2017 0:00", "5/9/2017 23:59");
    cout << "Output_2:" << endl;
    planner.query_minimum_cost_path(17, 52, "5/7/2017 0:00", "5/9/2017 23:59");
    cout << "Output_3:" << endl;
    planner.query_minimum_cost_path(18, 52, "5/5/2017 0:00", "5/7/2017 0:00");
    cout << "Output_4:" << endl;
    planner.query_minimum_cost_path(80, 52, "5/5/2017 0:00", "5/7/2017 0:00");

    cout << "***Question 5***" << endl;
    planner.query_all_paths(20, 16, "5/4/2017 20:10", "5/7/2017 23:30");
 

    return 0;
}