#include "miniSTL/stl.hpp"
#include <string>
#include <tuple>

struct DFSResult
{
    std::vector<int> visited_airports; //* 存储遍历过的机场编号序列
    void print() const
    {
        std::cout << "DFS visited airports: ";
        for (int id : visited_airports)
            std::cout << id << " ";
        std::cout << "\nCount: " << visited_airports.size() << std::endl;
    }
};

struct BFSResult
{
    std::vector<int> visited_airports; //* 存储遍历过的机场编号序列
    void print() const
    {
        std::cout << "BFS visited airports: ";
        for (int id : visited_airports)
            std::cout << id << " ";
        std::cout << "\nCount: " << visited_airports.size() << std::endl;
    }
};

struct ConnectivityResult
{
    bool connected;
    Vector<int> path;
    void print() const
    {
        if (connected)
        {
            std::cout << "Path exists: ";
            for (int id : path)
                std::cout << id << " ";
        }
        else
            std::cout << "No path.";
        std::cout << std::endl;
    }
};

struct ShortestPathResult
{
    Vector<int> path;
    int total_time;
    void print() const
    {
        if (!path.empty())
        {
            std::cout << "Shortest path: ";
            for (int i = 0; i < path.size(); i++)
            {
                std::cout << path[i];
                if (i != path.size() - 1)
                    std::cout << " -> ";
            }
            std::cout << "\nTotal time: " << total_time << " min" << std::endl;
        }
        else
            std::cout << "No path." << std::endl;
    }
};

struct MinimumCostPathResult
{
    Vector<int> path;
    int cost;
    void print() const
    {
        std::cout << "Path: ";
        for (int i = 0; i < path.size(); i++)
        {
            std::cout << path[i];
            if (i != path.size() - 1)
                std::cout << " -> ";
        }
        std::cout << "\nCost: " << cost << std::endl;
    }
};

struct AllPathsResult
{
    Vector<Vector<int>> paths;
    void print() const
    {
        std::cout << "All Paths:\n";
        for (const auto &path : paths)
        {
            for (int i = 0; i < path.size(); i++)
            {
                std::cout << path[i];
                if (i != path.size() - 1)
                    std::cout << " -> ";
            }
            std::cout << "\n";
        }
    }
};

struct Planner
{
private:
    std::vector<std::vector<FlightInfo>> flights; // 存储各机场出发的航班信息

    struct FlightInfo
    {
        int to;
        std::string time;
        FlightInfo(int t, std::string tm) : to(t), time(tm) {}
        bool operator<(const FlightInfo &other) const
        {
            if (time != other.time)
                return time < other.time;
            return to < other.to;
        }
    };

    void dfs_helper(int airport_id, const std::string &start_time,
                    std::vector<bool> &visited, DFSResult &result);
    void bfs_helper(int airport_id, const std::string &start_time,
                    std::vector<bool> &visited, BFSResult &result);
    void connectivity_helper(int current, int target,
                             std::vector<bool> &visited,
                             Vector<int> &path,
                             bool &found);
    void shortest_path_helper(int start, int end, int start_time, int end_time,
                              Vector<int> &path, int &min_time);

    void minimum_cost_helper(int start, int end, int start_time, int end_time,
                             Vector<int> &path, int &min_cost);

    void all_paths_helper(int current, int target, int end_time,
                          std::vector<bool> &visited,
                          Vector<int> &curr_path,
                          Vector<Vector<int>> &all_paths);

    // 获取从某机场出发的所有航班,按时间和目标机场排序
    std::vector<FlightInfo> get_sorted_flights(int from_airport, const std::string &start_time);
    
    int time_to_minutes(const std::string &time);
    std::string minutes_to_time(int minutes);

public:
 Planner(int n) : flights(n) {}
    
    void add_flight(int from, int to, const std::string &time) {
        flights[from].emplace_back(to, time);
    }
    DFSResult query_dfs(int airport_id, std::string start_time);
    BFSResult query_bfs(int airport_id, std::string start_time);
    ConnectivityResult query_connectivity(int airport_1, int airport_2);
    ShortestPathResult query_shortest_path(int airport_1, int airport_2, int start_time, int end_time);
    MinimumCostPathResult query_minimum_cost_path(int airport_1, int airport_2, int start_time, int end_time);
    AllPathsResult query_all_paths(int airport_1, int airport_2, int start_time, int end_time);
};

int Planner::time_to_minutes(const std::string &time) {
    return std::stoi(time.substr(0, 2)) * 60 + std::stoi(time.substr(2, 2));
}

