cp -f schedule.c /usr/src/minix/servers/sched/schedule.c
cp -f forkexit.c /usr/src/minix/servers/pm/forkexit.c
cp -f system.c /usr/src/minix/kernel/system.c
cd /usr/src && make build MKUPDATE=yes