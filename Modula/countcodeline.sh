#/bin/bash
cnt=0; IFS=$'\n';
for i in $(find -name '*.h' -or -name '*.c' -or -name '*.asm' -or -name '*.inc' -or -name '*.S');
do
	cnt=$(($cnt + $(wc -l $i | cut -d\  -f1)));
done;
echo 'Modula lines count :' $cnt
