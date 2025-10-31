#!/bin/bash

teams=(45 95 289 1750)

bots_per_team=20

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
