 # -- coding: UTF-8 --
import os
import codecs 
import sys
import time
#command = "mosquitto_pub -h 43.255.224.59 -p 8883 -i OPENWRT -t \"sys/userId/deviceId/ecg/upload\" -m \"this for ssl test!!!\" --cafile ca.crt -u admin1 -P passw0rd@ --insecure"
#read the data block
data_file = codecs.open('datablocks2.txt', 'r', 'string_escape')
#with open('datablocks2.txt') as data_file: # 默认模式为‘r’，只读模式
contents = data_file.read() # 读取文件全部内容
#print  contents.rstrip() # rstrip()函数用于删除字符串末的空白
#params 
server_address = " -h 43.255.224.59"
port = " -p 8883"
client_id = " -i OPENWRT"
topic = " -t \"sys/userId/deviceId/ecg/upload\""
playload = contents
send_file = " -f datablocks2.txt"
cafile = " --cafile ca.crt"
user_name = " -u admin1"
password = " -P passw0rd@"
#command
command = "mosquitto_pub " + server_address + port + client_id + topic + send_file + cafile + user_name + password + " --insecure" + " -d"
print "Start upload" + bytes(time.time()) + "s"
temp = os.system(command)
print "upload command return value = " + bytes(temp)
print "End upload " + bytes(time.time()) + "s"
data_file.close() 
print 'close file ...'
