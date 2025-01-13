#include <miniSTL/stl.hpp>

#include <string>
#include <tuple>
#include <ctime>
#include <iomanip>
#include <iostream>
#include <fstream>
#include <sstream>
#include <algorithm>
#include <queue>
#include <map>

struct BFS
{
    Vector<int> route;

    void print() const
    {
        std::cout << "BFS Result: ";
        for (const auto &id : route)
            std::cout << id << " ";
        std::cout << std::endl;
        std::cout << "Count: " << route.size() << std::endl;
    }
};

struct DFS
{
    Vector<int> route;

    void print() const
    {
        std::cout << "DFS Result: ";
        for (const auto &id : route)
            std::cout << id << " ";
        std::cout << std::endl;
        std::cout << "Count: " << route.size() << std::endl;
    }
};

struct Link
{
    Vector<int> chain;

    void print() const
    {
        if (chain.empty())
            std::cout << "404_NotFound" << std::endl;
        else
        {
            std::cout << chain[0];
            for (int i = 1; i < chain.size(); i++)
                std::cout << " -> " << chain[i];
            std::cout << std::endl;
        }
    }
};

struct LeastTime
{
    Vector<int> route;
    int total_duration;

    void print() const
    {
        if (!route.empty())
        {
            std::cout << "Shortest path: ";
            for (int i = 0; i < route.size(); i++)
            {
                std::cout << route[i];
                if (i != route.size() - 1)
                    std::cout << " -> ";
            }
            std::cout << "\nTotal time: " << total_duration / 60 << " min" << std::endl;
        }
        else
            std::cout << "404_NotFound" << std::endl;
    }
};

struct LeastCost
{
    Vector<int> route;
    int total_price;

    void print() const
    {
        std::cout << "Path: ";
        for (int i = 0; i < route.size(); i++)
        {
            std::cout << route[i];
            if (i != route.size() - 1)
                std::cout << " -> ";
        }
        std::cout << "\nCost: " << total_price << std::endl;
    }
};

struct AllPaths
{
    Vector<Vector<int>> every_route;

    void print() const
    {
        std::cout << "All Paths: \n";
        for (const auto &path : every_route)
        {
            if (!path.empty())
                std::cout << path[0];
            for (int i = 1; i < path.size(); i++)
                std::cout << " -> " << path[i];
            std::cout << "\n";
        }
    }
};

struct Trip
{
    int trip_id;
    int target;
    std::tm start_time;
    std::tm end_time;
    int price;
};

struct Station
{
    Vector<Trip> journeys;
};

class Planner
{
public:
    Vector<Station> stations;

    Planner()
    {
        stations.resize(80);
    }

    void sortFlights()
    {
        for (auto &st : stations)
        {
            std::sort(st.journeys.begin(), st.journeys.end(), [](const Trip &a, const Trip &b)
                      {
                std::time_t a_start = std::mktime(const_cast<std::tm*>(&a.start_time));
                std::time_t b_start = std::mktime(const_cast<std::tm*>(&b.start_time));
                if (a_start != b_start) 
                    return a_start < b_start;
                 else 
                    return a.target < b.target; });
        }
    }

    void addFlight(int departure_airport, const Trip &flight)
    {
        stations[departure_airport].journeys.push_back(flight);
    }

    DFS query_dfs(int ID, std::string start_time)
    {
        Vector<int> visited(stations.size(), false);
        Vector<int> route;
        std::tm start_tm = string_to_tm(start_time);

        dfs(ID, start_tm, visited, route);

        DFS result{route};
        result.print();
        return result;
    }

    BFS query_bfs(int ID, std::string start_time)
    {
        Vector<int> visited(stations.size(), false);
        Vector<int> route;
        std::tm start_tm = string_to_tm(start_time);

        bfs(ID, start_tm, visited, route);

        BFS result{route};
        result.print();
        return result;
    }

