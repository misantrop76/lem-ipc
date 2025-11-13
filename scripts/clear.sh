
pkill -x lem-ipc
for id in $(ipcs -m | awk 'NR>3 {print $2}'); do ipcrm -m "$id"; done
for id in $(ipcs -s | awk 'NR>3 {print $2}'); do ipcrm -s "$id"; done
for id in $(ipcs -q | awk 'NR>3 {print $2}'); do ipcrm -q "$id"; done
