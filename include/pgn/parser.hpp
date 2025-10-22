#pragma once
#include "types.hpp"
#include <functional>
#include <memory>

namespace pgn {

class Parser {
public:
    using ProgressCallback = std::function<void(int, const std::string&)>;
    
    static DatabaseStats analyze_file(const std::string& filename, 
                                     ProgressCallback callback = nullptr);
    
    Parser();
    ~Parser();
    
    bool load_file(const std::string& filename, ProgressCallback callback = nullptr);
    const DatabaseStats& get_stats() const;
    
    const std::vector<Game>& get_games() const;
    const std::unordered_map<std::string, PlayerStats>& get_player_stats() const;
    const std::unordered_map<std::string, Tournament>& get_tournaments() const;
    
    void export_player_stats_csv(const std::string& filename) const;
    void export_tournaments_csv(const std::string& filename) const;

private:
    struct Impl;
    std::unique_ptr<Impl> pimpl;
};

} // namespace pgn