#!/bin/bash

teams=(2315 6020 1243 4800)

bots_per_team=500

echo "Lancement de $(( ${#teams[@]} * bots_per_team )) bots..."

for team_id in "${teams[@]}"; do
    echo "Lancement de l'équipe $team_id ..."
    for ((i=1; i<=bots_per_team; i++)); do
        ./lem-ipc "$team_id" &
    done
done
./vizu
echo "Tous les bots ont été lancés !"
