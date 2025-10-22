#include <iostream>
#include <iomanip>
#include <chrono>
#include <vector>
#include <algorithm>
#include <windows.h>
#include <psapi.h>
#include <pgn/parser.hpp>
#include <pgn/streaming_parser.hpp>

// Function to get current process memory usage in MB
size_t get_current_memory_usage() {
    PROCESS_MEMORY_COUNTERS_EX pmc;
    if (GetProcessMemoryInfo(GetCurrentProcess(), (PROCESS_MEMORY_COUNTERS*)&pmc, sizeof(pmc))) {
        return pmc.WorkingSetSize / (1024 * 1024); // Convert to MB
    }
    return 0;
}

struct MemorySample {
    int games_processed;
    size_t memory_mb;
    double time_seconds;
    std::string operation;
};

struct ParserResult {
    pgn::DatabaseStats stats;
    std::vector<MemorySample> memory_samples;
    size_t peak_memory_mb;
    double total_time_seconds;
};

// Test the original parser
ParserResult test_original_parser(const std::string& filename) {
    std::cout << "Testing ORIGINAL PARSER...\n";
    
    ParserResult result;
    std::vector<MemorySample> samples;
    auto start_time = std::chrono::high_resolution_clock::now();
    size_t peak_memory = 0;
    
    auto stats = pgn::Parser::analyze_file(filename, 
        [&samples, &peak_memory, start_time](int games, const std::string& operation) {
            if (games % 50000 == 0) {  // Sample every 50k games
                auto current_time = std::chrono::high_resolution_clock::now();
                double elapsed = std::chrono::duration<double>(current_time - start_time).count();
                size_t memory = get_current_memory_usage();
                
                samples.push_back({games, memory, elapsed, operation});
                peak_memory = std::max(peak_memory, memory);
                
                std::cout << "  [ORIGINAL] " << operation << ": " << games 
                          << " games, RAM: " << memory << " MB, Time: " 
                          << std::fixed << std::setprecision(1) << elapsed << "s\n";
            }
        });
    
    auto end_time = std::chrono::high_resolution_clock::now();
    double total_time = std::chrono::duration<double>(end_time - start_time).count();
    
    // Final memory reading
    size_t final_memory = get_current_memory_usage();
    peak_memory = std::max(peak_memory, final_memory);
    
    result.stats = stats;
    result.memory_samples = samples;
    result.peak_memory_mb = peak_memory;
    result.total_time_seconds = total_time;
    
    return result;
}

// Test the streaming parser
ParserResult test_streaming_parser(const std::string& filename) {
    std::cout << "Testing STREAMING PARSER...\n";
    
    ParserResult result;
    std::vector<MemorySample> samples;
    auto start_time = std::chrono::high_resolution_clock::now();
    size_t peak_memory = 0;
    
    auto stats = pgn::StreamingParser::analyze_file(filename, 
        [&samples, &peak_memory, start_time](int games, const std::string& operation) {
            if (games % 50000 == 0) {  // Sample every 50k games
                auto current_time = std::chrono::high_resolution_clock::now();
                double elapsed = std::chrono::duration<double>(current_time - start_time).count();
                size_t memory = get_current_memory_usage();
                
                samples.push_back({games, memory, elapsed, operation});
                peak_memory = std::max(peak_memory, memory);
                
                std::cout << "  [STREAMING] " << operation << ": " << games 
                          << " games, RAM: " << memory << " MB, Time: " 
                          << std::fixed << std::setprecision(1) << elapsed << "s\n";
            }
        });
    
    auto end_time = std::chrono::high_resolution_clock::now();
    double total_time = std::chrono::duration<double>(end_time - start_time).count();
    
    // Final memory reading
    size_t final_memory = get_current_memory_usage();
    peak_memory = std::max(peak_memory, final_memory);
    
    result.stats = stats;
    result.memory_samples = samples;
    result.peak_memory_mb = peak_memory;
    result.total_time_seconds = total_time;
    
    return result;
}

