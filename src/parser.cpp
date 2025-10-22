#include "pgn/parser.hpp"
#include "pgn/types.hpp"
#include <fstream>
#include <iostream>
#include <unordered_set>
#include <unordered_map>
#include <chrono>
#include <algorithm>

namespace pgn {

struct Parser::Impl {
    std::vector<Game> games;
    DatabaseStats stats;
    
    void parse_file(const std::string& filename, ProgressCallback callback);
    void analyze_data(ProgressCallback callback);
    void update_player_stats(const Game& game);
    void update_tournament_stats(const Game& game);
};

Parser::Parser() : pimpl(std::make_unique<Impl>()) {}
Parser::~Parser() = default;

bool Parser::load_file(const std::string& filename, ProgressCallback callback) {
    try {
        pimpl->parse_file(filename, callback);
        pimpl->analyze_data(callback);
        return true;
    } catch (const std::exception& e) {
        std::cerr << "Error loading file: " << e.what() << std::endl;
        return false;
    }
}

const DatabaseStats& Parser::get_stats() const {
    return pimpl->stats;
}

const std::vector<Game>& Parser::get_games() const {
    return pimpl->games;
}

const std::unordered_map<std::string, PlayerStats>& Parser::get_player_stats() const {
    return pimpl->stats.player_stats;
}

const std::unordered_map<std::string, Tournament>& Parser::get_tournaments() const {
    return pimpl->stats.tournaments;
}

void Parser::Impl::parse_file(const std::string& filename, ProgressCallback callback) {
    auto start_time = std::chrono::high_resolution_clock::now();
    
    std::ifstream pgn_file(filename);
    if (!pgn_file.is_open()) {
        throw std::runtime_error("Cannot open file: " + filename);
    }
    
    games.clear();
    stats = DatabaseStats{};
    
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
        
        if (line.empty() && in_game) {
            games.push_back(current_game);
            current_game = Game{};
            in_game = false;
            stats.total_games++;
            
            if (callback && stats.total_games % 1000 == 0) {
                callback(stats.total_games, "Parsing games");
            }
        }
        else if (!line.empty() && line[0] == '[') {
            in_game = true;
        }
    }
    
    if (in_game) {
        games.push_back(current_game);
        stats.total_games++;
    }
    
    stats.tournament_names.assign(tournament_names.begin(), tournament_names.end());
    stats.player_names.assign(player_names.begin(), player_names.end());
    stats.unique_tournaments = tournament_names.size();
    stats.unique_players = player_names.size();
    
    auto end_time = std::chrono::high_resolution_clock::now();
    stats.parsing_time_seconds = 
        std::chrono::duration<double>(end_time - start_time).count();
    
    pgn_file.close();
}

void Parser::Impl::analyze_data(ProgressCallback callback) {
    if (callback) callback(0, "Analyzing data");
    
    stats.player_stats.clear();
    stats.tournaments.clear();
    stats.white_wins = 0;
    stats.black_wins = 0;
    stats.draws = 0;
    stats.unknown_results = 0;
    
    for (size_t i = 0; i < games.size(); ++i) {
        update_player_stats(games[i]);
        update_tournament_stats(games[i]);
        
        if (games[i].is_white_win()) stats.white_wins++;
        else if (games[i].is_black_win()) stats.black_wins++;
        else if (games[i].is_draw()) stats.draws++;
        else stats.unknown_results++;
        
        if (callback && (i % 1000 == 0)) {
            callback(i, "Analyzing games");
        }
    }
    
    for (auto& [name, player] : stats.player_stats) {
        player.calculate_percentages();
        
        if (player.total_games > stats.max_games_by_player) {
            stats.max_games_by_player = player.total_games;
            stats.most_active_player = name;
        }
    }
    
    for (const auto& [name, tournament] : stats.tournaments) {
        if (tournament.total_games > stats.max_games_in_tournament) {
            stats.max_games_in_tournament = tournament.total_games;
            stats.largest_tournament = name;
        }
    }
    
    if (callback) callback(games.size(), "Analysis complete");
}

void Parser::Impl::update_player_stats(const Game& game) {
    PlayerStats& white = stats.player_stats[game.white];
    white.name = game.white;
    white.total_games++;
    white.games_as_white++;
    
    if (game.is_white_win()) white.wins++;
    else if (game.is_black_win()) white.losses++;
    else if (game.is_draw()) white.draws++;
    
    if (std::find(white.opponents.begin(), white.opponents.end(), game.black) == white.opponents.end()) {
        white.opponents.push_back(game.black);
    }
    
    if (!game.eco.empty()) {
        white.opening_frequency[game.eco]++;
    }
    
    PlayerStats& black = stats.player_stats[game.black];
    black.name = game.black;
    black.total_games++;
    black.games_as_black++;
    
    if (game.is_black_win()) black.wins++;
    else if (game.is_white_win()) black.losses++;
    else if (game.is_draw()) black.draws++;
    
    if (std::find(black.opponents.begin(), black.opponents.end(), game.white) == black.opponents.end()) {
        black.opponents.push_back(game.white);
    }
    
    if (!game.eco.empty()) {
        black.opening_frequency[game.eco]++;
    }
}

void Parser::Impl::update_tournament_stats(const Game& game) {
    Tournament& tournament = stats.tournaments[game.event];
    tournament.name = game.event;
    tournament.total_games++;
    
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

DatabaseStats Parser::analyze_file(const std::string& filename, ProgressCallback callback) {
    Parser parser;
    if (parser.load_file(filename, callback)) {
        return parser.get_stats();
    }
    return DatabaseStats{};
}

void Parser::export_player_stats_csv(const std::string& filename) const {
    // TODO: Implement
}

void Parser::export_tournaments_csv(const std::string& filename) const {
    // TODO: Implement
}

} // namespace pgn