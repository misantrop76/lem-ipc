#!/bin/bash

teams=(7784 945)

bots_per_team=4

echo "Lancement de $(( ${#teams[@]} * bots_per_team )) bots..."
make
for ((i=1; i<=bots_per_team; i++)); do
    for team_id in "${teams[@]}"; do
        ./lem-ipc "$team_id" &
    done
done
echo "Tous les bots sont lancé !"
./vizu
wait

