#!/bin/bash
#7784 945 274 6699 2
ipcs -m > before.txt
teams=(7784 945 274 6699 2)

bots_per_team=200

echo "Lancement de $(( ${#teams[@]} * bots_per_team )) bots..."

for ((i=1; i<=bots_per_team; i++)); do
    #echo "Lancement de l'équipe $team_id ..."
    for team_id in "${teams[@]}"; do
        ./lem-ipc "$team_id" &
    done
done
./vizu
wait
ipcs -m > after.txt
diff before.txt after.txt