std::string Planner::minutes_to_time(int minutes) {
    int hours = minutes / 60;
    int mins = minutes % 60;
    std::string result;
    if (hours < 10) result += "0";
    result += std::to_string(hours);
    if (mins < 10) result += "0";
    result += std::to_string(mins);
    return result;
}

std::vector<Planner::FlightInfo> Planner::get_sorted_flights(int from_airport, const std::string &start_time) {
    std::vector<FlightInfo> result;
    for (const auto &flight : flights[from_airport]) {
        if (flight.time >= start_time) {
            result.push_back(flight);
        }
    }
    std::sort(result.begin(), result.end());
    return result;
}

// DFS实现
DFSResult Planner::query_dfs(int airport_id, std::string start_time) {
    DFSResult result;
    std::vector<bool> visited(flights.size(), false);
    dfs_helper(airport_id, start_time, visited, result);
    return result;
}

void Planner::dfs_helper(int airport_id, const std::string &start_time,
                        std::vector<bool> &visited, DFSResult &result) {
    visited[airport_id] = true;
    result.visited_airports.push_back(airport_id);
    
    auto available_flights = get_sorted_flights(airport_id, start_time);
    for (const auto &flight : available_flights) {
        if (!visited[flight.to]) {
            dfs_helper(flight.to, flight.time, visited, result);
        }
    }
}

// BFS实现
BFSResult Planner::query_bfs(int airport_id, std::string start_time) {
    BFSResult result;
    std::vector<bool> visited(flights.size(), false);
    bfs_helper(airport_id, start_time, visited, result);
    return result;
}

void Planner::bfs_helper(int airport_id, const std::string &start_time,
                        std::vector<bool> &visited, BFSResult &result) {
    std::queue<std::pair<int, std::string>> q;
    q.push({airport_id, start_time});
    visited[airport_id] = true;
    result.visited_airports.push_back(airport_id);

    while (!q.empty()) {
        auto [current, curr_time] = q.front();
        q.pop();

        // 获取从当前时间点开始的所有可用航班
        auto available_flights = get_sorted_flights(current, curr_time);
        
        for (const auto &flight : available_flights) {
            if (!visited[flight.to]) {
                visited[flight.to] = true;
                result.visited_airports.push_back(flight.to);
                // 使用航班的实际起飞时间作为下一个搜索时间点
                q.push({flight.to, flight.time});
            }
        }
    }
}

// 连通性检查实现
ConnectivityResult Planner::query_connectivity(int airport_1, int airport_2) {
    ConnectivityResult result;
    std::vector<bool> visited(flights.size(), false);
    Vector<int> path;
    bool found = false;
    
    path.push_back(airport_1);
    connectivity_helper(airport_1, airport_2, visited, path, found);
    
    result.connected = found;
    if (found) {
        result.path = path;
    }
    return result;
}

void Planner::connectivity_helper(int current, int target,
                                std::vector<bool> &visited,
                                Vector<int> &path,
                                bool &found) {
    if (current == target) {
        found = true;
        return;
    }
    
    visited[current] = true;
    for (const auto &flight : flights[current]) {
        if (!visited[flight.to] && !found) {
            path.push_back(flight.to);
            connectivity_helper(flight.to, target, visited, path, found);
            if (!found) {
                path.pop_back();
            }
        }
    }
}

// 最短路径实现
ShortestPathResult Planner::query_shortest_path(int airport_1, int airport_2, 
                                              int start_time, int end_time) {
    ShortestPathResult result;
    Vector<int> current_path;
    int min_time = INT_MAX;
    
    shortest_path_helper(airport_1, airport_2, start_time, end_time, 
                        current_path, min_time);
    
    if (min_time != INT_MAX) {
        result.path = current_path;
        result.total_time = min_time;
    }
    return result;
}

