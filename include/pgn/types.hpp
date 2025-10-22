#pragma once
#include <string>
#include <vector>
#include <unordered_map>
#include <chrono>

namespace pgn {

struct Game {
    std::string event;
    std::string site;
    std::string date;
    std::string round;
    std::string white;
    std::string black;
    std::string result;
    std::string white_elo;
    std::string black_elo;
    std::string eco;
    std::string opening;
    int move_count = 0;
    
    bool is_white_win() const { return result == "1-0"; }
    bool is_black_win() const { return result == "0-1"; }
    bool is_draw() const { return result == "1/2-1/2"; }
    bool is_unknown_result() const { return result == "*"; }
};

struct PlayerStats {
    std::string name;
    int total_games = 0;
    int games_as_white = 0;
    int games_as_black = 0;
    int wins = 0;
    int losses = 0;
    int draws = 0;
    double win_percentage = 0.0;
    double draw_percentage = 0.0;
    std::vector<std::string> opponents;
    std::unordered_map<std::string, int> opening_frequency;
    
    void calculate_percentages() {
        if (total_games > 0) {
            win_percentage = (static_cast<double>(wins) / total_games) * 100.0;
            draw_percentage = (static_cast<double>(draws) / total_games) * 100.0;
        }
    }
};

struct Tournament {
    std::string name;
    int total_games = 0;
    int unique_players = 0;
    std::vector<std::string> players;
    std::unordered_map<std::string, int> player_game_count;
};

struct DatabaseStats {
    int total_games = 0;
    int unique_tournaments = 0;
    int unique_players = 0;
    int white_wins = 0;
    int black_wins = 0;
    int draws = 0;
    int unknown_results = 0;
    std::string most_active_player;
    int max_games_by_player = 0;
    std::string largest_tournament;
    int max_games_in_tournament = 0;
    double parsing_time_seconds = 0.0;
    std::vector<std::string> tournament_names;
    std::vector<std::string> player_names;
    std::unordered_map<std::string, PlayerStats> player_stats;
    std::unordered_map<std::string, Tournament> tournaments;
};

} // namespace pgn