/*******************************************************************************
 * Copyright (c) 2012, 2017 IBM Corp.
 *
 * All rights reserved. This program and the accompanying materials
 * are made available under the terms of the Eclipse Public License v1.0
 * and Eclipse Distribution License v1.0 which accompany this distribution. 
 *
 * The Eclipse Public License is available at 
 *   http://www.eclipse.org/legal/epl-v10.html
 * and the Eclipse Distribution License is available at 
 *   http://www.eclipse.org/org/documents/edl-v10.php.
 *
 * Contributors:
 *    Ian Craggs - initial contribution
 *******************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "MQTTClient.h"

#define ADDRESS     "tcp://localhost:1883"
#define CLIENTID    "ExampleClientPub"
#define TOPIC       "topic"
#define PAYLOAD     "Hello World!"
#define QOS         1
#define TIMEOUT     10000L


//#define ADDRESS     "43.255.224.132:1883" //更改此处地址
//#define ADDRESS     "tcp://10.120.115.237:1883"
//#define CLIENTID    "eee38a7cf9eb4c5ba6aa747e48e5e578" //更改此处客户端ID
//#define TOPIC       "sys/user22/eee38a7cf9eb4c5ba6aa747e48e5e578/ecg/upload"  //更改发送的话题
//#define PAYLOAD     "Hello Man, Can you see me ?!" //更改信息内容
//#define QOS         1
//#define TIMEOUT     10000L

int main(int argc, char* argv[])
{
    MQTTClient client;
    char *username= "user22"; //添加的用户名
    char *password = "passw0rd"; //添加的密码
    MQTTClient_connectOptions conn_opts = MQTTClient_connectOptions_initializer;
    MQTTClient_message pubmsg = MQTTClient_message_initializer;
    MQTTClient_deliveryToken token;
    int rc;

    MQTTClient_create(&client, ADDRESS, CLIENTID,
        MQTTCLIENT_PERSISTENCE_NONE, NULL);
    conn_opts.keepAliveInterval = 20;
    conn_opts.cleansession = 1;
    
    conn_opts.username = username; //将用户名写入连接选项中
    conn_opts.password = password;//将密码写入连接选项中

    rc = MQTTClient_connect(client, &conn_opts);
    printf("rc=%d",rc);

    if (rc != MQTTCLIENT_SUCCESS)
    {
        printf("Failed to connect, return code %d\n", rc);
        exit(EXIT_FAILURE);
    }
    pubmsg.payload = PAYLOAD;
    pubmsg.payloadlen = strlen(PAYLOAD);
    pubmsg.qos = QOS;
    pubmsg.retained = 0;
    MQTTClient_publishMessage(client, TOPIC, &pubmsg, &token);
    printf("Waiting for up to %d seconds for publication of %s\n"
            "on topic %s for client with ClientID: %s\n",
            (int)(TIMEOUT/1000), PAYLOAD, TOPIC, CLIENTID);
    rc = MQTTClient_waitForCompletion(client, token, TIMEOUT);
    printf("Message with delivery token %d delivered\n", token);
    MQTTClient_disconnect(client, 10000);
    MQTTClient_destroy(&client);
    return rc;
}
