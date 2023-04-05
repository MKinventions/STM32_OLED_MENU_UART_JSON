/* UART asynchronous example, that uses separate RX and TX tasks

   This example code is in the Public Domain (or CC0 licensed, at your option.)

   Unless required by applicable law or agreed to in writing, this
   software is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR
   CONDITIONS OF ANY KIND, either express or implied.
*/
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_system.h"
#include "esp_log.h"
#include "driver/uart.h"
#include "string.h"
#include "driver/gpio.h"
#include "driver/adc.h"
#include "esp_adc_cal.h"
#include "cJSON.h"
#include "nvs_flash.h"
#include "nvs.h"

static const int RX_BUF_SIZE = 1024;

#define TXD_PIN (GPIO_NUM_4)
#define RXD_PIN (GPIO_NUM_5)

#define POT1 ADC1_CHANNEL_0//GPIO36(ADC1_0)
#define POT2 ADC1_CHANNEL_3//GPIO39(ADC1_3)
#define POT3 ADC1_CHANNEL_6//GPIO34(ADC1_6)
#define POT4 ADC1_CHANNEL_7//GPIO35(ADC1_7)

#define JOY1 ADC1_CHANNEL_4//GPIO32(ADC1_4)
#define JOY2 ADC1_CHANNEL_5//GPIO33(ADC1_5)
#define JOY3 ADC2_CHANNEL_8//GPIO25(ADC2_8)
#define JOY4 ADC2_CHANNEL_9//GPIO26(ADC2_9)

#define BTN1 23
#define BTN2 22
#define BTN3 21
#define BTN4 19

#define LED1 2
#define LED2 15
#define LED3 18
#define LED4 4


int joy_1 = 0;
int joy_2 = 0;
int joy_3 = 0;
int joy_4 = 0;
int joyDir_1 = 0;
int joyDir_2 = 0;
int joyDir_3 = 0;
int joyDir_4 = 0;

int pot_1 = 0;
int pot_2 = 0;
int pot_3 = 0;
int pot_4 = 0;
int potDir_1 = 0;
int potDir_2 = 0;
int potDir_3 = 0;
int potDir_4 = 0;

int sw_1 = 0;
int sw_2 = 0;
int sw_3 = 0;
int sw_4 = 0;

// Define memory handle
nvs_handle_t my_handle;

int32_t read_POT1_VALUE;
int32_t read_POT2_VALUE;
int32_t read_POT3_VALUE;
int32_t read_POT4_VALUE;
int32_t read_POT1DIR_VALUE;
int32_t read_POT2DIR_VALUE;
int32_t read_POT3DIR_VALUE;
int32_t read_POT4DIR_VALUE;

int32_t read_JOY1_VALUE;
int32_t read_JOY2_VALUE;
int32_t read_JOY3_VALUE;
int32_t read_JOY4_VALUE;
int32_t read_JOY1DIR_VALUE;
int32_t read_JOY2DIR_VALUE;
int32_t read_JOY3DIR_VALUE;
int32_t read_JOY4DIR_VALUE;

int32_t read_SW1DIR_VALUE;
int32_t read_SW2DIR_VALUE;
int32_t read_SW3DIR_VALUE;
int32_t read_SW4DIR_VALUE;


int btn1State = 0;
int btn2State = 0;
int btn3State = 0;
int btn4State = 0;
int lastButton1 = 0;
int lastButton2 = 0;
int lastButton3 = 0;
int lastButton4 = 0;
int pot1State = 0;
int pot2State = 0;
int pot3State = 0;
int pot4State = 0;
int joy1State = 0;
int joy2State = 0;
int joy3State = 0;
int joy4State = 0;

void init(void) {
    const uart_config_t uart_config = {
        .baud_rate = 115200,
        .data_bits = UART_DATA_8_BITS,
        .parity = UART_PARITY_DISABLE,
        .stop_bits = UART_STOP_BITS_1,
        .flow_ctrl = UART_HW_FLOWCTRL_DISABLE,
        .source_clk = UART_SCLK_APB,
    };
    // We won't use a buffer for sending data.
    uart_driver_install(UART_NUM_1, RX_BUF_SIZE * 2, 0, 0, NULL, 0);
    uart_param_config(UART_NUM_1, &uart_config);
    uart_set_pin(UART_NUM_1, TXD_PIN, RXD_PIN, UART_PIN_NO_CHANGE, UART_PIN_NO_CHANGE);
}



