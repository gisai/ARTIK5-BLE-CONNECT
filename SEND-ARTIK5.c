/***********************************************************************************************//**
 * \file   SEND-ARTIK5.c
 * \brief  Connect with display device and image transmission
 ***************************************************************************************************
 * 
 **************************************************************************************************/


#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <stdbool.h>
#include <errno.h>
#include <signal.h>
#include <inttypes.h>
#include <sys/socket.h>
#include <artik_module.h>
#include <artik_loop.h>
#include <artik_bluetooth.h>
#include "readfile.h"
//#include "image.h"

#define tam_paq 244
#define SPP_SERVICE "4880c12c-fdcb-4077-8920-a450d7f9b907"
#define SPP_CHARACTERISTIC "fec26ec4-6d71-4442-9f81-55bc21d658d6"
#define SCAN_TIME_MILLISECONDS (30*1000)
#define CONNECT_TIME_MILLISECONDS (25*1000)

static artik_bluetooth_module *bt;
static artik_loop_module *loop;
static char addr[18] = "";
static int scan_timeout_id;
static int connect_timeout_id;

unsigned char image[Image_Size];

/**
 * Callback funtion to scan Timeout
 * @param user_data
 */
static void scan_timeout_callback(void *user_data)
{
    fprintf(stderr, "> Device not found. 30s Timeout exceeded.\n");
    bt->stop_scan();
    bt->unset_callback(BT_EVENT_SCAN);
    loop->remove_timeout_callback(scan_timeout_id);
    loop->quit();
}

/**
 * Callback funtion to connect timeout
 * @param user_data
 */
static void connect_timeout_callback(void *user_data)
{
    fprintf(stderr, "> Can't connect to the device. 25s Timeout exceeded. Try again?\n");
    bt->stop_scan();
    bt->unset_callback(BT_EVENT_CONNECT);
    loop->remove_timeout_callback(connect_timeout_id);
    loop->quit();
}

/**
 * Callback funtion to scan procedures
 * @param event
 * @param data
 * @param user_data
 */
static void on_scan(artik_bt_event event, void *data, void *user_data)
{
	artik_bt_device *devices = (artik_bt_device *) data;
	//Compare found devices with target
	if (!strncmp(devices->remote_address, addr, 18)) {
		fprintf(stdout, "> Device found: [%s]\n", devices->remote_address);
		//Disable scan timeout
		loop->remove_timeout_callback(scan_timeout_id);
		//Disable scan
		bt->stop_scan();
		//Set connect timeout
		loop->add_timeout_callback(&connect_timeout_id, CONNECT_TIME_MILLISECONDS, connect_timeout_callback, NULL);
		//Try to connect
		bt->connect(addr);
	}
}

/**
 * Callback funtion to connection procedures
 * @param event
 * @param data
 * @param user_data
 */
static void on_connect(artik_bt_event event, void *data, void *user_data)
{
	artik_bt_device d = *(artik_bt_device *)data;
	//Check if properly connected
	if (d.is_connected){ 
		//Device connected
		fprintf(stdout, "> [%s] is connected\n", d.remote_address);
		//Disable connect timeout
		loop->remove_timeout_callback(connect_timeout_id);
	}else {
		fprintf(stdout, "> [%s] is disconnected\n", d.remote_address);
		//Unexpected disconnection. Abort
		loop->quit();
	}
}

/**
 * Callback funtion to on service resolve procedures
 * @param event
 * @param data
 * @param user_data
 */
static void on_service_resolved(artik_bt_event event, void *data, void *user_data)
{
	//Once connection stablished, services are resolved
	artik_bt_gatt_char_properties prop1;
	unsigned char *b = NULL;
	int i, len;
	int image_pointer=0;
	unsigned char byte[tam_paq];
	
	fprintf(stdout, "> Image Loading...\n");
	//Read image file before trying to notify device
	if(readFile(&image[0])){
		//Abort if error loading image data
		fprintf(stderr, "> Error loading image. Aborting...\n");
		bt->disconnect(addr);
		loop->quit();
	}

	//Retrieve service characteristics 
	if (bt->gatt_get_char_properties(addr, SPP_SERVICE, SPP_CHARACTERISTIC, &prop1) == 0) {
		//If notify property found, start notifying
		if (prop1 & BT_GATT_CHAR_PROPERTY_NOTIFY) {
			fprintf(stdout, "> Notifying device...\n");
			bt->gatt_start_notify(addr, SPP_SERVICE, SPP_CHARACTERISTIC);
			//Start transmision
			fprintf(stdout, "> Start transmission\n");
			while(image_pointer < Image_Size){
				//Build datagram of tam_paq to send
				for(i=image_pointer;i<image_pointer+tam_paq;i++){
					byte[i-image_pointer]=image[i];
				}
				//Send datagram
				bt->gatt_char_write_value(addr, SPP_SERVICE, SPP_CHARACTERISTIC, byte, tam_paq);
				//Increase index
				image_pointer+=tam_paq;
			}
			//Stop notifying, device starts screen update
			bt->gatt_stop_notify(addr, SPP_SERVICE, SPP_CHARACTERISTIC);
			fprintf(stdout,"> Transmission finished!\n");

			loop->quit();
		}
	}
}

/**
 * Set callbacks to BT events
 * Scan -> Connect -> Service_resolved
 */
static void set_user_callbacks(void)
{
	bt->set_callback(BT_EVENT_SCAN, on_scan, NULL);
	bt->set_callback(BT_EVENT_CONNECT, on_connect, NULL);
	bt->set_callback(BT_EVENT_SERVICE_RESOLVED, on_service_resolved, NULL);
}

/**
 * SIGINT Aware callback
 * @param user_data
 * @return true
 */
static int on_signal(void *user_data)
{
	fprintf(stderr, "> SIGINT! - Aborting...\n");
	loop->quit();
	return true;
}

/**
 * Main process
 *
 * Usage: -t <BTADDR>
 * 
 * @param
 * @param
 * @return
 */
int main(int argc, char *argv[])
{
	//Locate -t
	int opt;
	while ((opt = getopt(argc, argv, "t:")) != -1) {
		switch (opt) {
		case 't':
			//Save destination BTADDR
			strncpy(addr, optarg, 18);
			fprintf(stdout, "> target address: %s\n", addr);
			break;
		default:
			fprintf(stderr, ">Call error. Usage: -t <target BDADDR>\n");
			return -1;
		}
	}

	//Initialize
	int signal_id;
	bt = (artik_bluetooth_module *)artik_request_api_module("bluetooth");
	loop = (artik_loop_module *)artik_request_api_module("loop");
	bt->init();
	set_user_callbacks();

	//Start scanning
	fprintf(stdout, "> Start scan\n");
	bt->start_scan();
	loop->add_signal_watch(SIGINT, on_signal, NULL, &signal_id);
	loop->add_timeout_callback(&scan_timeout_id, SCAN_TIME_MILLISECONDS, scan_timeout_callback, NULL);
	//Background loop starts
	loop->run();
	//loop->quit() resumes here
	loop->remove_signal_watch(signal_id);
	bt->gatt_stop_notify(addr, SPP_SERVICE, SPP_CHARACTERISTIC);
	bt->disconnect(addr);
	bt->deinit();
	artik_release_api_module(bt);
	artik_release_api_module(loop);

	return 0;
}