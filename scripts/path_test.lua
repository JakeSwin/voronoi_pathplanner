---@type Game
local Game = Game
package.path = package.path .. ";./scripts/?.lua"
local lib = require("lib")

local resultPath = "./ppresults/"

local numIters = 75

print("Running lua script")

local g = Game.new("./images/gp_map_grey_inv.png", "This is a test Raylib window")
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
    g:MovePlanner()
    g:Screenshot(resultPath .. string.format("%03d", iterCount) .. "_frame.png")
    iterCount = iterCount + 1
end