//static void tx_task(void *arg)
//{
//
//
//
//    while (1) {
//
//        //    	ESP_LOGI(TAG, "Serialize.....");
//            	cJSON *root;
//            	root = cJSON_CreateObject();
//        //    	cJSON_AddStringToObject(root, "version", IDF_VER);
//            	cJSON_AddNumberToObject(root, "btn1", btn1State);
//            	cJSON_AddNumberToObject(root, "btn2", btn2State);
//            	cJSON_AddNumberToObject(root, "btn3", btn3State);
//            	cJSON_AddNumberToObject(root, "btn4", btn4State);
//            	cJSON_AddNumberToObject(root, "pot1", pot1State);
//            	cJSON_AddNumberToObject(root, "pot2", pot2State);
//            	cJSON_AddNumberToObject(root, "pot3", pot3State);
//            	cJSON_AddNumberToObject(root, "pot4", pot4State);
//            	cJSON_AddNumberToObject(root, "joy1", joy1State);
//            	cJSON_AddNumberToObject(root, "joy2", joy2State);
//            	cJSON_AddNumberToObject(root, "joy3", joy3State);
//            	cJSON_AddNumberToObject(root, "joy4", joy4State);
//
//
//        		char *sending_json_string = cJSON_Print(root);
//            	printf("\nTX UART : %s\n", sending_json_string);
//
//        		//    	ESP_LOGI(TAG, "my_json_string\n%s",my_json_string);
//            	cJSON_Delete(root);
//
//           uart_write_bytes(UART_NUM_1, sending_json_string, strlen(sending_json_string));
//    	    	vTaskDelay(100 / portTICK_PERIOD_MS);
//    }
//}


