#include <iostream>
#include <iomanip>
#include <chrono>
#include <fstream>
#include <pgn/parser.hpp>

void performance_test(const std::string& filename) {
    std::cout << "Performance test for: " << filename << "\n";
    std::cout << "================================\n";
    
    auto start_time = std::chrono::high_resolution_clock::now();
    
    auto stats = pgn::Parser::analyze_file(filename, 
        [](int games, const std::string& operation) {
            // Silent progress - parameters are intentionally unused
            (void)games;        // Silence unused parameter warning
            (void)operation;    // Silence unused parameter warning
        });
    
    auto end_time = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration<double>(end_time - start_time);
    
    std::cout << "Results:\n";
    std::cout << "  Games processed: " << stats.total_games << "\n";
    std::cout << "  Time taken: " << std::fixed << std::setprecision(3) 
              << duration.count() << " seconds\n";
    if (duration.count() > 0) {
        std::cout << "  Games per second: " << std::fixed << std::setprecision(1)
                  << (stats.total_games / duration.count()) << "\n";
    } else {
        std::cout << "  Games per second: N/A (time too short)\n";
    }
    std::cout << "  Memory efficiency: " << stats.unique_players 
              << " unique players found\n";
    std::cout << "  Parsing speed: " << std::fixed << std::setprecision(3)
              << stats.parsing_time_seconds << " seconds (internal)\n";
}

int main() {
    std::cout << "=== libpgn Performance Tests ===\n\n";
    
    // Test with different files
    performance_test("tests/test_comprehensive.pgn");
    
    // Try with the original test file if it exists
    std::ifstream test_file("tests/test.pgn");
    if (test_file.good()) {
        test_file.close();
        std::cout << "\n";
        performance_test("tests/test.pgn");
    }
    
    std::cout << "\n=== Performance test completed ===\n";
    return 0;
}