    Link query_connectivity(int airport_1, int airport_2)
    {
        Link outcome;
        Vector<int> temp_chain;
        for (const auto &fl : stations[airport_1].journeys)
        {
            if (fl.target == airport_2)
            {
                temp_chain.push_back(fl.trip_id);
                outcome.chain = temp_chain;
                outcome.print();
                return outcome;
            }
        }

        for (const auto &fl : stations[airport_1].journeys)
        {
            temp_chain.push_back(fl.trip_id);
            for (const auto &transfer : stations[fl.target].journeys)
            {
                temp_chain.push_back(transfer.trip_id);
                if (transfer.target == airport_2)
                {
                    outcome.chain = temp_chain;
                    outcome.print();
                    return outcome;
                }
                temp_chain.pop_back();
            }
            temp_chain.pop_back();
        }
        std::cout << "No path." << std::endl;
        return outcome;
    }

    LeastTime query_shortest_path(int airport_1, int airport_2, std::string start_time, std::string end_time)
    {
        std::tm start_tm = string_to_tm(start_time);
        std::time_t start_time_t = std::mktime(&start_tm);

        std::tm end_tm = string_to_tm(end_time);
        std::time_t end_time_t = std::mktime(&end_tm);

        struct Node
        {
            int trip_id;
            int from;
            std::time_t arrive_time_t;
        };

        Vector<int> dist(stations.size(), INT_MAX);
        Vector<Node> prev(stations.size(), {-1, -1, 0});
        Vector<bool> visited(stations.size(), false);

        dist[airport_1] = 0;

        for (int i = 0; i < stations.size(); i++)
        {
            int u = -1;
            for (int j = 0; j < stations.size(); j++)
                if (!visited[j] && (u == -1 || dist[j] < dist[u]))
                    u = j;

            if (dist[u] == INT_MAX)
                break;

            visited[u] = true;

            for (const auto &trip : stations[u].journeys)
            {
                std::time_t dep_t = std::mktime(const_cast<std::tm *>(&trip.start_time));
                std::time_t arr_t = std::mktime(const_cast<std::tm *>(&trip.end_time));

                if (dep_t >= start_time_t && arr_t <= end_time_t && dep_t >= prev[u].arrive_time_t)
                {
                    int v = trip.target;
                    int flight_time = difftime(arr_t, dep_t);

                    int wait_time = 0;
                    if (prev[u].arrive_time_t > 0)
                        wait_time = difftime(dep_t, prev[u].arrive_time_t);

                    int total_cost = dist[u] + flight_time + wait_time;
                    if (total_cost < dist[v])
                    {
                        dist[v] = total_cost;
                        prev[v] = {trip.trip_id, u, arr_t};
                    }
                }
            }
        }

        if (dist[airport_2] == INT_MAX)
        {
            std::cout << "No path from airport " << airport_1 << " to airport " << airport_2 << " within the given time range." << std::endl;
            return LeastTime{};
        }
        else
        {
            Vector<int> route;
            int total_time = dist[airport_2];
            for (Node v = prev[airport_2]; v.from != -1; v = prev[v.from])
                route.push_back(v.trip_id);
            std::reverse(route.begin(), route.end());
            LeastTime outcome{route, total_time};
            outcome.print();
            return outcome;
        }
    }

