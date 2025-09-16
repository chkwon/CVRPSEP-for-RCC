#pragma once
#include <algorithm>
#include <cmath>
#include <cstdio>
#include <cstdlib>
#include <fstream>
#include <sstream>
#include <string>
#include <utility>
#include <vector>

struct instance_information {
    const int num_cities;
    const int capacity;
    const double* demand;
    int* distances;
};

inline int euc_2d(double px, double py, double qx, double qy) {
    const double dx = px - qx;
    const double dy = py - qy;
    return static_cast<int>(std::sqrt(dx * dx + dy * dy) + 0.5);
}

inline bool startswith(const std::string& str, const char* prefix) {
    return str.rfind(prefix, 0) == 0;
}

inline std::string build_instance_path(const char* filename) {
    const char* base_dir = std::getenv("CVRP_DATA_DIR");
    std::string path;
    if (base_dir && base_dir[0] != '\0') {
        path = base_dir;
    } else {
#ifdef CVRP_DEFAULT_DATA_DIR
        path = CVRP_DEFAULT_DATA_DIR;
#else
        path.clear();
#endif
    }
    if (!path.empty()) {
        const char last = path.back();
        if (last != '/' && last != '\\') {
            path.push_back('/');
        }
    }
    path.append(filename);
    path.append(".vrp");
    return path;
}

struct instance_information load_CVRP_instance(const char* filename) {
    const std::string instance_path = build_instance_path(filename);
    std::ifstream input(instance_path);
    if (!input.is_open()) {
        std::fprintf(stderr, "Error: could not open file %s\n", instance_path.c_str());
        std::exit(EXIT_FAILURE);
    }

    std::string line;
    int num_cities = 0;
    int capacity = 0;
    std::string edge_weight_type;

    auto parse_int_attribute = [](const std::string& content) {
        const auto colon = content.find(':');
        std::string value = colon == std::string::npos ? content : content.substr(colon + 1);
        std::istringstream iss(value);
        int result = 0;
        iss >> result;
        return result;
    };

    auto parse_string_attribute = [](const std::string& content) {
        const auto colon = content.find(':');
        std::string value = colon == std::string::npos ? content : content.substr(colon + 1);
        const auto first = value.find_first_not_of(" \t\r\n");
        const auto last = value.find_last_not_of(" \t\r\n");
        if (first == std::string::npos || last == std::string::npos) {
            return std::string();
        }
        return value.substr(first, last - first + 1);
    };

    int attributes_found = 0;
    while (attributes_found < 3 && std::getline(input, line)) {
        if (line.empty()) {
            continue;
        }
        if (startswith(line, "DIMENSION")) {
            num_cities = parse_int_attribute(line);
            ++attributes_found;
        } else if (startswith(line, "CAPACITY")) {
            capacity = parse_int_attribute(line);
            ++attributes_found;
        } else if (startswith(line, "EDGE_WEIGHT_TYPE")) {
            edge_weight_type = parse_string_attribute(line);
            ++attributes_found;
        }
    }

    if (num_cities <= 0 || capacity <= 0 || edge_weight_type.empty()) {
        std::fprintf(stderr, "Error: missing header information in %s\n", instance_path.c_str());
        std::exit(EXIT_FAILURE);
    }

    std::vector<int> distances(num_cities * num_cities, 0);
    std::vector<std::pair<double, double>> coordinates(num_cities, {0.0, 0.0});
    std::vector<double> demand(num_cities, 0.0);

    if (edge_weight_type == "EXPLICIT") {
        while (std::getline(input, line)) {
            if (startswith(line, "EDGE_WEIGHT_SECTION")) {
                break;
            }
        }
        for (int row = 1; row < num_cities; ++row) {
            int col = 0;
            while (col < row) {
                if (!std::getline(input, line)) {
                    std::fprintf(stderr, "Error: unexpected end of EDGE_WEIGHT_SECTION in %s\n", instance_path.c_str());
                    std::exit(EXIT_FAILURE);
                }
                if (line.empty()) {
                    continue;
                }
                std::istringstream iss(line);
                int value = 0;
                while (col < row && iss >> value) {
                    distances[row * num_cities + col] = value;
                    distances[col * num_cities + row] = value;
                    ++col;
                }
            }
        }
    }

    while (std::getline(input, line)) {
        if (startswith(line, "NODE_COORD_SECTION")) {
            break;
        }
    }

    for (int i = 0; i < num_cities;) {
        if (!std::getline(input, line)) {
            std::fprintf(stderr, "Error: unexpected end of NODE_COORD_SECTION in %s\n", instance_path.c_str());
            std::exit(EXIT_FAILURE);
        }
        if (line.empty()) {
            continue;
        }
        std::istringstream iss(line);
        int index = 0;
        double x = 0.0;
        double y = 0.0;
        if (!(iss >> index >> x >> y)) {
            continue;
        }
        coordinates[i] = {x, y};
        ++i;
    }

    while (std::getline(input, line)) {
        if (startswith(line, "DEMAND_SECTION")) {
            break;
        }
    }

    for (int i = 0; i < num_cities;) {
        if (!std::getline(input, line)) {
            std::fprintf(stderr, "Error: unexpected end of DEMAND_SECTION in %s\n", instance_path.c_str());
            std::exit(EXIT_FAILURE);
        }
        if (line.empty()) {
            continue;
        }
        std::istringstream iss(line);
        int index = 0;
        double value = 0.0;
        if (!(iss >> index >> value)) {
            continue;
        }
        demand[i] = value;
        ++i;
    }

    if (edge_weight_type == "EUC_2D") {
        for (int i = 0; i < num_cities; ++i) {
            for (int j = i + 1; j < num_cities; ++j) {
                const int distance = euc_2d(coordinates[i].first, coordinates[i].second,
                                            coordinates[j].first, coordinates[j].second);
                distances[i * num_cities + j] = distance;
                distances[j * num_cities + i] = distance;
            }
        }
    }

    auto* distance_buffer = static_cast<int*>(std::malloc(sizeof(int) * distances.size()));
    auto* demand_buffer = static_cast<double*>(std::malloc(sizeof(double) * demand.size()));
    if (!distance_buffer || !demand_buffer) {
        std::fprintf(stderr, "Error: memory allocation failure while loading %s\n", instance_path.c_str());
        std::exit(EXIT_FAILURE);
    }

    std::copy(distances.begin(), distances.end(), distance_buffer);
    std::copy(demand.begin(), demand.end(), demand_buffer);

    return instance_information{num_cities, capacity, demand_buffer, distance_buffer};
}
