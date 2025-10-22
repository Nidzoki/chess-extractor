#pragma once
#include "types.hpp"
#include <functional>
#include <unordered_map>

namespace pgn {

class StreamingParser {
public:
    using ProgressCallback = std::function<void(int, const std::string&)>;
    
    static DatabaseStats analyze_file(const std::string& filename, 
                                     ProgressCallback callback = nullptr);
    
    StreamingParser();
    bool load_file(const std::string& filename, ProgressCallback callback = nullptr);
    const DatabaseStats& get_stats() const;
    
private:
    DatabaseStats stats;
    std::unordered_map<std::string, PlayerStats> player_stats;
    std::unordered_map<std::string, Tournament> tournaments;
    
    void process_game(const Game& game);
    void update_result_stats(const Game& game);
};

} // namespace pgn