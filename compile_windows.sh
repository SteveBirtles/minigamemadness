#!/bin/bash
if [ -z "$1" ] 
then
    echo "No game number supplied"
else 
    g++ -o game$1 game$1source/game$1.cpp -luser32 -lgdi32 -lopengl32 -lgdiplus -lShlwapi -ldwmapi -lstdc++fs -std=c++17
fi