static void rx_task(void *arg)
{
    static const char *RX_TASK_TAG = "RX_TASK";
    esp_log_level_set(RX_TASK_TAG, ESP_LOG_INFO);
    uint8_t* data = (uint8_t*) malloc(RX_BUF_SIZE+1);


    while (1) {
        const int rxBytes = uart_read_bytes(UART_NUM_1, data, RX_BUF_SIZE, 500 / portTICK_RATE_MS);
        if (rxBytes > 0) {
            data[rxBytes] = 0;

            ESP_LOGE(RX_TASK_TAG, "%s", data);
            const char* my_json_string = (const char*)data;

//            printf("UART RX:%s\n", my_json_string);

       //      ESP_LOGI("RX", "Deserialize.....");
             	cJSON *root2 = cJSON_Parse(my_json_string);

//             	if (cJSON_GetObjectItem(root2, "sw1")) {
//             		sw1 = cJSON_GetObjectItem(root2,"sw1")->valuestring;
//             		ESP_LOGI("RX", "sw1=%s",sw1);
//             	}
//             	if (cJSON_GetObjectItem(root2, "sw2")) {
//             		sw2 = cJSON_GetObjectItem(root2,"sw2")->valuestring;
//             		ESP_LOGI("RX", "sw2=%s",sw2);
//             	}


		/***********Switch array start***************************/
				char str3[25];
				char SW_1[10];
				char SW_2[10];
				char SW_3[10];
				char SW_4[10];
				cJSON *array3 = cJSON_GetObjectItem(root2,"button");
				int array3_size = cJSON_GetArraySize(array3);
				for (int i=0;i<array3_size;i++) {
					cJSON *element3 = cJSON_GetArrayItem(array3, i);
					char* switchs = cJSON_Print(element3);
		 //            printf("switchs: %s\n", switchs);
					strcpy(str3, switchs);
		 //            printf("str1: %s\n", str1);

					if (i == 0) { strcpy(SW_1, str3); sw_1 = atoi(SW_1); }
					if (i == 1) { strcpy(SW_2, str3); sw_2 = atoi(SW_2); }
					if (i == 2) { strcpy(SW_3, str3); sw_3 = atoi(SW_3); }
					if (i == 3) { strcpy(SW_4, str3); sw_4 = atoi(SW_4); }

				}

				nvs_set_i32(my_handle, "SW1DIR_VALUE", sw_1);
				nvs_set_i32(my_handle, "SW2DIR_VALUE", sw_2);
				nvs_set_i32(my_handle, "SW3DIR_VALUE", sw_3);
				nvs_set_i32(my_handle, "SW4DIR_VALUE", sw_4);
		 /***********Switch array end***************************/


         /***********joystick array start***************************/
             	char str1[25];
             	char JOY_1[10];
             	char JOY_2[10];
             	char JOY_3[10];
             	char JOY_4[10];
             	char JOYDIR_1[10];
             	char JOYDIR_2[10];
             	char JOYDIR_3[10];
             	char JOYDIR_4[10];

           	cJSON *array1 = cJSON_GetObjectItem(root2,"joysticks");
           	int array1_size = cJSON_GetArraySize(array1);
           	for (int i=0;i<array1_size;i++) {
           		cJSON *element1 = cJSON_GetArrayItem(array1, i);
           		char* joystick = cJSON_Print(element1);
       //            printf("joystick: %s\n", joystick);
                   strcpy(str1, joystick);
       //            printf("str1: %s\n", str1);

                   if (i == 0) { strcpy(JOY_1, str1);    joy_1 = atoi(JOY_1); }
                   if (i == 1) { strcpy(JOYDIR_1, str1); joyDir_1 = atoi(JOYDIR_1); }
                   if (i == 2) { strcpy(JOY_2, str1);    joy_2 = atoi(JOY_2); }
                   if (i == 3) { strcpy(JOYDIR_2, str1); joyDir_2 = atoi(JOYDIR_2); }
                   if (i == 4) { strcpy(JOY_3, str1);    joy_3 = atoi(JOY_3); }
                   if (i == 5) { strcpy(JOYDIR_3, str1); joyDir_3 = atoi(JOYDIR_3); }
                   if (i == 6) { strcpy(JOY_4, str1);    joy_4 = atoi(JOY_4); }
                   if (i == 7) { strcpy(JOYDIR_4, str1); joyDir_4 = atoi(JOYDIR_4); }
           	}

           	nvs_set_i32(my_handle, "JOY1_VALUE", joy_1);
           	nvs_set_i32(my_handle, "JOY2_VALUE", joy_2);
           	nvs_set_i32(my_handle, "JOY3_VALUE", joy_3);
           	nvs_set_i32(my_handle, "JOY4_VALUE", joy_4);
           	nvs_set_i32(my_handle, "JOY1DIR_VALUE", joyDir_1);
           	nvs_set_i32(my_handle, "JOY2DIR_VALUE", joyDir_2);
           	nvs_set_i32(my_handle, "JOY3DIR_VALUE", joyDir_3);
           	nvs_set_i32(my_handle, "JOY4DIR_VALUE", joyDir_4);
       /***********joystick array end***************************/


       /***********potentiometer array start***************************/
             	char str2[25];
             	char POT_1[10];
             	char POT_2[10];
             	char POT_3[10];
             	char POT_4[10];
             	char POTDIR_1[10];
             	char POTDIR_2[10];
             	char POTDIR_3[10];
             	char POTDIR_4[10];
            cJSON *array2 = cJSON_GetObjectItem(root2,"potentiometers");
           	int array2_size = cJSON_GetArraySize(array2);
           	for (int i=0;i<array2_size;i++) {
           		cJSON *element2 = cJSON_GetArrayItem(array2, i);
           		char* potentiometer = cJSON_Print(element2);
       //            printf("potentiometer: %s\n", potentiometer);
                   strcpy(str2, potentiometer);
       //            printf("str1: %s\n", str1);

                   if (i == 0) { strcpy(POT_1, str2);    pot_1 = atoi(POT_1); }
                   if (i == 1) { strcpy(POTDIR_1, str2); potDir_1 = atoi(POTDIR_1); }
                   if (i == 2) { strcpy(POT_2, str2);    pot_2 = atoi(POT_2); }
                   if (i == 3) { strcpy(POTDIR_2, str2); potDir_2 = atoi(POTDIR_2); }
                   if (i == 4) { strcpy(POT_3, str2);    pot_3 = atoi(POT_3); }
                   if (i == 5) { strcpy(POTDIR_3, str2); potDir_3 = atoi(POTDIR_3); }
                   if (i == 6) { strcpy(POT_4, str2);    pot_4 = atoi(POT_4); }
                   if (i == 7) { strcpy(POTDIR_4, str2); potDir_4 = atoi(POTDIR_4); }

           	}

		    // Set data structure for update during the commit
		    // key - "data"
		    // value - "write_data"
//    		    int32_t write_BUTTON_STATE = button_flag;
		    nvs_set_i32(my_handle, "POT1_VALUE", pot_1);
		    nvs_set_i32(my_handle, "POT2_VALUE", pot_2);
		    nvs_set_i32(my_handle, "POT3_VALUE", pot_3);
		    nvs_set_i32(my_handle, "POT4_VALUE", pot_4);
		    nvs_set_i32(my_handle, "POT1DIR_VALUE", potDir_1);
		    nvs_set_i32(my_handle, "POT2DIR_VALUE", potDir_2);
		    nvs_set_i32(my_handle, "POT3DIR_VALUE", potDir_3);
		    nvs_set_i32(my_handle, "POT4DIR_VALUE", potDir_4);
		    // Write any pending changes to non-volatile storage
		    nvs_commit(my_handle);
       /***********potentiometer array end***************************/






//       		ESP_LOGI("RX_UART", "Sw1=%d, Sw2=%d, Sw3=%d, Sw4=%d", sw_1, sw_2, sw_3, sw_4);
//       		ESP_LOGI("RX_UART", "Pot1=%d,Pot1Dir=%d, Pot2=%d,Pot2Dir=%d, Pot3=%d,Pot3Dir=%d, Pot4=%d,Pot4Dir=%d",  pot_1, potDir_1,pot_2, potDir_2,pot_3, potDir_3,pot_4, potDir_4);
//            ESP_LOGI("RX_UART", "Joy1=%d,Joy1Dir=%d, Joy2=%d,Joy2Dir=%d, Joy3=%d,Joy3Dir=%d, Joy4=%d,Joy4Dir=%d",  joy_1, joyDir_1,joy_2, joyDir_2,joy_3, joyDir_3,joy_4, joyDir_4);



//               ESP_LOGI("RX", "sw1:%d,sw2:%d,sw3:%d,sw4:%d, pot1:%d, pot1Dir:%d, joy1:%d, joyDir1:%d, joy2:%d, joyDir2:%d, joy3:%d, joyDir3:%d, joy4:%d, joyDir4:%d",
//            		   sw_1, sw_2, sw_3, sw_4, pot_1, potDir_1, joy_1, joyDir_1, joy_2, joyDir_2, joy_3, joyDir_3, joy_4, joyDir_4);
//             	printf("sw1:%s, sw2:%s\n", sw1, sw2);

        cJSON_Delete(root2);
//        cJSON_free(my_json_string);
        }

    }
    free(data);
    nvs_close(my_handle);


}