void Planner::shortest_path_helper(int start, int end, int start_time, int end_time,
                                 Vector<int> &path, int &min_time) {
    std::vector<std::vector<int>> dist(flights.size(), 
                                     std::vector<int>(1440, INT_MAX));
    std::vector<std::vector<int>> prev(flights.size(), 
                                     std::vector<int>(1440, -1));
    
    dist[start][start_time] = 0;
    std::priority_queue<std::pair<int, std::pair<int, int>>> pq;
    pq.push({0, {start, start_time}});
    
    while (!pq.empty()) {
        auto [d, p] = pq.top();
        auto [u, t] = p;
        pq.pop();
        
        if (-d > dist[u][t]) continue;
        
        for (const auto &flight : flights[u]) {
            int v = flight.to;
            int new_time = time_to_minutes(flight.time);
            if (new_time > end_time) continue;
            
            int w = new_time - t;
            if (w < 0) w += 1440;
            
            if (dist[u][t] + w < dist[v][new_time]) {
                dist[v][new_time] = dist[u][t] + w;
                prev[v][new_time] = u;
                pq.push({-dist[v][new_time], {v, new_time}});
            }
        }
    }
    
    int best_time = INT_MAX;
    int best_t = -1;
    for (int t = 0; t <= end_time; t++) {
        if (dist[end][t] < best_time) {
            best_time = dist[end][t];
            best_t = t;
        }
    }
    
    if (best_time != INT_MAX) {
        path.clear();
        int curr = end;
        int curr_t = best_t;
        while (curr != -1) {
            path.push_back(curr);
            int next = prev[curr][curr_t];
            curr = next;
            if (next != -1) {
                for (const auto &flight : flights[next]) {
                    if (flight.to == curr && 
                        time_to_minutes(flight.time) == curr_t) {
                        curr_t = time_to_minutes(flight.time);
                        break;
                    }
                }
            }
        }
        std::reverse(path.begin(), path.end());
        min_time = best_time;
    }
}

// 最小成本路径实现
MinimumCostPathResult Planner::query_minimum_cost_path(int airport_1, int airport_2,
                                                     int start_time, int end_time) {
    MinimumCostPathResult result;
    Vector<int> current_path;
    int min_cost = INT_MAX;
    
    minimum_cost_helper(airport_1, airport_2, start_time, end_time,
                       current_path, min_cost);
                       
    if (min_cost != INT_MAX) {
        result.path = current_path;
        result.cost = min_cost;
    }
    return result;
}

void Planner::minimum_cost_helper(int start, int end, int start_time, int end_time,
                                Vector<int> &path, int &min_cost) {
    std::vector<std::vector<int>> dp(flights.size(),
                                   std::vector<int>(1440, INT_MAX));
    std::vector<std::vector<int>> prev(flights.size(),
                                     std::vector<int>(1440, -1));
                                     
    dp[start][start_time] = 0;
    
    for (int t = start_time; t <= end_time; t++) {
        for (int u = 0; u < flights.size(); u++) {
            if (dp[u][t] == INT_MAX) continue;
            
            for (const auto &flight : flights[u]) {
                int v = flight.to;
                int new_time = time_to_minutes(flight.time);
                if (new_time > end_time) continue;
                
                int cost = 1; // 可以根据实际情况修改成本计算方式
                
                if (dp[u][t] + cost < dp[v][new_time]) {
                    dp[v][new_time] = dp[u][t] + cost;
                    prev[v][new_time] = u;
                }
            }
        }
    }
    
    int best_cost = INT_MAX;
    int best_t = -1;
    for (int t = 0; t <= end_time; t++) {
        if (dp[end][t] < best_cost) {
            best_cost = dp[end][t];
            best_t = t;
        }
    }
    
    if (best_cost != INT_MAX) {
        path.clear();
        int curr = end;
        int curr_t = best_t;
        while (curr != -1) {
            path.push_back(curr);
            curr = prev[curr][curr_t];
        }
        std::reverse(path.begin(), path.end());
        min_cost = best_cost;
    }
}

// 所有路径查找实现
AllPathsResult Planner::query_all_paths(int airport_1, int airport_2,
                                      int start_time, int end_time) {
    AllPathsResult result;
    std::vector<bool> visited(flights.size(), false);
    Vector<int> current_path;
    current_path.push_back(airport_1);
    
    all_paths_helper(airport_1, airport_2, end_time, visited,
                    current_path, result.paths);
                    
    return result;
}

void Planner::all_paths_helper(int current, int target, int end_time,
                             std::vector<bool> &visited,
                             Vector<int> &curr_path,
                             Vector<Vector<int>> &all_paths) {
    if (current == target) {
        all_paths.push_back(curr_path);
        return;
    }
    
    visited[current] = true;
    for (const auto &flight : flights[current]) {
        if (!visited[flight.to] && 
            time_to_minutes(flight.time) <= end_time) {
            curr_path.push_back(flight.to);
            all_paths_helper(flight.to, target, end_time,
                           visited, curr_path, all_paths);
            curr_path.pop_back();
        }
    }
    visited[current] = false;
}