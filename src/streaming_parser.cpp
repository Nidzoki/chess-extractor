#include "pgn/streaming_parser.hpp"
#include <fstream>
#include <iostream>
#include <chrono>
#include <algorithm>  // Add this include for std::find
#include <unordered_set>

namespace pgn {

StreamingParser::StreamingParser() {
    stats = DatabaseStats{};
}

bool StreamingParser::load_file(const std::string& filename, ProgressCallback callback) {
    auto start_time = std::chrono::high_resolution_clock::now();
    
    std::ifstream pgn_file(filename);
    if (!pgn_file.is_open()) {
        std::cerr << "Error: Cannot open file " << filename << std::endl;
        return false;
    }
    
    // Reset stats
    stats = DatabaseStats{};
    player_stats.clear();
    tournaments.clear();
    
    std::string line;
    Game current_game;
    bool in_game = false;
    
    std::unordered_set<std::string> tournament_names;
    std::unordered_set<std::string> player_names;
    
    while (std::getline(pgn_file, line)) {
        if (line.find("[Event \"") == 0) {
            size_t start = 8;
            size_t end = line.find_last_of('"');
            if (end != std::string::npos && end > start) {
                current_game.event = line.substr(start, end - start);
                tournament_names.insert(current_game.event);
            }
        }
        else if (line.find("[Site \"") == 0) {
            size_t start = 7;
            size_t end = line.find_last_of('"');
            if (end != std::string::npos && end > start) {
                current_game.site = line.substr(start, end - start);
            }
        }
        else if (line.find("[Date \"") == 0) {
            size_t start = 7;
            size_t end = line.find_last_of('"');
            if (end != std::string::npos && end > start) {
                current_game.date = line.substr(start, end - start);
            }
        }
        else if (line.find("[Round \"") == 0) {
            size_t start = 8;
            size_t end = line.find_last_of('"');
            if (end != std::string::npos && end > start) {
                current_game.round = line.substr(start, end - start);
            }
        }
        else if (line.find("[White \"") == 0) {
            size_t start = 8;
            size_t end = line.find_last_of('"');
            if (end != std::string::npos && end > start) {
                current_game.white = line.substr(start, end - start);
                player_names.insert(current_game.white);
            }
        }
        else if (line.find("[Black \"") == 0) {
            size_t start = 8;
            size_t end = line.find_last_of('"');
            if (end != std::string::npos && end > start) {
                current_game.black = line.substr(start, end - start);
                player_names.insert(current_game.black);
            }
        }
        else if (line.find("[Result \"") == 0) {
            size_t start = 9;
            size_t end = line.find_last_of('"');
            if (end != std::string::npos && end > start) {
                current_game.result = line.substr(start, end - start);
            }
        }
        else if (line.find("[WhiteElo \"") == 0) {
            size_t start = 11;
            size_t end = line.find_last_of('"');
            if (end != std::string::npos && end > start) {
                current_game.white_elo = line.substr(start, end - start);
            }
        }
        else if (line.find("[BlackElo \"") == 0) {
            size_t start = 11;
            size_t end = line.find_last_of('"');
            if (end != std::string::npos && end > start) {
                current_game.black_elo = line.substr(start, end - start);
            }
        }
        else if (line.find("[ECO \"") == 0) {
            size_t start = 6;
            size_t end = line.find_last_of('"');
            if (end != std::string::npos && end > start) {
                current_game.eco = line.substr(start, end - start);
            }
        }
        else if (line.find("[Opening \"") == 0) {
            size_t start = 10;
            size_t end = line.find_last_of('"');
            if (end != std::string::npos && end > start) {
                current_game.opening = line.substr(start, end - start);
            }
        }
        else if (!line.empty() && line[0] != '[') {
            current_game.move_count = 0;
            for (size_t i = 0; i < line.length(); i++) {
                if (line[i] == '.') current_game.move_count++;
            }
        }
        
        // Empty line indicates end of game - PROCESS IT IMMEDIATELY
        if (line.empty() && in_game) {
            process_game(current_game);
            update_result_stats(current_game);
            
            current_game = Game{};
            in_game = false;
            stats.total_games++;
            
            if (callback && stats.total_games % 10000 == 0) {
                callback(stats.total_games, "Processing games");
            }
        }
        else if (!line.empty() && line[0] == '[') {
            in_game = true;
        }
    }
    
    // Process last game if file doesn't end with empty line
    if (in_game) {
        process_game(current_game);
        update_result_stats(current_game);
        stats.total_games++;
    }
    
    // Finalize statistics
    stats.tournament_names.assign(tournament_names.begin(), tournament_names.end());
    stats.player_names.assign(player_names.begin(), player_names.end());
    stats.unique_tournaments = tournament_names.size();
    stats.unique_players = player_names.size();
    
    // Calculate player percentages and find most active
    for (auto& [name, player] : player_stats) {
        player.calculate_percentages();
        
        if (player.total_games > stats.max_games_by_player) {
            stats.max_games_by_player = player.total_games;
            stats.most_active_player = name;
        }
    }
    
    // Find largest tournament
    for (const auto& [name, tournament] : tournaments) {
        if (tournament.total_games > stats.max_games_in_tournament) {
            stats.max_games_in_tournament = tournament.total_games;
            stats.largest_tournament = name;
        }
    }
    
    // Move the stats to the result
    stats.player_stats = std::move(player_stats);
    stats.tournaments = std::move(tournaments);
    
    auto end_time = std::chrono::high_resolution_clock::now();
    stats.parsing_time_seconds = 
        std::chrono::duration<double>(end_time - start_time).count();
    
    pgn_file.close();
    return true;
}

void StreamingParser::process_game(const Game& game) {
    // Update player stats
    PlayerStats& white = player_stats[game.white];
    white.name = game.white;
    white.total_games++;
    white.games_as_white++;
    
    if (game.is_white_win()) white.wins++;
    else if (game.is_black_win()) white.losses++;
    else if (game.is_draw()) white.draws++;
    
    // Fix: Use begin() and end() iterators for std::find
    if (std::find(white.opponents.begin(), white.opponents.end(), game.black) == white.opponents.end()) {
        white.opponents.push_back(game.black);
    }
    
    if (!game.eco.empty()) {
        white.opening_frequency[game.eco]++;
    }
    
    PlayerStats& black = player_stats[game.black];
    black.name = game.black;
    black.total_games++;
    black.games_as_black++;
    
    if (game.is_black_win()) black.wins++;
    else if (game.is_white_win()) black.losses++;
    else if (game.is_draw()) black.draws++;
    
    // Fix: Use begin() and end() iterators for std::find
    if (std::find(black.opponents.begin(), black.opponents.end(), game.white) == black.opponents.end()) {
        black.opponents.push_back(game.white);
    }
    
    if (!game.eco.empty()) {
        black.opening_frequency[game.eco]++;
    }
    
    // Update tournament stats
    Tournament& tournament = tournaments[game.event];
    tournament.name = game.event;
    tournament.total_games++;
    
    // Fix: Use begin() and end() iterators for std::find
    if (std::find(tournament.players.begin(), tournament.players.end(), game.white) == tournament.players.end()) {
        tournament.players.push_back(game.white);
    }
    if (std::find(tournament.players.begin(), tournament.players.end(), game.black) == tournament.players.end()) {
        tournament.players.push_back(game.black);
    }
    
    tournament.unique_players = tournament.players.size();
    tournament.player_game_count[game.white]++;
    tournament.player_game_count[game.black]++;
}

void StreamingParser::update_result_stats(const Game& game) {
    if (game.is_white_win()) stats.white_wins++;
    else if (game.is_black_win()) stats.black_wins++;
    else if (game.is_draw()) stats.draws++;
    else stats.unknown_results++;
}

const DatabaseStats& StreamingParser::get_stats() const {
    return stats;
}

DatabaseStats StreamingParser::analyze_file(const std::string& filename, ProgressCallback callback) {
    StreamingParser parser;
    if (parser.load_file(filename, callback)) {
        return parser.get_stats();
    }
    return DatabaseStats{};
}

} // namespace pgn