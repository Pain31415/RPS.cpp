#ifndef GAME_H
#define GAME_H

#include <string>
#include <vector>

enum class Move { ROCK, PAPER, SCISSORS, INVALID };

class Game {
public:
    enum class GameMode { PLAYER_VS_PLAYER, PLAYER_VS_COMPUTER, COMPUTER_VS_COMPUTER };

    Game(int rounds, GameMode mode = GameMode::PLAYER_VS_PLAYER);
    void playRound(Move player1Move, Move player2Move);
    std::string getResult() const;
    bool isGameOver() const;
    void displayStats() const;
    void requestDraw();
    void forfeit();
    Move getRandomMove() const;
    std::string moveToString(Move move) const;
    Move getMoveFromString(const std::string& moveStr) const;

private:
    int totalRounds;
    int currentRound;
    int player1Score;
    int player2Score;
    GameMode gameMode;
    std::vector<std::string> roundsResults;
};

#endif // GAME_H
