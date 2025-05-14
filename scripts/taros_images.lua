---@type Game
local Game = Game
package.path = package.path .. ";./scripts/?.lua"
local lib = require("lib")

local imagePath = "./images/gps/000_0.png"
local resultPath = "./results/taros/"

print("Running lua script")

local g = Game.new(imagePath, "This is a test Raylib window")

g:SetParamAlpha(12)
g:SetParamBeta(0)
g:SetParamGamma(1)
g:SetMaxDistanceMultiplier(1.5)
g:SetWeightFactor(1.75)
g:SetWeightMult(1)

g:Sample(20000, 500)
g:Screenshot(resultPath .. "initial_sampling.png")
g:GenerateVoronoi(0)
g:Screenshot(resultPath .. "initial_voro.png")
g:GenerateVoronoi(75)
g:Screenshot(resultPath .. "75_iters.png")
