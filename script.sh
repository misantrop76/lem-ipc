#!/bin/bash

teams=(233 420 143 40)

bots_per_team=5

echo "Lancement de $(( ${#teams[@]} * bots_per_team )) bots..."

for team_id in "${teams[@]}"; do
    echo "Lancement de l'équipe $team_id ..."
    for ((i=1; i<=bots_per_team; i++)); do
        ./lem-ipc "$team_id" &
        sleep 0.1
    done
done
./vizu
echo "Tous les bots ont été lancés !"
wait
