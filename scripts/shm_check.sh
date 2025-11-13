#!/bin/bash
#7784 945 274 6699 2
ipcs -m > before.txt
teams=(7784 945 274 6699 2)

bots_per_team=1999

echo "Lancement de $(( ${#teams[@]} * bots_per_team )) bots..."
make
for ((i=1; i<=bots_per_team; i++)); do
    for team_id in "${teams[@]}"; do
        ./lem-ipc "$team_id" &
    done
done
valgrind ./lem-in 2
valgrind ./vizu
echo "Tous les bots sont lancé !"
wait
ipcs -m > after.txt
diff before.txt after.txt