#include <iostream>
#include <iomanip>
#include <chrono>
#include <vector>
#include <algorithm>
#include <pgn/parser.hpp>

void test_huge_file(const std::string& filename) {
    std::cout << "=== HUGE FILE STRESS TEST ===\n";
    std::cout << "File: " << filename << "\n";
    std::cout << "=============================\n\n";
    
    auto total_start = std::chrono::high_resolution_clock::now();
    
    // Test with progress reporting
    auto stats = pgn::Parser::analyze_file(filename, 
        [](int games, const std::string& operation) {
            if (games % 10000 == 0) {  // Report every 10k games to avoid spam
                std::cout << "  " << operation << ": " << games << " games\n";
            }
        });
    
    auto total_end = std::chrono::high_resolution_clock::now();
    auto total_duration = std::chrono::duration<double>(total_end - total_start);
    
    std::cout << "\n=== FINAL RESULTS ===\n";
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
    } else {
        std::cout << "Games per second: N/A (time too short)\n";
    }
    std::cout << "Memory efficiency: " << (stats.unique_players + stats.unique_tournaments) 
              << " unique entities tracked\n";
    
    // Show top players
    if (!stats.player_stats.empty()) {
        std::cout << "\n=== TOP 10 MOST ACTIVE PLAYERS ===\n";
        // Create a vector to sort players by game count
        std::vector<std::pair<std::string, pgn::PlayerStats>> players(
            stats.player_stats.begin(), stats.player_stats.end());
        
        // Sort by total games (descending)
        std::sort(players.begin(), players.end(),
            [](const auto& a, const auto& b) {
                return a.second.total_games > b.second.total_games;
            });
        
        // Show top 10
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
    std::cout << "=== libpgn HUGE FILE TEST ===\n\n";
    
    std::string huge_file = "tests/huge_file_test.pgn";
    
    std::cout << "Testing with: " << huge_file << "\n";
    std::cout << "This may take a while for large files...\n\n";
    
    try {
        test_huge_file(huge_file);
    } catch (const std::exception& e) {
        std::cout << "ERROR: " << e.what() << "\n";
        std::cout << "This might be due to:\n";
        std::cout << "1. File not found\n";
        std::cout << "2. Out of memory\n";
        std::cout << "3. File too large for current implementation\n";
    }
    
    std::cout << "\n=== Test completed ===\n";
    return 0;
}