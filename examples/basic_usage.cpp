#include <iostream>
#include <iomanip>
#include <pgn/parser.hpp>

int main() {
    std::cout << "=== libpgn Enhanced Test ===\n";
    
    try {
        auto stats = pgn::Parser::analyze_file("tests/test_comprehensive.pgn", 
            [](int games, const std::string& operation) {
                std::cout << "Processed " << games << " games (" << operation << ")\n";
            });
        
        std::cout << "\n=== COMPREHENSIVE RESULTS ===\n";
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
        std::cout << "Parsing time: " << std::fixed << std::setprecision(3) 
                  << stats.parsing_time_seconds << " seconds\n";
        
        if (!stats.player_stats.empty()) {
            std::cout << "\n=== TOP PLAYERS ===\n";
            int count = 0;
            for (const auto& [name, player] : stats.player_stats) {
                if (count++ >= 5) break;
                std::cout << name << ": " << player.total_games << " games, "
                          << player.wins << " wins (" << std::setprecision(1) 
                          << player.win_percentage << "%)" << std::endl;
            }
        }
        
    } catch (const std::exception& e) {
        std::cout << "Error: " << e.what() << "\n";
    }
    
    return 0;
}