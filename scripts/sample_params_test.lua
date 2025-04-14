---@type Game
local Game = Game
package.path = package.path .. ";./scripts/?.lua"
local lib = require("lib")

local imagePath = "./images/"
local resultPath = "./results/"

local images = {
    "dots.png",
    "fisk.png",
    "gp_map_grey_inv.png",
}
-- local alphaList = { 8, 10, 12 }
-- local betaList = { 0.65, 0, 0.35, 0.9 }
-- local gammaList = { 1.5, 1, 2 }
-- local maxDistMultList = { 2, 3, 1, 4 }
-- local weightFactorList = { 3, 2, 4 }
-- local weightMultList = { 2, 1.5, 2.5, 3 }
local alphaList = { 8, 12 }
local betaList = { 0.65, 0, 0.9 }
local gammaList = { 1.5, 1, 2 }
local maxDistMultList = { 2, 3, 1 }
local weightFactorList = { 3, 2, 4 }
local weightMultList = { 2, 1.5, 2.5 }

local testParams = {
    alphaList,
    betaList,
    gammaList,
    maxDistMultList,
    weightFactorList,
    weightMultList
}

-- Generate Cartesian product
local combinations = lib.cartesianProduct(testParams)

local numIters = 10

print("Running lua script")

local g = Game.new(imagePath .. images[1], "This is a test Raylib window")

-- Combinations
for _, combo in ipairs(combinations) do
    local paramName = table.concat(combo, "_")
    paramName = string.gsub(paramName, "%.", "p")
    print("Executing: " .. paramName)
    g:SetParamAlpha(combo[1])
    g:SetParamBeta(combo[2])
    g:SetParamGamma(combo[3])
    g:SetMaxDistanceMultiplier(combo[4])
    g:SetWeightFactor(combo[5])
    g:SetWeightMult(combo[6])

    local imageIndex = 1
    local iterCount = 1

    -- g:Sample(20000, 5000)
    -- while not g:Step() do
    --     if iterCount >= numIters then
    --         g:Screenshot(
    --             resultPath ..
    --             paramName ..
    --             "_" ..
    --             images[imageIndex]
    --         )
    --         iterCount = 1
    --         if imageIndex == #images then
    --             imageIndex = 1
    --             break
    --         else
    --             imageIndex = imageIndex + 1
    --         end
    --         g:SetImage(imagePath .. images[imageIndex])
    --         g:Sample(20000, 5000)
    --     else
    --         iterCount = iterCount + 1
    --     end
    -- end
    g:SetImage(imagePath .. images[imageIndex])
    g:Sample(20000, 5000)

    while imageIndex <= #images and not g:Step() do
        if iterCount >= numIters then
            g:Screenshot(
                resultPath ..
                paramName ..
                "_" ..
                images[imageIndex]
            )
            iterCount = 1
            imageIndex = imageIndex + 1
            if imageIndex > #images then
                break
            end
            g:SetImage(imagePath .. images[imageIndex])
            g:Sample(20000, 5000)
        else
            iterCount = iterCount + 1
        end
    end
end
