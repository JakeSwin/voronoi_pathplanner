-- Function to compute Cartesian product of multiple lists
local M = {}

function M.cartesianProduct(lists)
    local result = { {} }

    for _, list in ipairs(lists) do
        local temp = {}
        for _, res in ipairs(result) do
            for _, value in ipairs(list) do
                -- Create a new combination by appending the value to the existing result
                local newCombination = { table.unpack(res) }
                table.insert(newCombination, value)
                table.insert(temp, newCombination)
            end
        end
        result = temp
    end

    return result
end

return M
