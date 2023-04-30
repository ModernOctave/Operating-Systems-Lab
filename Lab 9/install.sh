cp -f open.c /usr/src/minix/servers/vfs/open.c
cp -f read.c /usr/src/minix/servers/vfs/read.c
cp -f link.c /usr/src/minix/servers/vfs/link.c
cd /usr/src && make build MKUPDATE=yes