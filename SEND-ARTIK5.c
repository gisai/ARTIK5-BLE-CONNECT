

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
#include "image.h"

#define SPP_SERVICE "4880c12c-fdcb-4077-8920-a450d7f9b907"
#define SPP_DATA_CHAR "fec26ec4-6d71-4442-9f81-55bc21d658d6"


static artik_bluetooth_module *bt;
static artik_loop_module *loop_main;

static char remote_address[]="00:0B:57:0B:DE:21";



static int uninit(void *user_data)
{
    fprintf(stdout, "<SPP>: Process cancel\n");
    loop_main->quit();
    return true;
}

void callback_on_spp_connect(artik_bt_event event,
    void *data, void *user_data)
{
    fprintf(stdout, "<SPP>: %s\n", __func__);
}
void callback_on_spp_release(artik_bt_event event,
    void *data, void *user_data)
{
    fprintf(stdout, "<SPP>: %s\n", __func__);
}
void callback_on_spp_disconnect(artik_bt_event event,
    void *data, void *user_data)
{
    fprintf(stdout, "<SPP>: %s\n", __func__);
}

static artik_error set_callback(void)
{
    artik_error ret = S_OK;
    artik_bt_callback_property callback_property[] = {
        {BT_EVENT_SPP_CONNECT, callback_on_spp_connect, NULL},
        {BT_EVENT_SPP_RELEASE, callback_on_spp_release, NULL},
        {BT_EVENT_SPP_DISCONNECT, callback_on_spp_disconnect, NULL},
    };
    ret = bt->set_callbacks(callback_property, 4);
    return ret;
}

static artik_error spp_profile_register(void)
{
    artik_error ret = S_OK;
    artik_bt_spp_profile_option profile_option;
    profile_option.name = "Artik SPP Loopback";
    profile_option.service = "spp char loopback";
    profile_option.role = "client";
    profile_option.channel = 22;
    profile_option.PSM = 3;
    profile_option.require_authentication = 1;
    profile_option.auto_connect = 1;
    profile_option.version = 10;
    profile_option.features = 20;
    ret = bt->spp_register_profile(&profile_option);
    return ret;
}

void notifyoff(){
    bt->gatt_stop_notify(remote_address, SPP_SERVICE, SPP_DATA_CHAR);
    bt->disconnect(remote_address);
    bt->deinit();
    fprintf(stderr, "\nNOTIFY OFF\n");
}



int main(void){

	 artik_error ret = S_OK;

	 unsigned char *b = NULL;
	 int i, len;
	 int timeout_id = 0;
	 unsigned char byte[20];
	 int image_pointer=0;





	 if (!artik_is_module_available(ARTIK_MODULE_BLUETOOTH)) {
	       fprintf(stderr, "<SPP>: Bluetooth module not available!\n");
	       goto loop_quit;
	  }

	    bt = (artik_bluetooth_module *)artik_request_api_module("bluetooth");
	    loop_main = (artik_loop_module *)artik_request_api_module("loop");

	    if (!bt || !loop_main)
	        goto loop_quit;

	    bt->init();

	    ret = spp_profile_register();
	    if (ret != S_OK) {
	        fprintf(stdout, "<SPP>: SPP register error!\n");
	        goto spp_quit;
	    }
	    fprintf(stdout, "<SPP>: SPP register profile success!\n");

	    ret = set_callback();
	    if (ret != S_OK) {
	        fprintf(stdout, "<SPP>: SPP set callback error!\n");
	        goto spp_quit;
	    }
	    fprintf(stdout, "<SPP>: SPP set callback success!\n");

	    ret = bt->start_bond(remote_address);
	    if (ret != S_OK)
	        goto spp_quit;
	    fprintf(stdout, "<SPP>: SPP paired success!\n");

	    ret = bt->connect(remote_address);
	    if (ret != S_OK)
	        goto spp_quit;

	    bt->gatt_start_notify(remote_address, SPP_SERVICE, SPP_DATA_CHAR);


	    while(image_pointer < Image_Size){

	    	for(i=image_pointer;i<image_pointer+20;i++){
	    		byte[i-image_pointer]=image[i];
	    	}
	    	bt->gatt_char_write_value(remote_address, SPP_SERVICE, SPP_DATA_CHAR,
	    			byte, 20);

	    	image_pointer+=20;
	    }

	    //loop_main->add_timeout_callback(&timeout_id, 30000, notifyoff, (void*)loop_main);

	    notifyoff();




	    loop_main->add_signal_watch(SIGINT, uninit, NULL, NULL);
	    loop_main->run();

	 spp_quit:
	     bt->spp_unregister_profile();
	     bt->unset_callback(BT_EVENT_SCAN);
	     bt->unset_callback(BT_EVENT_SPP_CONNECT);
	     bt->unset_callback(BT_EVENT_SPP_RELEASE);
	     bt->unset_callback(BT_EVENT_SPP_DISCONNECT);
	     bt->disconnect(remote_address);
	     fprintf(stdout, "<SPP>: SPP quit!\n");
	 loop_quit:
	     if (bt) {
	         bt->deinit();
	         artik_release_api_module(bt);
	     }
	     if (loop_main)
	         artik_release_api_module(loop_main);
	     fprintf(stdout, "<SPP>: Loop quit!\n");
	     return S_OK;

}
