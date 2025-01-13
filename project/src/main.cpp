#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <ctime>
#include <iomanip>
#include <planner.hpp>

int main() {
    Planner planner;
    std::ifstream file("D:/PROGRAMMING/_DSA/Lab2/project/data/flight-data.csv");
    std::string line;
    std::getline(file, line);  // Skip the header line

    while (std::getline(file, line)) {
        std::istringstream ss(line);
        std::string field;

        std::getline(ss, field, ',');  // Skip the flight ID
        std::getline(ss, field, ',');  // Skip the departure date
        std::getline(ss, field, ',');  // Skip the Intl/Dome field
        std::getline(ss, field, ',');  // Skip the flight NO.

        std::getline(ss, field, ',');
        int departure_airport = std::stoi(field);

        std::getline(ss, field, ',');
        int arrival_airport = std::stoi(field);

        std::getline(ss, field, ',');
        std::tm departure_time = {};
        std::istringstream dt_ss(field);
        dt_ss >> std::get_time(&departure_time, "%m/%d/%Y %H:%M");

        std::getline(ss, field, ',');
        std::tm arrival_time = {};
        std::istringstream at_ss(field);
        at_ss >> std::get_time(&arrival_time, "%m/%d/%Y %H:%M");

        std::getline(ss, field, ',');
        std::getline(ss, field, ',');

        std::getline(ss, field, ',');
        int cost = std::stoi(field);

        Flight flight = {arrival_airport, departure_time, arrival_time, cost};
        planner.airports[departure_airport].flights.push_back(flight);
    }
    planner.sortFlights();
    // 以下为每个问题的测试样例
    std::cout << "#Question 1" << std::endl;
    planner.query_dfs(48, "5/5/2017 12:20");
    planner.query_bfs(48, "5/5/2017 12:20");

    std::cout << "#Question 2" << std::endl;
    std::cout << "case 1:" << std::endl;
    planner.query_connectivity(48, 50);
    std::cout << "case 2:" << std::endl;
    planner.query_connectivity(49, 4);
    std::cout << "case 3:" << std::endl;
    planner.query_connectivity(1, 4);

    std::cout << "#Question 3" << std::endl;
    planner.query_shortest_path(48, 50, "5/5/2017 12:20", "5/5/2017 15:30");

    std::cout << "#Question 4" << std::endl;
    planner.query_minimum_cost_path(49, 4, "5/5/2017 12:20", "5/6/2017 1:50");

    std::cout << "#Question 5" << std::endl;
    AllPathsResult result = planner.query_all_paths(50, 32, "5/5/2017 12:20", "5/9/2017 15:30");
    result.print(50);
    return 0;
}