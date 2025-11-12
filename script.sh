#!/bin/bash
#7784 945 274 6699 2
ipcs -m > before.txt
teams=(7784 945 274 6699 2)

bots_per_team=1000

echo "Lancement de $(( ${#teams[@]} * bots_per_team )) bots..."

for team_id in "${teams[@]}"; do
    echo "Lancement de l'équipe $team_id ..."
    for ((i=1; i<=bots_per_team; i++)); do
        ./lem-ipc "$team_id" &
    done
	sleep 0.1
done
./vizu
wait
ipcs -m > after.txt
diff before.txt after.txt