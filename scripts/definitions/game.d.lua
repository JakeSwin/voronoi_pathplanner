---@meta
---@alias number number
---@alias string string
---@alias nil nil

---@class Game
---@field new fun(imagepath: string, title: string): Game
local Game = {}

---Run the game main loop, return true when game is ending
---@return boolean
function Game:Step() end

---Sets the image the pathplanner runs on
---@param imagepath string The path to the image file
---@return nil
function Game:SetImage(imagepath) end

---Generates a weighted voronoi diagram using current initial sample
---@param num_iters number how many iterations to run for
---@return boolean
function Game:GenerateVoronoi(num_iters) end

---Takes a screenshot of the current frame and saves it to the specified file
---@param imagepath string The path to the image file
---@return nil
function Game:Screenshot(imagepath) end

---Performs Poisson sample elimination weighted by the current image
---@param input_size number number of input points to eliminate
---@param output_size number number of output points to produce
---@return nil
function Game:Sample(input_size, output_size) end

---Sets the alpha parameter for sample elimination
---@param alpha number desired alpha value
---@return nil
function Game:SetParamAlpha(alpha) end

---Sets the beta parameter for sample elimination
---@param beta number desired beta value
---@return nil
function Game:SetParamBeta(beta) end

---Sets the gamma parameter for sample elimination
---@param gamma number desired gamma value
---@return nil
function Game:SetParamGamma(gamma) end

---Sets the d_max_mult parameter for sample elimination
---@param d_max_mult number desired d_max_mult value
---@return nil
function Game:SetMaxDistanceMultiplier(d_max_mult) end

---Sets the weight_factor parameter for sample elimination
---@param weight_factor number desired weight_factor value
---@return nil
function Game:SetWeightFactor(weight_factor) end

---Sets the weight_mult parameter for sample elimination
---@param weight_mult number desired weight_mult value
---@return nil
function Game:SetWeightMult(weight_mult) end
