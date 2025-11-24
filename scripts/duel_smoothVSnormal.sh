#!/bin/bash

teams=(7784 945)

bots_per_team=300

echo "Lancement de $(( ${#teams[@]} * bots_per_team )) bots..."
make
for ((i=1; i<=bots_per_team; i++)); do
    ./lem-ipc 7794 1 &
	./lem-ipc 945 &
done
echo "Tous les bots sont lancé !"
./vizu
wait
