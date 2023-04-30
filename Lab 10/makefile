IP = 10.196.89.226

sync-src:
	rsync -av --exclude='.git' "../Minix3 (copy)/minix/" root@${IP}:/usr/src/minix/

zip:
	rm -f 200010036_200010037_lab10.zip
	zip -r 200010036_200010037_lab10.zip . -x makefile

sync:
	rsync -av 200010036_200010037_lab10.zip root@${IP}:/root/