static void nvs_data(void *arg){
    // Initialize the NVS partition
    nvs_flash_init();
    // Wait for memory initialization
    vTaskDelay(100 / portTICK_PERIOD_MS);



    // Open storage with a given namespace
    // storage namespase - "storage"
    nvs_open("storage", NVS_READWRITE, &my_handle);

    // Read data
    // key - "data"
    // value - "read_data"
    nvs_get_i32(my_handle, "POT1_VALUE", &read_POT1_VALUE);
    nvs_get_i32(my_handle, "POT2_VALUE", &read_POT2_VALUE);
    nvs_get_i32(my_handle, "POT3_VALUE", &read_POT3_VALUE);
    nvs_get_i32(my_handle, "POT4_VALUE", &read_POT4_VALUE);
    nvs_get_i32(my_handle, "POT1DIR_VALUE", &read_POT1DIR_VALUE);
    nvs_get_i32(my_handle, "POT2DIR_VALUE", &read_POT2DIR_VALUE);
    nvs_get_i32(my_handle, "POT3DIR_VALUE", &read_POT3DIR_VALUE);
    nvs_get_i32(my_handle, "POT4DIR_VALUE", &read_POT4DIR_VALUE);

    nvs_get_i32(my_handle, "JOY1_VALUE", &read_JOY1_VALUE);
    nvs_get_i32(my_handle, "JOY2_VALUE", &read_JOY2_VALUE);
    nvs_get_i32(my_handle, "JOY3_VALUE", &read_JOY3_VALUE);
    nvs_get_i32(my_handle, "JOY4_VALUE", &read_JOY4_VALUE);
    nvs_get_i32(my_handle, "JOY1DIR_VALUE", &read_JOY1DIR_VALUE);
    nvs_get_i32(my_handle, "JOY2DIR_VALUE", &read_JOY2DIR_VALUE);
    nvs_get_i32(my_handle, "JOY3DIR_VALUE", &read_JOY3DIR_VALUE);
    nvs_get_i32(my_handle, "JOY4DIR_VALUE", &read_JOY4DIR_VALUE);

    nvs_get_i32(my_handle, "SW1DIR_VALUE", &read_SW1DIR_VALUE);
    nvs_get_i32(my_handle, "SW2DIR_VALUE", &read_SW2DIR_VALUE);
    nvs_get_i32(my_handle, "SW3DIR_VALUE", &read_SW3DIR_VALUE);
    nvs_get_i32(my_handle, "SW4DIR_VALUE", &read_SW4DIR_VALUE);

//    // Wait for memory initialization
//    vTaskDelay(200 / portTICK_PERIOD_MS);

	while(1){


//	    ESP_LOGW("NVS_POT", "POT1:%d, POT2:%d, POT3:%d, POT4:%d, POT1DIR:%d, POT2DIR:%d, POT2DIR:%d, POT2DIR:%d", read_POT1_VALUE, read_POT2_VALUE, read_POT3_VALUE, read_POT4_VALUE, read_POT1DIR_VALUE, read_POT2DIR_VALUE, read_POT3DIR_VALUE, read_POT4DIR_VALUE);
//	    ESP_LOGW("NVS_JOY", "JOY1:%d, JOY2:%d, JOY3:%d, JOY4:%d, JOY1DIR:%d, JOY2DIR:%d, JOY2DIR:%d, JOY2DIR:%d", read_JOY1_VALUE, read_JOY2_VALUE, read_JOY3_VALUE, read_JOY4_VALUE, read_JOY1DIR_VALUE, read_JOY2DIR_VALUE, read_JOY3DIR_VALUE, read_JOY4DIR_VALUE);
	    ESP_LOGW("NVS_SW", "SW1DIR:%d, SW2DIR:%d, SW3DIR:%d, SW4DIR:%d", read_SW1DIR_VALUE, read_SW2DIR_VALUE, read_SW3DIR_VALUE, read_SW4DIR_VALUE);

	    vTaskDelay(500 / portTICK_PERIOD_MS);

	}
}


