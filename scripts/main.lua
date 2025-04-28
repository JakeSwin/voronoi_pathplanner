---@type Game
local Game = Game
package.path = package.path .. ";./scripts/?.lua"
local lib = require("lib")

local imagePath = "./images/"
local resultPath = "./results1/"

local images = {
    "dots.png",
    "car.png",
    "dog.png",
    "fisk.png",
    "gp_map_grey_inv.png",
}

local numIters = 75

print("Running lua script")

local g = Game.new(imagePath .. images[1], "This is a test Raylib window")
g:SetParamAlpha(12)
g:SetParamBeta(0)
g:SetParamGamma(1)
g:SetMaxDistanceMultiplier(1.5)
g:SetWeightFactor(1.75)
g:SetWeightMult(1)
g:Sample(20000, 500)
g:GenerateVoronoi(1)

local imageIndex = 1
local iterCount = 1

while not g:Step() do
    if iterCount >= numIters then
        g:Screenshot(resultPath .. images[imageIndex])
        imageIndex = (imageIndex % #images) + 1
        iterCount = 1
        g:SetImage(imagePath .. images[imageIndex])
        g:Sample(20000, 500)
    else
        iterCount = iterCount + 1
        g:GenerateVoronoi(1)
    end
end
