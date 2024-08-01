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
#include "AlgorithmRegistrar.h"
#include <atomic>

std::atomic<int> counter(-1);
std::vector<std::filesystem::path> house_file_paths;
std::vector<AlgorithmRegistrar::AlgorithmFactoryPair> algorithms;

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
        } else {
            std::cerr << "Path does not exist or is not a directory." << std::endl;
        }
    } catch (const std::filesystem::filesystem_error& e) {
        std::cerr << "Filesystem error: " << e.what() << std::endl;
    }
    return paths;
}


void run() {
    size_t my_task;
    while((my_task = counter++) < algorithms.size() * house_file_paths.size()) {
        auto my_house_path = house_file_paths[my_task / house_file_paths.size()];
        auto my_algo_factory = algorithms[my_task % house_file_paths.size()];
        
        Simulator simulator;
        if(!simulator.readHouseFile(my_house_path)) {
            std::cerr << "Error in input file: ^^^" << std::endl;
            continue;
        }
        
        simulator.setAlgorithmName(my_algo_factory.name());
        simulator.setAlgorithm(std::move(my_algo_factory.create()));
        simulator.run();
    }
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
    std::regex house_path_pattern(R"(^-house_path=([^ ]+))");
    std::regex algo_path_pattern(R"(^-algo_path=([^ ]+))");
    std::regex summary_only_pattern(R"(^-summary_only)");
    std::regex arg_patterns[3] = {house_path_pattern, algo_path_pattern, summary_only_pattern};
    std::filesystem::path algo_path = std::filesystem::current_path();
    std::filesystem::path house_path = std::filesystem::current_path();
    bool summary_only = false;
    std::filesystem::path* vals[2] = {&house_path, &algo_path};
    std::string args;
    
    // Check the number of arguments
    if (argc > 4) {
        std::cerr << "Too many arguments!" << std::endl;
        return EXIT_FAILURE;
    }

    for (size_t i = 1; i < argc; i++) {
        args += argv[i];
    }

    // extract arguments
    for (size_t p = 0; p < sizeof(arg_patterns); p++)
    {
        std::smatch matches;
        if(std::regex_match(args, matches, arg_patterns[p])) {
            if(matches.size() > 1) {
                std::cerr << "Same argument specified more than once" << std::endl;
                return EXIT_FAILURE;
            }

            if(p==2) {
                summary_only = true;
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
    
    std::vector<void*> handles(algo_file_paths.size());
    // dlopen
    for (auto path : algo_file_paths)
    {
        void* handle = dlopen(path.c_str(), RTLD_LAZY);
        if(!handle) {
            std::cerr << "Failed to open library: " << dlerror() << std::endl;
            return EXIT_FAILURE;
        }
        // Clear any existing errors
        dlerror();
        handles.push_back(handle);
    }

    algorithms = AlgorithmRegistrar::getAlgorithmRegistrar().getAlgorithmFactories();

    // just one thread for now;
    std::thread t1(run);

    t1.join();

    // dlclose
    for(auto handle : handles) {
        dlclose(handle);
    }
}

