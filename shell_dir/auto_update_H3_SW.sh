#!/bin/sh
#start when start H3 hock
LOCAL_DIR=/home/yh/foropenwrt/
echo $FLOCAL_DIR
IP=www.baidu.com
TAR_NAME=/tmp/package.tar
packge_file=/tmp/package/

ping -s 1 -c 1 $IP
if [ "$?" != "0" ]
then
	echo "network error"
else
	echo "start download"
	if [ -d "$packge_file" ]
	then  
		rm -rf $packge_file
		rm /tmp/$TAR_NAME 
	fi  

	curl -X GET --header "Accept: application/json;charset=UTF-8" "https://43.255.224.58/api/test/backend-service/fileDownload?fileId=0000romupdate" --insecure -o $TAR_NAME
	tar -xvpf $TAR_NAME -C /tmp/
	if [ -d "$packge_file" ]
	then  
		echo "down load success,package exist!"
		mv $packge_file/* $LOCAL_DIR

		#opkg remove serial_read_save
		#opkg install serial_read_save*
		rm -rf $packge_file
		rm $TAR_NAME
	else  
    		echo "down load failed"  
	fi  


fi

