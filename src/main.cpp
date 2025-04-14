#define SOL_ALL_SAFETIES_ON 1
#include <sol/sol.hpp>

#include "game.hpp"

int main(int argc, char* argv[]) {
    sol::state lua;
    lua.open_libraries(
        sol::lib::base,
        sol::lib::package,
        sol::lib::table,
        sol::lib::string
    );

    lua.new_usertype<Game>(
        "Game",
        sol::constructors<Game(const char*,const char*)>(),
        "Step", &Game::Step,
        "SetImage", &Game::SetImage,
        "Screenshot", &Game::Screenshot,
        "Sample", &Game::Sample,
        "SetParamAlpha", &Game::SetParamAlpha,
        "SetParamBeta", &Game::SetParamBeta,
        "SetParamGamma", &Game::SetParamGamma,
        "SetMaxDistanceMultiplier", &Game::SetMaxDistanceMultiplier,
        "SetWeightFactor", &Game::SetWeightFactor,
        "SetWeightMult", &Game::SetWeightMult
    );

    if (argc > 1) {
        lua.script_file(argv[1]);
    } else {
        lua.script_file("./scripts/main.lua");
    }

    return 0;
}
