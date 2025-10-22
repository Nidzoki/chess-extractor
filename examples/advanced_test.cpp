#include <iostream>
#include <iomanip>
#include <pgn/parser.hpp>

void print_player_stats(const pgn::PlayerStats& player) {
    std::cout << "  " << player.name << ":\n";
    std::cout << "    Total games: " << player.total_games << "\n";
    std::cout << "    As White: " << player.games_as_white << "\n";
    std::cout << "    As Black: " << player.games_as_black << "\n";
    std::cout << "    Wins: " << player.wins << " (" << std::fixed << std::setprecision(1) << player.win_percentage << "%)\n";
    std::cout << "    Draws: " << player.draws << " (" << player.draw_percentage << "%)\n";
    std::cout << "    Losses: " << player.losses << "\n";
    std::cout << "    Opponents: " << player.opponents.size() << "\n";
    if (!player.opening_frequency.empty()) {
        std::cout << "    Most common opening: " << player.opening_frequency.begin()->first << "\n";
    }
}

void print_tournament_stats(const pgn::Tournament& tournament) {
    std::cout << "  " << tournament.name << ":\n";
    std::cout << "    Total games: " << tournament.total_games << "\n";
    std::cout << "    Unique players: " << tournament.unique_players << "\n";
    if (!tournament.players.empty()) {
        std::cout << "    Players: ";
        for (size_t i = 0; i < std::min(tournament.players.size(), size_t(3)); ++i) {
            std::cout << tournament.players[i];
            if (i < std::min(tournament.players.size(), size_t(3)) - 1) std::cout << ", ";
        }
        if (tournament.players.size() > 3) std::cout << "...";
        std::cout << "\n";
    }
}

int main() {
    std::cout << "=== libpgn Advanced Test ===\n\n";
    
    // Test 1: Basic analysis
    std::cout << "TEST 1: Basic file analysis\n";
    std::cout << "============================\n";
    
    auto stats = pgn::Parser::analyze_file("tests/test_comprehensive.pgn", 
        [](int games, const std::string& operation) {
            std::cout << "  " << operation << ": " << games << " games\n";
        });
    
    std::cout << "\nRESULTS:\n";
    std::cout << "  Total games: " << stats.total_games << "\n";
    std::cout << "  Unique tournaments: " << stats.unique_tournaments << "\n";
    std::cout << "  Unique players: " << stats.unique_players << "\n";
    std::cout << "  Game results:\n";
    std::cout << "    White wins: " << stats.white_wins << "\n";
    std::cout << "    Black wins: " << stats.black_wins << "\n";
    std::cout << "    Draws: " << stats.draws << "\n";
    std::cout << "    Unknown: " << stats.unknown_results << "\n";
    std::cout << "  Most active player: " << stats.most_active_player 
              << " (" << stats.max_games_by_player << " games)\n";
    std::cout << "  Largest tournament: " << stats.largest_tournament 
              << " (" << stats.max_games_in_tournament << " games)\n";
    std::cout << "  Parsing time: " << std::fixed << std::setprecision(3) 
              << stats.parsing_time_seconds << " seconds\n";
    
    // Test 2: Player statistics
    std::cout << "\nTEST 2: Player Statistics\n";
    std::cout << "==========================\n";
    if (!stats.player_stats.empty()) {
        for (const auto& [name, player] : stats.player_stats) {
            print_player_stats(player);
            std::cout << "\n";
        }
    }
    
    // Test 3: Tournament statistics
    std::cout << "TEST 3: Tournament Statistics\n";
    std::cout << "==============================\n";
    if (!stats.tournaments.empty()) {
        for (const auto& [name, tournament] : stats.tournaments) {
            print_tournament_stats(tournament);
            std::cout << "\n";
        }
    }
    
    // Test 4: Detailed parser usage
    std::cout << "TEST 4: Detailed Parser Usage\n";
    std::cout << "==============================\n";
    
    pgn::Parser parser;
    if (parser.load_file("tests/test_comprehensive.pgn")) {
        std::cout << "  Games loaded: " << parser.get_games().size() << "\n";
        std::cout << "  Player stats entries: " << parser.get_player_stats().size() << "\n";
        std::cout << "  Tournament entries: " << parser.get_tournaments().size() << "\n";
        
        // Show first game details
        if (!parser.get_games().empty()) {
            const auto& first_game = parser.get_games()[0];
            std::cout << "\n  First game details:\n";
            std::cout << "    Event: " << first_game.event << "\n";
            std::cout << "    White: " << first_game.white << " (" << first_game.white_elo << ")\n";
            std::cout << "    Black: " << first_game.black << " (" << first_game.black_elo << ")\n";
            std::cout << "    Result: " << first_game.result << "\n";
            std::cout << "    ECO: " << first_game.eco << "\n";
            std::cout << "    Opening: " << first_game.opening << "\n";
            std::cout << "    Moves: " << first_game.move_count << "\n";
        }
    }
    
    std::cout << "\n=== All tests completed ===\n";
    return 0;
}