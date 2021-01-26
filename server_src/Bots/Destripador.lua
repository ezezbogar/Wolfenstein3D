WALKABLE = 0
DIVISIONS_AMOUNT = 10
PI = 3.1415926

NOT_MOVE = 0
MOVE_FORWARDS = 1
MOVE_BACKWARDS = 2
TURN_LEFT = 3
TURN_RIGHT = 4
ATTACK = 5

function hola()

    print(5)

end

function getBotInstruction(map, positions, posX, posY, dirX, dirY)

    print(54);
    print(posX)
    print(posY)
    print(dirX)
    print(dirY)


    --print(map[0][0])
    --print(map[0][1])
    --print(map[1][1])
    --print(map[1][0])
    --print(map[2][0])
    --print(map[3][4])

    --print(positions[0])
    --print(positions[1])
    --print(positions[2])
    --print(positions[3])
    --print(positions[4])
    --print(positions[5])
    --print(positions[6])


    --prueba
    posx1 = 2
    posy2 = 2

    dirx = 1
    diry = 0
    --prueba

    print(tableLength(positions))

    x = isInTheFieldOfView(3, 3, 7, 3, map)
    print(x)

    print(angle(2, 1, 1, 0, 2, 1))
    print("-----Fin Lua-----")

    return MOVE_FORWARDS
end

function tableLength(T)
    local count = 0
    for i in pairs(T) do
        count = count + 1
    end
    return count
end


function isInTheFieldOfView(xP1, xP2, yP1, yP2, map)
    inTheFieldOfView = true

    dirX = xP2 - xP1
    dirY = yP2 - yP1
    norm = math.sqrt(dirX * dirX + dirY * dirY)

    stepX = dirX/(norm * DIVISIONS_AMOUNT)  -- Step de norma igual a 1/10
    stepY = dirY/(norm * DIVISIONS_AMOUNT)  -- Step de norma igual a 1/10

    for i = 0, (DIVISIONS_AMOUNT * norm) do
        if map[math.floor(xP1 + i * stepX)][math.floor(yP1 + i * stepY)] ~= WALKABLE then
            inTheFieldOfView = false
            break
        end
    end

    return inTheFieldOfView;
end

function angle(xP1, xP2, dirXP1, dirYP1, yP1, yP2)
    diffX = xP2 - xP1
    diffY = yP2 - yP1
    theta = math.acos((dirXP1 * diffX + dirYP1 * diffY) / math.sqrt(diffX * diffX + diffY * diffY))
    return theta
end

function getDirection(thetaDir, thetaPos)
    if thetaDir > thetaPos then
        if thetaDir - thetaPos > PI then
            return TURN_LEFT
        else
            return TURN_RIGHT
        end
    end
    if thetaDir <= thetaPos then
        if thetaPos - thetaDir < PI then
            return TURN_RIGHT
        else
            return TURN_LEFT
        end
    end
end