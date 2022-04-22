/*
 * ModbusTCP.h
 *
 *  Created on: 20 апр. 2022 г.
 *      Author: producer
 */
 /*
#warning "Remove next line, jast for testing"
#define CONFIG_MB_MDNS_IP_RESOLVER	true
// */

#ifndef COMPONENTS_ESP32_SYSTEM_EXTENSIONS_MODBUSTCP_H_
#define COMPONENTS_ESP32_SYSTEM_EXTENSIONS_MODBUSTCP_H_


#include <stdio.h>
#include <stdint.h>
#include "string.h"
#include "esp_log.h"
#include "esp_check.h"
#include "esp_err.h"
#include "esp_system.h"
#include "esp_wifi.h"
#include "esp_event.h"
#include "esp_log.h"
#include "nvs_flash.h"
#include "esp_netif.h"
#include "mdns.h"

#include "ESP32_System.h"

#include "../modbus/mb_common/include/modbus_params.h"  // for modbus parameters structures
#include "mbcontroller.h"
// #include "sdkconfig.h"

#if CONFIG_ESP32_SYSTEM_HW_MODBUS_TCP
#ifndef CONFIG_ESP32_SYSTEM_HW_NETWORK
#error "Networking-extension not activated!"
#endif


#define MB_TCP_PORT                     (CONFIG_FMB_TCP_PORT_DEFAULT)   // TCP port used by example

// The number of parameters that intended to be used in the particular control process
#define MASTER_MAX_CIDS num_device_parameters

// Number of reading of parameters from slave
#define MASTER_MAX_RETRY                (30)

// Timeout to update cid over Modbus
#define UPDATE_CIDS_TIMEOUT_MS          (500)
#define UPDATE_CIDS_TIMEOUT_TICS        (UPDATE_CIDS_TIMEOUT_MS / portTICK_RATE_MS)

// Timeout between polls
#define POLL_TIMEOUT_MS                 (1)
#define POLL_TIMEOUT_TICS               (POLL_TIMEOUT_MS / portTICK_RATE_MS)
#define MB_MDNS_PORT                    (502)

#define MASTER_TAG "MASTER_TEST"

// The macro to get offset for parameter in the appropriate structure
#define HOLD_OFFSET(field) ((uint16_t)(offsetof(holding_reg_params_t, field) + 1))
#define INPUT_OFFSET(field) ((uint16_t)(offsetof(input_reg_params_t, field) + 1))
#define COIL_OFFSET(field) ((uint16_t)(offsetof(coil_reg_params_t, field) + 1))
#define DISCR_OFFSET(field) ((uint16_t)(offsetof(discrete_reg_params_t, field) + 1))
#define STR(fieldname) ((const char*)( fieldname ))

// Options can be used as bit masks or parameter limits
#define OPTS(min_val, max_val, step_val) { .opt1 = min_val, .opt2 = max_val, .opt3 = step_val }

#define MB_ID_BYTE0(id) ((uint8_t)(id))
#define MB_ID_BYTE1(id) ((uint8_t)(((uint16_t)(id) >> 8) & 0xFF))
#define MB_ID_BYTE2(id) ((uint8_t)(((uint32_t)(id) >> 16) & 0xFF))
#define MB_ID_BYTE3(id) ((uint8_t)(((uint32_t)(id) >> 24) & 0xFF))

#define MB_ID2STR(id) MB_ID_BYTE0(id), MB_ID_BYTE1(id), MB_ID_BYTE2(id), MB_ID_BYTE3(id)

#if CONFIG_FMB_CONTROLLER_SLAVE_ID_SUPPORT
#define MB_DEVICE_ID (uint32_t)CONFIG_FMB_CONTROLLER_SLAVE_ID
#else
#define MB_DEVICE_ID (uint32_t)0x00112233
#endif

#define MB_MDNS_INSTANCE(pref) pref"mb_master_tcp"








// next for slave

#define MB_TCP_PORT_NUMBER      (CONFIG_FMB_TCP_PORT_DEFAULT)
#define MB_MDNS_PORT            (502)

// Defines below are used to define register start address for each type of Modbus registers
#define MB_REG_DISCRETE_INPUT_START         (0x0000)
#define MB_REG_COILS_START                  (0x0000)
#define MB_REG_INPUT_START_AREA0            (INPUT_OFFSET(input_data0)) // register offset input area 0
#define MB_REG_INPUT_START_AREA1            (INPUT_OFFSET(input_data4)) // register offset input area 1
#define MB_REG_HOLDING_START_AREA0          (HOLD_OFFSET(holding_data0))
#define MB_REG_HOLDING_START_AREA1          (HOLD_OFFSET(holding_data4))

#define MB_PAR_INFO_GET_TOUT                (10) // Timeout for get parameter info
#define MB_CHAN_DATA_MAX_VAL                (10)
#define MB_CHAN_DATA_OFFSET                 (1.1f)

