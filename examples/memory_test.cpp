#include <iostream>
#include <iomanip>
#include <chrono>
#include <vector>
#include <algorithm>
#include <pgn/streaming_parser.hpp>

void test_memory_optimized(const std::string& filename) {
    std::cout << "=== MEMORY OPTIMIZED PARSER TEST ===\n";
    std::cout << "File: " << filename << "\n";
    std::cout << "=====================================\n\n";
    
    std::cout << "This version doesn't store individual games, only statistics.\n";
    std::cout << "Expected RAM reduction: 60-80%\n\n";
    
    auto total_start = std::chrono::high_resolution_clock::now();
    
    auto stats = pgn::StreamingParser::analyze_file(filename, 
        [](int games, const std::string& operation) {
            if (games % 10000 == 0) {
                std::cout << "  " << operation << ": " << games << " games\n";
            }
        });
    
    auto total_end = std::chrono::high_resolution_clock::now();
    auto total_duration = std::chrono::duration<double>(total_end - total_start);
    
    std::cout << "\n=== RESULTS ===\n";
    std::cout << "Total games: " << stats.total_games << "\n";
    std::cout << "Unique tournaments: " << stats.unique_tournaments << "\n";
    std::cout << "Unique players: " << stats.unique_players << "\n";
    std::cout << "Game results - White: " << stats.white_wins 
              << ", Black: " << stats.black_wins 
              << ", Draws: " << stats.draws << "\n";
    std::cout << "Most active player: " << stats.most_active_player 
              << " (" << stats.max_games_by_player << " games)\n";
    std::cout << "Largest tournament: " << stats.largest_tournament 
              << " (" << stats.max_games_in_tournament << " games)\n";
    
    std::cout << "\n=== PERFORMANCE ===\n";
    std::cout << "Total time: " << std::fixed << std::setprecision(2) 
              << total_duration.count() << " seconds\n";
    std::cout << "Library parsing time: " << stats.parsing_time_seconds << " seconds\n";
    if (total_duration.count() > 0) {
        std::cout << "Games per second: " << std::fixed << std::setprecision(1)
                  << (stats.total_games / total_duration.count()) << "\n";
    }
    
    // Show top players
    if (!stats.player_stats.empty()) {
        std::cout << "\n=== TOP 10 MOST ACTIVE PLAYERS ===\n";
        std::vector<std::pair<std::string, pgn::PlayerStats>> players(
            stats.player_stats.begin(), stats.player_stats.end());
        
        std::sort(players.begin(), players.end(),
            [](const auto& a, const auto& b) {
                return a.second.total_games > b.second.total_games;
            });
        
        for (int i = 0; i < std::min(10, (int)players.size()); ++i) {
            const auto& [name, player] = players[i];
            std::cout << std::setw(2) << (i + 1) << ". " << std::setw(25) << std::left << name
                      << ": " << std::setw(6) << player.total_games << " games, "
                      << std::setw(5) << player.wins << " wins (" 
                      << std::setprecision(1) << player.win_percentage << "%)\n";
        }
    }
}

int main() {
    std::cout << "=== libpgn MEMORY OPTIMIZED TEST ===\n\n";
    
    std::string huge_file = "tests/huge_file_test.pgn";
    
    std::cout << "Testing memory-optimized parser with: " << huge_file << "\n";
    std::cout << "This version should use significantly less RAM!\n\n";
    
    try {
        test_memory_optimized(huge_file);
    } catch (const std::exception& e) {
        std::cout << "ERROR: " << e.what() << "\n";
    }
    
    std::cout << "\n=== Test completed ===\n";
    std::cout << "Check Task Manager for RAM usage comparison!\n";
    return 0;
}