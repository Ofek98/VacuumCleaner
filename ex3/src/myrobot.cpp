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
#include <mutex>

struct RunValues{
    std::vector<std::filesystem::path> house_file_paths;
    std::vector<std::pair<std::unique_ptr<AbstractAlgorithm>, std::string>> algorithm_instances;
    std::vector<int> results;
    bool summary_only = false;
};

std::atomic<int> counter(-1);

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

void run_simulations(RunValues& rv) {
    size_t my_task;
    while((my_task = ++counter) < rv.algorithm_instances.size()) {
        auto my_house_path = rv.house_file_paths[my_task / (rv.algorithm_instances.size() / rv.house_file_paths.size())];
        auto my_algo = &rv.algorithm_instances[my_task];
        std::cout << std::this_thread::get_id() << std::endl;
        
        Simulator simulator;
        if(!simulator.readHouseFile(my_house_path)) {
            std::cerr << "Error in input file: ^^^" << std::endl;
            continue;
        }
        
        simulator.setAlgorithm(std::move(my_algo->first));
        simulator.setAlgorithmName(my_algo->second);
        const size_t FILE_OPS_TIMEOUT = 3000;
        auto timeout = std::chrono::milliseconds(simulator.getMaxSteps() + FILE_OPS_TIMEOUT);
        size_t default_score = simulator.getMaxSteps() * 2 + simulator.getInitialDirt() * 300 + 2000;
        std::mutex results_task_mutex;

        std::thread([&rv, my_task, default_score, timeout, &results_task_mutex]() {
            std::this_thread::sleep_for(timeout);
            std::lock_guard<std::mutex> lock(results_task_mutex);
            if(rv.results[my_task] == -1) {
                std::cout << "68 from " << std::this_thread::get_id() << std::endl;
                rv.results[my_task] = default_score;
                results_task_mutex.unlock(); // this line is important
                run_simulations(rv);
            }
        }).detach();

        size_t run_score = simulator.run(!rv.summary_only);
        std::lock_guard<std::mutex> lock(results_task_mutex);
        if(rv.results[my_task] == -1) {
            // we usually reach here
            
            rv.results[my_task] = run_score;
            std::cout << "77 from " << std::this_thread::get_id() << std::endl;
        } 
        else {
            // we only reach here when the simulator finished after timeout and another thread replaced the current thread

            std::cout << "81 from " << std::this_thread::get_id() << std::endl;
            break;
        }
    }
}

bool write_results_csv_file(const RunValues& rv) {
    std::ofstream file("summary.csv");
    
    if (file.is_open()) {
        // Write the header row (starting with an empty cell for the algorithm names)
        file << ",";
        for (size_t i = 0; i < rv.house_file_paths.size(); i++) {
            file << rv.house_file_paths[i].filename().replace_extension("") << (i+1 < rv.house_file_paths.size() ? "," : "");
        }
        file << "\n";

        // write a row for each algorithm
        size_t algo_num = rv.algorithm_instances.size() / rv.house_file_paths.size();

        for (size_t i = 0; i < algo_num; i++) {
            file << rv.algorithm_instances[i].second << ",";
            for (size_t j = 0; j < rv.house_file_paths.size(); j++) {
                file << rv.results[j*algo_num + i] << (j+1 < rv.house_file_paths.size() ? "," : "");
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
    size_t num_threads = 10;
    std::filesystem::path* vals[2] = {&house_path, &algo_path};
    std::string args;
    RunValues rv;

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
                rv.summary_only = true;
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

    rv.house_file_paths = get_file_path_list_from_dir(house_path, ".house");
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

    for (size_t i = 0; i < rv.house_file_paths.size(); i++) {
        for(const auto& algo: AlgorithmRegistrar::getAlgorithmRegistrar()) {
            rv.algorithm_instances.emplace_back(algo.create(), algo.name());
        }
    }
    rv.results.resize(rv.algorithm_instances.size(), -1);

    std::vector<std::thread> threads;
    threads.reserve(num_threads);

    for (size_t i = 0; i < num_threads; i++) {
        threads.emplace_back(run_simulations, std::ref(rv));
    }

    for (auto& thread : threads) {
        thread.join();
    }
    
    if(!write_results_csv_file(rv)) {
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