#define MB_READ_MASK                        (MB_EVENT_INPUT_REG_RD \
                                                | MB_EVENT_HOLDING_REG_RD \
                                                | MB_EVENT_DISCRETE_RD \
                                                | MB_EVENT_COILS_RD)
#define MB_WRITE_MASK                       (MB_EVENT_HOLDING_REG_WR \
                                                | MB_EVENT_COILS_WR)
#define MB_READ_WRITE_MASK                  (MB_READ_MASK | MB_WRITE_MASK) // ToDo: fix

#define SLAVE_TAG "SLAVE_TEST"


// Enumeration of modbus device addresses accessed by master device
// Each address in the table is a index of TCP slave ip address in mb_communication_info_t::tcp_ip_addr table
enum {
    MB_DEVICE_ADDR1 = 1, // Slave address 1
    MB_DEVICE_COUNT
};

// Enumeration of all supported CIDs for device (used in parameter definition table)
enum {
    CID_INP_DATA_0 = 0,
    CID_HOLD_DATA_0,
    CID_INP_DATA_1,
    CID_HOLD_DATA_1,
    CID_INP_DATA_2,
    CID_HOLD_DATA_2,
    CID_RELAY_P1,
    CID_RELAY_P2,
    CID_COUNT
};


namespace Device {
namespace Hardware {

class Modbus_TCP {
public:
	Modbus_TCP();
	virtual ~Modbus_TCP();



/************************************************************************************************************
* 																											*
* 									TCP modbus																*
*  																											*
************************************************************************************************************/



	// Example Data (Object) Dictionary for Modbus parameters:
	// The CID field in the table must be unique.
	// Modbus Slave Addr field defines slave address of the device with correspond parameter.
	// Modbus Reg Type - Type of Modbus register area (Holding register, Input Register and such).
	// Reg Start field defines the start Modbus register number and Reg Size defines the number of registers for the characteristic accordingly.
	// The Instance Offset defines offset in the appropriate parameter structure that will be used as instance to save parameter value.
	// Data Type, Data Size specify type of the characteristic and its data size.
	// Parameter Options field specifies the options that can be used to process parameter value (limits or masks).
	// Access Mode - can be used to implement custom options for processing of characteristic (Read/Write restrictions, factory mode values and etc).
	static const mb_parameter_descriptor_t * device_parameters;/*[] = {  // ToDo: fix
	    // { CID, Param Name, Units, Modbus Slave Addr, Modbus Reg Type, Reg Start, Reg Size, Instance Offset, Data Type, Data Size, Parameter Options, Access Mode}
	    { CID_INP_DATA_0, STR("Data_channel_0"), STR("Volts"), MB_DEVICE_ADDR1, MB_PARAM_INPUT, 0, 2,
	                    INPUT_OFFSET(input_data0), PARAM_TYPE_FLOAT, 4, OPTS( -10, 10, 1 ), PAR_PERMS_READ_WRITE_TRIGGER },
	    { CID_HOLD_DATA_0, STR("Humidity_1"), STR("%rH"), MB_DEVICE_ADDR1, MB_PARAM_HOLDING, 0, 2,
	            HOLD_OFFSET(holding_data0), PARAM_TYPE_FLOAT, 4, OPTS( 0, 100, 1 ), PAR_PERMS_READ_WRITE_TRIGGER },
	    { CID_INP_DATA_1, STR("Temperature_1"), STR("C"), MB_DEVICE_ADDR1, MB_PARAM_INPUT, 2, 2,
	            INPUT_OFFSET(input_data1), PARAM_TYPE_FLOAT, 4, OPTS( -40, 100, 1 ), PAR_PERMS_READ_WRITE_TRIGGER },
	    { CID_HOLD_DATA_1, STR("Humidity_2"), STR("%rH"), MB_DEVICE_ADDR1, MB_PARAM_HOLDING, 2, 2,
	            HOLD_OFFSET(holding_data1), PARAM_TYPE_FLOAT, 4, OPTS( 0, 100, 1 ), PAR_PERMS_READ_WRITE_TRIGGER },
	    { CID_INP_DATA_2, STR("Temperature_2"), STR("C"), MB_DEVICE_ADDR1, MB_PARAM_INPUT, 4, 2,
	            INPUT_OFFSET(input_data2), PARAM_TYPE_FLOAT, 4, OPTS( -40, 100, 1 ), PAR_PERMS_READ_WRITE_TRIGGER },
	    { CID_HOLD_DATA_2, STR("Humidity_3"), STR("%rH"), MB_DEVICE_ADDR1, MB_PARAM_HOLDING, 4, 2,
	            HOLD_OFFSET(holding_data2), PARAM_TYPE_FLOAT, 4, OPTS( 0, 100, 1 ), PAR_PERMS_READ_WRITE_TRIGGER },
	    { CID_RELAY_P1, STR("RelayP1"), STR("on/off"), MB_DEVICE_ADDR1, MB_PARAM_COIL, 0, 8,
	            COIL_OFFSET(coils_port0), PARAM_TYPE_U16, 2, OPTS( BIT1, 0, 0 ), PAR_PERMS_READ_WRITE_TRIGGER },
	    { CID_RELAY_P2, STR("RelayP2"), STR("on/off"), MB_DEVICE_ADDR1, MB_PARAM_COIL, 8, 8,
	            COIL_OFFSET(coils_port1), PARAM_TYPE_U16, 2, OPTS( BIT0, 0, 0 ), PAR_PERMS_READ_WRITE_TRIGGER }
	};
*/  //Allocate and fill in some init function
	// Calculate number of parameters in the table
	static const uint16_t num_device_parameters; // = (sizeof(device_parameters)/sizeof(device_parameters[0])); // fill in same function as above // ToDo: fix

