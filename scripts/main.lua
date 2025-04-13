---@type Game
local Game = Game

local imagePath = "./images/"
local resultPath = "./results/"

local images = {
    "dots.png",
    "car.png",
    "dog.jpg",
    "fisk.jpg",
    "gp_map_grey.png",
}
local numIters = 75

print("Running lua script")

local g = Game.new(imagePath .. images[1], "This is a test Raylib window")
g:Sample(20000, 5000)

local imageIndex = 1
local iterCount = 1

while not g:Step() do
    if iterCount >= numIters then
        imageIndex = (imageIndex % #images) + 1
        iterCount = 1
        g:Screenshot(resultPath .. images[imageIndex])
        g:SetImage(imagePath .. images[imageIndex])
        g:Sample(20000, 5000)
    else
        iterCount = iterCount + 1
    end
end
