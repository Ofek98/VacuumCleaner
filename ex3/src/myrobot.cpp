/**
 * @file myrobot.cpp
 * @brief This file contains the implementation of a vacuum cleaner simulation program.
 */

#include <iostream>
#include <string>
#include <filesystem>
#include "House.h"
#include "Simulator.h"
#include <regex>
#include <dlfcn.h>
#include <thread>
#include "common/AlgorithmRegistrar.h"
#include <utility>
#include <atomic>

std::atomic<int> counter(-1);
std::vector<std::filesystem::path> house_file_paths;
std::vector<std::pair<std::unique_ptr<AbstractAlgorithm>, std::string>> algorithm_instances;

std::vector<std::filesystem::path> get_file_path_list_from_dir(std::filesystem::path dir_path, std::string extension) {
    std::vector<std::filesystem::path> paths;
    try {
        if (std::filesystem::exists(dir_path) && std::filesystem::is_directory(dir_path)) {
            // Iterate over files in the directory
            for (const auto& entry : std::filesystem::directory_iterator(dir_path)) {
                if (std::filesystem::is_regular_file(entry.status())) {
                    std::filesystem::path file_path = entry.path();
                    if(file_path.extension().string() == extension) {
                        paths.push_back(file_path);
                    }
                }
            }
        }
        else {
            std::cerr << "Path does not exist or is not a directory." << std::endl;
        }
    } catch (const std::filesystem::filesystem_error& e) {
        std::cerr << "Filesystem error: " << e.what() << std::endl;
    }
    return paths;
}

void run_simulations(bool summary_only, std::vector<size_t>& results) {
    size_t my_task;
    while((my_task = ++counter) < algorithm_instances.size()) {
        auto my_house_path = house_file_paths[my_task / (algorithm_instances.size() / house_file_paths.size())];
        auto my_algo = &algorithm_instances[my_task];
        std::cout << std::this_thread::get_id() << std::endl;
        
        Simulator simulator;
        if(!simulator.readHouseFile(my_house_path)) {
            std::cerr << "Error in input file: ^^^" << std::endl;
            continue;
        }
        
        simulator.setAlgorithm(std::move(my_algo->first));
        simulator.setAlgorithmName(my_algo->second);
        // TODO: change to milliseconds!!!
        auto timeout = std::chrono::seconds(simulator.getMaxSteps());

        std::thread([summary_only, &results, timeout, my_task]() {
            std::this_thread::sleep_for(timeout);
            if(results[my_task] == 0)
                run_simulations(summary_only, results);
        }).detach();
        
        results[my_task] = simulator.run(!summary_only);
    }
}

bool write_results_csv_file(const std::vector<size_t>& results) {
    std::ofstream file("summary.csv");

    if (file.is_open()) {
        // Write the header row (starting with an empty cell for the algorithm names)
        file << ",";
        for (size_t i = 0; i < house_file_paths.size(); i++) {
            file << house_file_paths[i].filename().replace_extension("") << (i+1 < house_file_paths.size() ? "," : "");
        }
        file << "\n";

        // write a row for each algorithm
        for (size_t i = 0; i < algorithm_instances.size(); i++) {
            file << algorithm_instances[i].second << ",";
            for (size_t j = 0; j < house_file_paths.size(); j++) {
                file << results[i*algorithm_instances.size() + j] << (j+1 < house_file_paths.size() ? "," : "");
            }
            file << "\n";
        }
        file.close();
    } 
    else {
        std::cerr << "Could not open the file for writing" << std::endl;
        return false;
    }
    return true;
}

/**
 * @brief The main function of the program.
 * 
 * @param argc The number of command-line arguments.
 * @param argv The array of command-line arguments.
 * @return int The exit status of the program.
 */
int main(int argc, char** argv) {
	Simulator simulator;
    std::string houseFilePath;
    std::regex house_path_pattern(R"(-house_path=([^ ]+))");
    std::regex algo_path_pattern(R"(-algo_path=([^ ]+))");
    std::regex summary_only_pattern(R"(-summary_only)");
    std::regex num_threads_pattern(R"(-num_threads=(\d+))");
    std::regex arg_patterns[4] = {house_path_pattern, algo_path_pattern, summary_only_pattern, num_threads_pattern};
    std::filesystem::path algo_path = std::filesystem::current_path();
    std::filesystem::path house_path = std::filesystem::current_path();
    bool summary_only = false;
    size_t num_threads = 10;
    std::filesystem::path* vals[2] = {&house_path, &algo_path};
    std::string args;
    
    // Check the number of arguments
    if (argc > 4) {
        std::cerr << "Too many arguments!" << std::endl;
        return EXIT_FAILURE;
    }

    for (int i = 1; i < argc; i++) {
        args += std::string(argv[i]) + " ";
    }

    // extract arguments
    for (size_t p = 0; p < sizeof(arg_patterns)/sizeof(arg_patterns[0]); p++)
    {
        std::smatch matches;
        if(std::regex_search(args, matches, arg_patterns[p])) {
            if(p==2) {
                summary_only = true;
            }
            else if(p==3) {
                try {
                    num_threads = std::stoi(matches[1]);
                } catch (std::invalid_argument& e) {
                    std::cerr << "Error: Invalid number format" << std::endl;
                } catch (std::out_of_range& e) {
                    std::cerr << "Error: Number out of range" << std::endl;
                }
            }
            else {
                try {
                    *(vals[p]) = matches[1];
                }
                catch (const std::filesystem::filesystem_error& e) {
                    std::cerr << "Filesystem error: " << e.what() << std::endl;
                    return EXIT_FAILURE;
                }
            }
        }
    }

    house_file_paths = get_file_path_list_from_dir(house_path, ".house");
    std::vector<std::filesystem::path> algo_file_paths = get_file_path_list_from_dir(algo_path, ".so");
    
    std::vector<void*> handles;
    handles.reserve(algo_file_paths.size());
    // dlopen
    for (auto path : algo_file_paths)
    {
        void* handle = dlopen(path.c_str(), RTLD_NOW | RTLD_GLOBAL);
        if(!handle) {
            std::cerr << "Failed to open library: " << dlerror() << std::endl;
            return EXIT_FAILURE;
        }
        // Clear any existing errors
        dlerror();
        handles.push_back(handle);
    }

    for (size_t i = 0; i < house_file_paths.size(); i++) {
        for(const auto& algo: AlgorithmRegistrar::getAlgorithmRegistrar()) {
            algorithm_instances.emplace_back(algo.create(), algo.name());
        }
    }
    std::vector<size_t> results(algorithm_instances.size(), 0);

    // TODO: Additional Requirements 2,3 in the pdf
    std::vector<std::thread> threads;
    threads.reserve(num_threads);

    for (size_t i = 0; i < num_threads; i++) {
        threads.emplace_back(run_simulations, summary_only, std::ref(results));
    }

    for (auto& thread : threads) {
        thread.join();
    }
    
    if(!write_results_csv_file(results)) {
        return EXIT_FAILURE;
    } 

    // dlclose
    for(auto handle : handles) {
        if(dlclose(handle)) {
            std::cerr << "Failed to close library: " << dlerror() << std::endl;
            return EXIT_FAILURE;
        }
    }

    return EXIT_SUCCESS;
}