void print_comparison_report(const ParserResult& original, const ParserResult& streaming) {
    std::cout << "\n" << std::string(80, '=') << "\n";
    std::cout << "                      MEMORY USAGE COMPARISON REPORT\n";
    std::cout << std::string(80, '=') << "\n\n";
    
    // Basic stats comparison
    std::cout << "=== BASIC STATISTICS ===\n";
    std::cout << std::setw(25) << std::left << "Metric" 
              << std::setw(20) << "Original Parser" 
              << std::setw(20) << "Streaming Parser" 
              << std::setw(15) << "Difference" << "\n";
    std::cout << std::string(80, '-') << "\n";
    
    std::cout << std::setw(25) << "Total games" 
              << std::setw(20) << original.stats.total_games 
              << std::setw(20) << streaming.stats.total_games 
              << std::setw(15) << "N/A" << "\n";
    
    std::cout << std::setw(25) << "Peak memory (MB)" 
              << std::setw(20) << original.peak_memory_mb 
              << std::setw(20) << streaming.peak_memory_mb 
              << std::setw(15) << (original.peak_memory_mb - streaming.peak_memory_mb) << "\n";
    
    std::cout << std::setw(25) << "Total time (seconds)" 
              << std::setw(20) << std::fixed << std::setprecision(2) << original.total_time_seconds
              << std::setw(20) << streaming.total_time_seconds
              << std::setw(15) << (original.total_time_seconds - streaming.total_time_seconds) << "\n";
    
    if (original.total_time_seconds > 0 && streaming.total_time_seconds > 0) {
        double original_speed = original.stats.total_games / original.total_time_seconds;
        double streaming_speed = streaming.stats.total_games / streaming.total_time_seconds;
        
        std::cout << std::setw(25) << "Games per second" 
                  << std::setw(20) << std::fixed << std::setprecision(0) << original_speed
                  << std::setw(20) << streaming_speed
                  << std::setw(15) << (streaming_speed - original_speed) << "\n";
    }
    
    // Memory efficiency
    if (original.stats.total_games > 0 && streaming.stats.total_games > 0) {
        double original_mem_per_game = (original.peak_memory_mb * 1024.0 * 1024.0) / original.stats.total_games;
        double streaming_mem_per_game = (streaming.peak_memory_mb * 1024.0 * 1024.0) / streaming.stats.total_games;
        
        std::cout << std::setw(25) << "Bytes per game" 
                  << std::setw(20) << std::fixed << std::setprecision(0) << original_mem_per_game
                  << std::setw(20) << streaming_mem_per_game
                  << std::setw(15) << (streaming_mem_per_game - original_mem_per_game) << "\n";
    }
    
    // Memory reduction percentage
    double memory_reduction = 100.0 * (original.peak_memory_mb - streaming.peak_memory_mb) / original.peak_memory_mb;
    std::cout << std::setw(25) << "Memory reduction" 
              << std::setw(20) << "N/A" 
              << std::setw(20) << "N/A" 
              << std::setw(15) << std::fixed << std::setprecision(1) << memory_reduction << "%\n";
    
    // Detailed memory progression
    if (!original.memory_samples.empty() && !streaming.memory_samples.empty()) {
        std::cout << "\n=== MEMORY PROGRESSION ===\n";
        std::cout << std::setw(10) << "Games" 
                  << std::setw(15) << "Orig RAM (MB)" 
                  << std::setw(15) << "Stream RAM (MB)" 
                  << std::setw(15) << "Savings (MB)" 
                  << std::setw(15) << "Orig Time (s)" 
                  << std::setw(15) << "Stream Time (s)" << "\n";
        std::cout << std::string(85, '-') << "\n";
        
        // Compare at common game counts
        for (size_t i = 0; i < std::min(original.memory_samples.size(), streaming.memory_samples.size()); i++) {
            const auto& orig_sample = original.memory_samples[i];
            const auto& stream_sample = streaming.memory_samples[i];
            
            if (orig_sample.games_processed == stream_sample.games_processed) {
                std::cout << std::setw(10) << orig_sample.games_processed
                          << std::setw(15) << orig_sample.memory_mb
                          << std::setw(15) << stream_sample.memory_mb
                          << std::setw(15) << (orig_sample.memory_mb - stream_sample.memory_mb)
                          << std::setw(15) << std::fixed << std::setprecision(1) << orig_sample.time_seconds
                          << std::setw(15) << stream_sample.time_seconds << "\n";
            }
        }
    }
    
    // Summary
    std::cout << "\n=== SUMMARY ===\n";
    std::cout << "The streaming parser uses " << std::fixed << std::setprecision(1) << memory_reduction 
              << "% less memory than the original parser.\n";
    
    if (streaming.total_time_seconds < original.total_time_seconds) {
        double time_improvement = 100.0 * (original.total_time_seconds - streaming.total_time_seconds) / original.total_time_seconds;
        std::cout << "The streaming parser is " << std::fixed << std::setprecision(1) << time_improvement 
                  << "% faster than the original parser.\n";
    } else {
        double time_slowdown = 100.0 * (streaming.total_time_seconds - original.total_time_seconds) / original.total_time_seconds;
        std::cout << "The streaming parser is " << std::fixed << std::setprecision(1) << time_slowdown 
                  << "% slower than the original parser (memory vs speed trade-off).\n";
    }
}

int main() {
    std::cout << "=== libpgn MEMORY USAGE COMPARISON ===\n\n";
    
    std::string huge_file = "tests/huge_file_test.pgn";
    
    std::cout << "Comparing memory usage between original and streaming parsers\n";
    std::cout << "File: " << huge_file << "\n";
    std::cout << "Sampling memory every 50,000 games\n\n";
    std::cout << "This will take a few minutes...\n\n";
    
    try {
        // Test original parser first
        std::cout << "PHASE 1: Testing original parser...\n";
        auto original_result = test_original_parser(huge_file);
        
        std::cout << "\nPHASE 2: Testing streaming parser...\n";
        auto streaming_result = test_streaming_parser(huge_file);
        
        // Print comparison report
        print_comparison_report(original_result, streaming_result);
        
    } catch (const std::exception& e) {
        std::cout << "ERROR: " << e.what() << "\n";
        std::cout << "This might be due to out of memory or file access issues.\n";
    }
    
    std::cout << "\n=== Comparison completed ===\n";
    return 0;
}