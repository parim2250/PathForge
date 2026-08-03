#pragma once
#include "../core/Grid.h"
#include "PresetMazes.h"
#include "MazeValidator.h"
#include "MazeAnalyzer.h"
#include <iostream>

class MazeLoader {
public:
    static bool loadPreset(Grid& grid, Difficulty diff, int level) {
        PresetMazes::loadInto(grid, diff, level);

        // Validate path
        if (!MazeValidator::hasValidPath(grid)) {
            std::cout << "[WARNING] Maze has no valid path!" << std::endl;
            return false;
        }

        // Analyze difficulty
        MazeAnalyzer::AnalysisResult analysis = MazeAnalyzer::analyze(grid);

        std::cout << "  ---------------------------------" << std::endl;
        std::cout << "  MAZE ANALYSIS" << std::endl;
        std::cout << "  ---------------------------------" << std::endl;
        std::cout << "  Path found:         YES" << std::endl;
        std::cout << "  Optimal path length: " << analysis.optimalPathLength << " cells" << std::endl;
        std::cout << "  Open cells:         " << analysis.openCells << std::endl;
        std::cout << "  Wall cells:         " << analysis.wallCells << std::endl;
        std::cout << "  Dead ends:          " << analysis.deadEnds << std::endl;
        std::cout << "  Difficulty score:   " << analysis.difficultyScore << "/100 (" << analysis.difficultyRating << ")" << std::endl;
        std::cout << "  ---------------------------------" << std::endl;

        return true;
    }

    static std::string difficultyName(Difficulty d) {
        switch (d) {
            case Difficulty::BEGINNER:     return "BEGINNER";
            case Difficulty::INTERMEDIATE: return "INTERMEDIATE";
            case Difficulty::ADVANCED:     return "ADVANCED";
        }
        return "UNKNOWN";
    }
};