//****************POTENTIOMETERS AND JOYSTICK MAPPING**************
int map(int value, int fromLow, int fromHigh, int toLow, int toHigh)
{
    return (value - fromLow) * (toHigh - toLow) / (fromHigh - fromLow) + toLow;
}




static void GPIO_data(void *arg){

	gpio_pad_select_gpio(LED1);
	gpio_set_direction(LED1, GPIO_MODE_OUTPUT);
	gpio_pad_select_gpio(LED2);
	gpio_set_direction(LED2, GPIO_MODE_OUTPUT);
	gpio_pad_select_gpio(LED3);
	gpio_set_direction(LED3, GPIO_MODE_OUTPUT);
	gpio_pad_select_gpio(LED4);
	gpio_set_direction(LED4, GPIO_MODE_OUTPUT);

	gpio_pad_select_gpio(BTN1);
	gpio_set_direction(BTN1, GPIO_MODE_INPUT);
	gpio_pullup_en(BTN1);
	gpio_pad_select_gpio(BTN2);
	gpio_set_direction(BTN2, GPIO_MODE_INPUT);
	gpio_pullup_en(BTN2);
	gpio_pad_select_gpio(BTN3);
	gpio_set_direction(BTN3, GPIO_MODE_INPUT);
	gpio_pullup_en(BTN3);
	gpio_pad_select_gpio(BTN4);
	gpio_set_direction(BTN4, GPIO_MODE_INPUT);
	gpio_pullup_en(BTN4);


		adc1_config_width(ADC_WIDTH_BIT_12);//(12)(0 - 4095)
	//	adc1_config_width(ADC_WIDTH_BIT_11);//(0 - 2047)
//		adc1_config_width(ADC_WIDTH_BIT_10);//(0 - 1023)
	//	adc1_config_width(ADC_WIDTH_BIT_9);// (0 - 511)
	    adc1_config_channel_atten(POT1, ADC_ATTEN_DB_11);   //~150mv - 2450mv(11dB)
	    adc1_config_channel_atten(POT2, ADC_ATTEN_DB_11);   //~150mv - 2450mv(11dB)
	    adc1_config_channel_atten(POT3, ADC_ATTEN_DB_11);   //~150mv - 2450mv(11dB)
	    adc1_config_channel_atten(POT4, ADC_ATTEN_DB_11);   //~150mv - 2450mv(11dB)

	    adc1_config_channel_atten(JOY1, ADC_ATTEN_DB_11);   //~150mv - 2450mv(11dB)
	    adc1_config_channel_atten(JOY2, ADC_ATTEN_DB_11);   //~150mv - 2450mv(11dB)
	    adc2_config_channel_atten(JOY3, ADC_ATTEN_DB_11);   //~150mv - 2450mv(11dB)
	    adc2_config_channel_atten(JOY4, ADC_ATTEN_DB_11);   //~150mv - 2450mv(11dB)


	while(1){
            int button1 = gpio_get_level(BTN1);
            int button2 = gpio_get_level(BTN2);
            int button3 = gpio_get_level(BTN3);
            int button4 = gpio_get_level(BTN4);

            int POTENTIOMETER1_VALUE = adc1_get_raw(POT1);
			int POTENTIOMETER2_VALUE = adc1_get_raw(POT2);
			int POTENTIOMETER3_VALUE = adc1_get_raw(POT3);
			int POTENTIOMETER4_VALUE = adc1_get_raw(POT4);

			int JOYSTICK1_VALUE = adc1_get_raw(JOY1);
			int JOYSTICK2_VALUE = adc1_get_raw(JOY2);
			int JOYSTICK3_VALUE;
			int JOYSTICK4_VALUE;
			adc2_get_raw(JOY3, ADC_WIDTH_BIT_12, &JOYSTICK3_VALUE);
			adc2_get_raw(JOY4, ADC_WIDTH_BIT_12, &JOYSTICK4_VALUE);

           /***********POTENTIOMETER1**********************************/
			     if(read_POT1_VALUE == 180 && read_POT1DIR_VALUE == 1){ pot1State = map(POTENTIOMETER1_VALUE, 0, 4095, 180, 0); }
			else if(read_POT1_VALUE == 180 && read_POT1DIR_VALUE == 0){ pot1State = map(POTENTIOMETER1_VALUE, 0, 4095, 0, 180); }
			else if(read_POT1_VALUE == 255 && read_POT1DIR_VALUE == 1){ pot1State = map(POTENTIOMETER1_VALUE, 0, 4095, 255, 0); }
			else if(read_POT1_VALUE == 255 && read_POT1DIR_VALUE == 0){ pot1State = map(POTENTIOMETER1_VALUE, 0, 4095, 0, 255); }
		   /***********POTENTIOMETER2**********************************/
				 if(read_POT2_VALUE == 180 && read_POT2DIR_VALUE == 1){ pot2State = map(POTENTIOMETER2_VALUE, 0, 4095, 180, 0); }
			else if(read_POT2_VALUE == 180 && read_POT2DIR_VALUE == 0){ pot2State = map(POTENTIOMETER2_VALUE, 0, 4095, 0, 180); }
			else if(read_POT2_VALUE == 255 && read_POT2DIR_VALUE == 1){ pot2State = map(POTENTIOMETER2_VALUE, 0, 4095, 255, 0); }
			else if(read_POT2_VALUE == 255 && read_POT2DIR_VALUE == 0){ pot2State = map(POTENTIOMETER2_VALUE, 0, 4095, 0, 255); }
		   /***********POTENTIOMETER3**********************************/
				 if(read_POT2_VALUE == 180 && read_POT2DIR_VALUE == 1){ pot3State = map(POTENTIOMETER3_VALUE, 0, 4095, 180, 0); }
			else if(read_POT2_VALUE == 180 && read_POT2DIR_VALUE == 0){ pot3State = map(POTENTIOMETER3_VALUE, 0, 4095, 0, 180); }
			else if(read_POT2_VALUE == 255 && read_POT2DIR_VALUE == 1){ pot3State = map(POTENTIOMETER3_VALUE, 0, 4095, 255, 0); }
			else if(read_POT2_VALUE == 255 && read_POT2DIR_VALUE == 0){ pot3State = map(POTENTIOMETER3_VALUE, 0, 4095, 0, 255); }
		   /***********POTENTIOMETER4**********************************/
				 if(read_POT4_VALUE == 180 && read_POT4DIR_VALUE == 1){ pot4State = map(POTENTIOMETER4_VALUE, 0, 4095, 180, 0); }
			else if(read_POT4_VALUE == 180 && read_POT4DIR_VALUE == 0){ pot4State = map(POTENTIOMETER4_VALUE, 0, 4095, 0, 180); }
			else if(read_POT4_VALUE == 255 && read_POT4DIR_VALUE == 1){ pot4State = map(POTENTIOMETER4_VALUE, 0, 4095, 255, 0); }
			else if(read_POT4_VALUE == 255 && read_POT4DIR_VALUE == 0){ pot4State = map(POTENTIOMETER4_VALUE, 0, 4095, 0, 255); }




		   /***********JOYSTICK1**********************************/
				 if(read_JOY1_VALUE == 180 && read_JOY1DIR_VALUE == 1){ joy1State = map(JOYSTICK1_VALUE, 0, 4095, 180, 0); }
			else if(read_JOY1_VALUE == 180 && read_JOY1DIR_VALUE == 0){ joy1State = map(JOYSTICK1_VALUE, 0, 4095, 0, 180); }
			else if(read_JOY1_VALUE == 255 && read_JOY1DIR_VALUE == 1){ joy1State = map(JOYSTICK1_VALUE, 0, 4095, 255, 0); }
			else if(read_JOY1_VALUE == 255 && read_JOY1DIR_VALUE == 0){ joy1State = map(JOYSTICK1_VALUE, 0, 4095, 0, 255); }
		   /***********JOYSTICK2**********************************/
				 if(read_JOY2_VALUE == 180 && read_JOY2DIR_VALUE == 1){ joy2State = map(JOYSTICK2_VALUE, 0, 4095, 180, 0); }
			else if(read_JOY2_VALUE == 180 && read_JOY2DIR_VALUE == 0){ joy2State = map(JOYSTICK2_VALUE, 0, 4095, 0, 180); }
			else if(read_JOY2_VALUE == 255 && read_JOY2DIR_VALUE == 1){ joy2State = map(JOYSTICK2_VALUE, 0, 4095, 255, 0); }
			else if(read_JOY2_VALUE == 255 && read_JOY2DIR_VALUE == 0){ joy2State = map(JOYSTICK2_VALUE, 0, 4095, 0, 255); }
		   /***********JOYSTICK3**********************************/
				 if(read_JOY2_VALUE == 180 && read_JOY2DIR_VALUE == 1){ joy3State = map(JOYSTICK3_VALUE, 0, 4095, 180, 0); }
			else if(read_JOY2_VALUE == 180 && read_JOY2DIR_VALUE == 0){ joy3State = map(JOYSTICK3_VALUE, 0, 4095, 0, 180); }
			else if(read_JOY2_VALUE == 255 && read_JOY2DIR_VALUE == 1){ joy3State = map(JOYSTICK3_VALUE, 0, 4095, 255, 0); }
			else if(read_JOY2_VALUE == 255 && read_JOY2DIR_VALUE == 0){ joy3State = map(JOYSTICK3_VALUE, 0, 4095, 0, 255); }
		   /***********JOYSTICK4**********************************/
				 if(read_JOY4_VALUE == 180 && read_JOY4DIR_VALUE == 1){ joy4State = map(JOYSTICK4_VALUE, 0, 4095, 180, 0); }
			else if(read_JOY4_VALUE == 180 && read_JOY4DIR_VALUE == 0){ joy4State = map(JOYSTICK4_VALUE, 0, 4095, 0, 180); }
			else if(read_JOY4_VALUE == 255 && read_JOY4DIR_VALUE == 1){ joy4State = map(JOYSTICK4_VALUE, 0, 4095, 255, 0); }
			else if(read_JOY4_VALUE == 255 && read_JOY4DIR_VALUE == 0){ pot4State = map(JOYSTICK4_VALUE, 0, 4095, 0, 255); }

//					printf("POT1 : %d, POT2 : %d, POT3 : %d, POT4 : %d, JOY1 : %d, JOY2 : %d, JOY3 : %d, JOY4 : %d\n",
//							pot1State, pot2State, pot3State, pot4State, joy1State, joy2State, joy3State, joy4State);






          //*********SWITCH1******************************************
          if(read_SW1DIR_VALUE == 1){//TOGGLE SWITCH

            if(button1 != lastButton1){
            	lastButton1 = button1;

            	if(lastButton1 == 0){
            		btn1State = (btn1State == 0) ? 1 : 0;
            	}
            }
          }
          else if(read_SW1DIR_VALUE == 0){//PUSH BUTTON
               btn1State = (button1 == 0) ? 1 : 0;
          }

          //*********SWITCH2******************************************
          if(read_SW2DIR_VALUE == 1){//TOGGLE SWITCH

            if(button2 != lastButton2){
            	lastButton2 = button2;

            	if(lastButton2 == 0){
            		btn2State = (btn2State == 0) ? 1 : 0;
            	}
            }
          }
          else if(read_SW2DIR_VALUE == 0){//PUSH BUTTON
               btn2State = (button2 == 0) ? 1 : 0;
          }

          //*********SWITCH3******************************************
          if(read_SW3DIR_VALUE == 1){//TOGGLE SWITCH

            if(button3 != lastButton3){
            	lastButton3 = button3;

            	if(lastButton3 == 0){
            		btn3State = (btn3State == 0) ? 1 : 0;
            	}
            }
          }
          else if(read_SW3DIR_VALUE == 0){//PUSH BUTTON
               btn3State = (button3 == 0) ? 1 : 0;
          }

          //*********SWITCH4******************************************
          if(read_SW4DIR_VALUE == 1){//TOGGLE SWITCH

            if(button4 != lastButton4){
            	lastButton4 = button4;

            	if(lastButton4 == 0){
            		btn4State = (btn4State == 0) ? 1 : 0;
            	}
            }
          }
          else if(read_SW4DIR_VALUE == 0){//PUSH BUTTON
               btn4State = (button4 == 0) ? 1 : 0;
          }

          gpio_set_level(LED1, btn1State);
          gpio_set_level(LED2, btn2State);
          gpio_set_level(LED3, btn3State);
          gpio_set_level(LED4, btn4State);


	    vTaskDelay(100 / portTICK_PERIOD_MS);

	}
}


void app_main(void)
{

    init();
    xTaskCreate(rx_task, "uart_rx_task", 1024*2, NULL, configMAX_PRIORITIES, NULL);
//    xTaskCreate(tx_task, "uart_tx_task", 1024*2, NULL, configMAX_PRIORITIES-1, NULL);
    xTaskCreate(nvs_data, "nvs_data", 1024*2, NULL, configMAX_PRIORITIES, NULL);
    xTaskCreate(GPIO_data, "GPIO_data", 1024*2, NULL, configMAX_PRIORITIES, NULL);
}
