#!/bin/bash
teams=(7784 945 274 6699 2 6000)

bots_per_team=200

echo "Lancement de $(( ${#teams[@]} * bots_per_team )) bots..."
make
for ((i=1; i<=bots_per_team; i++)); do
    for team_id in "${teams[@]}"; do
        ./lem-ipc "$team_id" &
    done
done
./vizu
echo "Tous les bots sont lancé !"
wait

