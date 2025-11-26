# lem-ipc

# Règles générales
L’idée est que des joueurs (réunis en équipes) s’affrontent sur un plateau en 2D. Pour qu’une équipe soit victorieuse, elle doit être la dernière encore présente sur le plateau.
Lorsqu’un joueur meurt, il disparaît du plateau.

Pour qu’un joueur soit tué, il doit être touché par au moins deux joueurs de la même équipe, c’est-à-dire occupant des cases adjacentes à celle où se trouve la cible (les diagonales comptent). Bien sûr, l’équipe qui tue doit être différente de celle de la cible. Lorsqu’un joueur réalise qu’il est entouré par au moins deux joueurs d’une autre équipe mais appartenant à la même équipe entre eux, il doit quitter le plateau et mettre fin à son exécution.

Une case du plateau ne peut contenir qu’un seul joueur à la fois.

Le rendu inclut :
- une simulation multi-processus
- un protocole de communication entre équipes
- un moteur graphique utilisant **MiniLibX**
- un arbitre (core) orchestrant l’arène

---

## 🎥 Démonstration

![Demonstration](https://github.com/misantrop76/lem-ipc/blob/main/assets/lem-ipc.gif?raw=true)

---

## 🧠 Objectifs pédagogiques

- Utilisation des **IPC** :
  - `message queues`
  - `shared memory`
  - `semaphores`
- Synchronisation concurrente
- Architecture logicielle modulaire
- Visualisation graphique avec **MiniLibX**
- Gestion d’un protocole réseau local (communication inter-agents)

---