    LeastCost query_minimum_cost_path(int airport_1, int airport_2, std::string start_time, std::string end_time)
    {
        if (airport_1 < 0 || airport_1 >= stations.size() || airport_2 < 0 || airport_2 >= stations.size())
        {
            std::cout << "Invalid airport ID." << std::endl;
            return LeastCost{};
        }
        std::tm start_tm = string_to_tm(start_time);
        std::time_t start_time_t = std::mktime(&start_tm);

        std::tm end_tm = string_to_tm(end_time);
        std::time_t end_time_t = std::mktime(&end_tm);

        struct FlightState
        {
            int station_id;
            std::time_t arrive_time;
            int accum_price;
            Vector<int> route;

            bool operator>(const FlightState &other) const
            {
                return accum_price > other.accum_price;
            }
        };

        std::priority_queue<FlightState, Vector<FlightState>, std::greater<>> pq;
        std::map<std::pair<int, std::time_t>, int> min_price;

        pq.push({airport_1, start_time_t, 0, {}});
        min_price[{airport_1, start_time_t}] = 0;

        while (!pq.empty())
        {
            FlightState current = pq.top();
            pq.pop();

            int curr_station = current.station_id;
            std::time_t curr_time = current.arrive_time;
            int curr_cost = current.accum_price;
            Vector<int> curr_route = current.route;

            if (curr_station == airport_2 && curr_time <= end_time_t)
            {
                LeastCost outcome{curr_route, curr_cost};
                outcome.print();
                return outcome;
            }

            for (const auto &trip : stations[curr_station].journeys)
            {
                std::time_t dep_time = std::mktime(const_cast<std::tm *>(&trip.start_time));
                std::time_t arr_time = std::mktime(const_cast<std::tm *>(&trip.end_time));
                int nxt_station = trip.target;
                int cst = trip.price;

                if (dep_time >= curr_time && dep_time >= start_time_t && arr_time <= end_time_t)
                {
                    int new_price = curr_cost + cst;
                    auto nxt_state = std::make_pair(nxt_station, arr_time);
                    if (min_price.find(nxt_state) == min_price.end() || new_price < min_price[nxt_state])
                    {
                        min_price[nxt_state] = new_price;

                        Vector<int> extended_route = curr_route;
                        extended_route.push_back(trip.trip_id);

                        pq.push({nxt_station, arr_time, new_price, extended_route});
                    }
                }
            }
        }

        std::cout << "No path from airport " << airport_1 << " to airport " << airport_2 << " within the given time range." << std::endl;
        return LeastCost{};
    }

    AllPaths query_all_paths(int airport_1, int airport_2, std::string start_time_str, std::string end_time_str)
    {
        AllPaths result;

        tm start_tm = string_to_tm(start_time_str);
        tm end_tm = string_to_tm(end_time_str);
        int st_time = static_cast<int>(mktime(&start_tm));
        int ed_time = static_cast<int>(mktime(&end_tm));

        for (auto &t1 : stations[airport_1].journeys)
        {
            tm dep1 = t1.start_time;
            tm arr1 = t1.end_time;

            if (mktime(&dep1) >= st_time && mktime(&arr1) <= ed_time)
            {
                if (t1.target == airport_2)
                {
                    Vector<int> route = {t1.trip_id};
                    result.every_route.push_back(route);
                }
                else
                {
                    for (auto &t2 : stations[t1.target].journeys)
                    {
                        tm dep2 = t2.start_time;
                        tm arr2 = t2.end_time;

                        if (mktime(&dep2) >= mktime(&arr1) && mktime(&arr2) <= ed_time && t2.target == airport_2)
                        {
                            Vector<int> route = {t1.trip_id, t2.trip_id};
                            result.every_route.push_back(route);
                        }
                    }
                }
            }
        }
        result.print();

        return result;
    }

private:
    std::tm string_to_tm(const std::string &t_str)
    {
        std::tm val = {};
        std::istringstream ss(t_str);
        ss >> std::get_time(&val, "%m/%d/%Y %H:%M");
        return val;
    }

    void dfs(int id, const std::tm &current_t, Vector<int> &visited, Vector<int> &route)
    {
        visited[id] = true;
        route.push_back(id);

        std::time_t cur_time_t = std::mktime(const_cast<std::tm *>(&current_t));
        for (const auto &tp : stations[id].journeys)
        {
            std::time_t dep_t = std::mktime(const_cast<std::tm *>(&tp.start_time));
            if (!visited[tp.target] && dep_t >= cur_time_t)
                dfs(tp.target, tp.end_time, visited, route);
        }
    }

    void bfs(int id, const std::tm &start_t, Vector<int> &visited, Vector<int> &route)
    {
        struct BFSNode
        {
            int station_id;
            std::tm current_t;
        };

        Vector<BFSNode> q;
        visited[id] = true;
        q.push_back({id, start_t});

        while (!q.empty())
        {
            BFSNode node = q.front();
            q.erase(q.begin());
            route.push_back(node.station_id);

            std::time_t T = std::mktime(&node.current_t);
            for (const auto &tp : stations[node.station_id].journeys)
            {
                std::time_t leave_time = std::mktime(const_cast<std::tm *>(&tp.start_time));
                if (!visited[tp.target] && leave_time >= T)
                {
                    visited[tp.target] = true;
                    q.push_back({tp.target, tp.end_time});
                }
            }
        }
    }
};