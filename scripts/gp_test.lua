---@type Game
local Game = Game
package.path = package.path .. ";./scripts/?.lua"
local lib = require("lib")

local resultPath = "./ppresults/"

local numIters = 75

print("Running lua script")

-- Table to store MovePlanner results
local movePlannerResults = {}

local g = Game.new("./images/gps/000_0.png", "This is a test Raylib window")
g:SetImage("./images/orthomosaics/000.png", true)
g:SetParamAlpha(12)
g:SetParamBeta(0)
g:SetParamGamma(1)
g:SetMaxDistanceMultiplier(1.5)
g:SetWeightFactor(1.75)
g:SetWeightMult(1)
g:Sample(20000, 500)
g:GenerateVoronoi(75)

local iterCount = 1

while not g:Step() do
    local result = g:MovePlanner()
    table.insert(movePlannerResults, result)
    if g:GetNeighbourCount() == 0 then
        break
    end

    g:Screenshot(resultPath .. string.format("%03d", iterCount) .. "_frame.png")
    iterCount = iterCount + 1
end

-- Save results to text file at program end
local outputFile = io.open(resultPath .. "move_planner_results.txt", "w")
if outputFile then
    for i, value in ipairs(movePlannerResults) do
        outputFile:write(string.format("Iteration %d: %.4f\n", i, value))
    end
    outputFile:close()
end

print("Saved MovePlanner results to " .. resultPath .. "move_planner_results.txt")