	// This table represents slave IP addresses that correspond to the short address field of the slave in device_parameters structure
	// Modbus TCP stack shall use these addresses to be able to connect and read parameters from slave
	static constexpr char* slave_ip_address_table[MB_DEVICE_COUNT] = {
	#if CONFIG_MB_SLAVE_IP_FROM_STDIN
	    "FROM_STDIN",     // Address corresponds to MB_DEVICE_ADDR1 and set to predefined value by user
	    NULL
	#elif CONFIG_MB_MDNS_IP_RESOLVER
	    NULL,
	    NULL
	#endif
	};

#if CONFIG_MB_SLAVE_IP_FROM_STDIN

// Scan IP address according to IPV settings
static char* master_scan_addr(int* index, char* buffer);
static int master_get_slave_ip_stdin(char** addr_table);

#elif CONFIG_MB_MDNS_IP_RESOLVER

// For Slave
#define MB_ID_BYTE0(id) ((uint8_t)(id))
#define MB_ID_BYTE1(id) ((uint8_t)(((uint16_t)(id) >> 8) & 0xFF))
#define MB_ID_BYTE2(id) ((uint8_t)(((uint32_t)(id) >> 16) & 0xFF))
#define MB_ID_BYTE3(id) ((uint8_t)(((uint32_t)(id) >> 24) & 0xFF))

#define MB_ID2STR(id) MB_ID_BYTE0(id), MB_ID_BYTE1(id), MB_ID_BYTE2(id), MB_ID_BYTE3(id)

#if CONFIG_FMB_CONTROLLER_SLAVE_ID_SUPPORT
#define MB_DEVICE_ID (uint32_t)CONFIG_FMB_CONTROLLER_SLAVE_ID
#endif

#define MB_SLAVE_ADDR (CONFIG_MB_SLAVE_ADDR)

#define MB_MDNS_INSTANCE(pref) pref"mb_slave_tcp"


// convert mac from binary format to string
static inline char* gen_mac_str(const uint8_t* mac, char* pref, char* mac_str); //*1 same, ready
static inline char* gen_id_str(char* service_name, char* slave_id_str);			//*2 same, ready
static inline char* gen_host_name_str(char* service_name, char* name);
static void start_mdns_service(const char* tag);	//*check same with little differencies (no metter, code commented)
static void stop_mdns_service(void);



// For Master
// convert MAC from binary format to string
//static void master_start_mdns_service(); //*check
static char* master_get_slave_ip_str(mdns_ip_addr_t* address, mb_tcp_addr_type_t addr_type);
static esp_err_t master_resolve_slave(const char* name, mdns_result_t* result, char** resolved_ip,
                                        mb_tcp_addr_type_t addr_type);
static int master_create_slave_list(mdns_result_t* results, char** addr_table,
                                        mb_tcp_addr_type_t addr_type);
static void master_destroy_slave_list(char** table);
static int master_query_slave_service(const char * service_name, const char * proto,
                                        mb_tcp_addr_type_t addr_type);
#endif





// modbus tcp master
static void* master_get_param_data(const mb_parameter_descriptor_t* param_descriptor); //**1
static void master_operation_func(void *arg);
static esp_err_t master_init(mb_communication_info_t* comm_info);
static esp_err_t master_destroy(void);
void run_tcp_master(void); // app_main


// modbus tcp slave
static void setup_reg_data(void);
static void slave_operation_func(void *arg);
static esp_err_t slave_init(mb_communication_info_t* comm_info);
static esp_err_t slave_destroy(void);
void run_tcp_slave(void); // app_main

static constexpr portMUX_TYPE param_lock = portMUX_INITIALIZER_UNLOCKED;


// Pre-final
static esp_err_t destroy_services(const char *tag);
static esp_err_t init_services(const char *tag, mb_tcp_addr_type_t ip_addr_type = MB_IPV4);


};

} /* namespace Hardware */
} /* namespace Device */



#endif // CONFIG_ESP32_SYSTEM_HW_MODBUS_TCP
#endif /* COMPONENTS_ESP32_SYSTEM_EXTENSIONS_MODBUSTCP_H_ */














