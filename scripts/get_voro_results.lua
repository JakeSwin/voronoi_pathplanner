---@type Game
local Game = Game
package.path = package.path .. ";./scripts/?.lua"
local lib = require("lib")

local imagePath = "./images/gps/"
local resultPath = "./resultsvoro/"

local gpMap = { "000", "001", "002", "003", "004" }
local numMaps = 5

print("Running lua script")

local g = Game.new(imagePath .. gpMap[1] .. "_0.png", "This is a test Raylib window")

g:SetParamAlpha(12)
g:SetParamBeta(0)
g:SetParamGamma(1)
g:SetMaxDistanceMultiplier(1.5)
g:SetWeightFactor(1.75)
g:SetWeightMult(1)
g:Sample(20000, 500)
g:GenerateVoronoi(75)

local imageIndex = 1
local iterCount = 1

while not g:Step() do
    g:Screenshot(resultPath .. gpMap[imageIndex] .. "_" .. (iterCount - 1) .. "_voro.png")
    if iterCount >= numMaps then
        iterCount = 1
        imageIndex = imageIndex + 1
        if imageIndex > #gpMap then
            break
        end
    else
        iterCount = iterCount + 1
    end
    g:SetImage(imagePath .. gpMap[imageIndex] .. "_" .. (iterCount - 1) .. ".png", false)
    g:Sample(20000, 500)
    g:GenerateVoronoi(75)
end
