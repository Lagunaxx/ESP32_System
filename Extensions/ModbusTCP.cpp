/*
 * ModbusTCP.cpp
 *
 *  Created on: 20 апр. 2022 г.
 *      Author: producer
 */

#include "ModbusTCP.h"

#if CONFIG_ESP32_SYSTEM_HW_MODBUS_TCP

namespace Device {
namespace Hardware {



Modbus_TCP::Modbus_TCP() {
	// ToDo: fix initializers
}

Modbus_TCP::~Modbus_TCP() {
	// TODO Auto-generated destructor stub
}


/************************************************************************************************************
* 																											*
* 									TCP modbus																*
*  																											*
************************************************************************************************************/


#if CONFIG_MB_SLAVE_IP_FROM_STDIN

// Scan IP address according to IPV settings
char* Modbus_TCP::master_scan_addr(int* index, char* buffer)
{
    char* ip_str = NULL;
    unsigned int a[8] = {0};
    int buf_cnt = 0;
#if !CONFIG_EXAMPLE_CONNECT_IPV6
    buf_cnt = sscanf(buffer, "IP%d="IPSTR, index, &a[0], &a[1], &a[2], &a[3]);
    if (buf_cnt == 5) {
        if (-1 == asprintf(&ip_str, IPSTR, a[0], a[1], a[2], a[3])) {
            abort();
        }
    }
#else
    buf_cnt = sscanf(buffer, "IP%d="IPV6STR, index, &a[0], &a[1], &a[2], &a[3], &a[4], &a[5], &a[6], &a[7]);
    if (buf_cnt == 9) {
        if (-1 == asprintf(&ip_str, IPV6STR, a[0], a[1], a[2], a[3], a[4], a[5], a[6], a[7])) {
            abort();
        }
    }
#endif
    return ip_str;
}

int Modbus_TCP::master_get_slave_ip_stdin(char** addr_table)
{
    char buf[128];
    int index;
    char* ip_str = NULL;
    int buf_cnt = 0;
    int ip_cnt = 0;

    if (!addr_table) {
        return 0;
    }

    ESP_ERROR_CHECK(Network::example_configure_stdin_stdout());
    while(1) {
        if (addr_table[ip_cnt] && strcmp(addr_table[ip_cnt], "FROM_STDIN") == 0) {
            printf("Waiting IP%d from stdin:\r\n", ip_cnt);
            while (fgets(buf, sizeof(buf), stdin) == NULL) {
                fputs(buf, stdout);
            }
            buf_cnt = strlen(buf);
            buf[buf_cnt - 1] = '\0';
            fputc('\n', stdout);
            ip_str = master_scan_addr(&index, buf);
            if (ip_str != NULL) {
                ESP_LOGI(MASTER_TAG, "IP(%d) = [%s] set from stdin.", ip_cnt, ip_str);
                if ((ip_cnt >= MB_DEVICE_COUNT) || (index != ip_cnt)) {
                    addr_table[ip_cnt] = NULL;
                    break;
                }
                addr_table[ip_cnt++] = ip_str;
            } else {
                // End of configuration
                addr_table[ip_cnt++] = NULL;
                break;
            }
        } else {
            if (addr_table[ip_cnt]) {
                ESP_LOGI(MASTER_TAG, "Leave IP(%d) = [%s] set manually.", ip_cnt, addr_table[ip_cnt]);
                ip_cnt++;
            } else {
                ESP_LOGI(MASTER_TAG, "IP(%d) is not set in the table.", ip_cnt);
                break;
            }
        }
    }
    return ip_cnt;
}

#elif CONFIG_MB_MDNS_IP_RESOLVER

// convert MAC from binary format to string
inline char* Modbus_TCP::gen_mac_str(const uint8_t* mac, char* pref, char* mac_str)
{
    sprintf(mac_str, "%s%02X%02X%02X%02X%02X%02X", pref, MAC2STR(mac));
    return mac_str;
}


inline char* Modbus_TCP::gen_id_str(char* service_name, char* slave_id_str)
{
    sprintf(slave_id_str, "%s%02X%02X%02X%02X", service_name, MB_ID2STR(MB_DEVICE_ID));
    return slave_id_str;
}

/*
static void Modbus_TCP::master_start_mdns_service()
{
    char temp_str[32] = {0};
    uint8_t sta_mac[6] = {0};
    ESP_ERROR_CHECK(esp_read_mac(sta_mac, ESP_MAC_WIFI_STA));
    char* hostname = gen_mac_str(sta_mac, MB_MDNS_INSTANCE("")"_", temp_str);
    // initialize mDNS
    ESP_ERROR_CHECK(mdns_init());
    // set mDNS hostname (required if you want to advertise services)
    ESP_ERROR_CHECK(mdns_hostname_set(hostname));
    ESP_LOGI(MASTER_TAG, "mdns hostname set to: [%s]", hostname);

    // set default mDNS instance name
    ESP_ERROR_CHECK(mdns_instance_name_set(MB_MDNS_INSTANCE("esp32_")));

    // structure with TXT records
    mdns_txt_item_t serviceTxtData[] = {
        {"board","esp32"}
    };

    // initialize service
    ESP_ERROR_CHECK(mdns_service_add(MB_MDNS_INSTANCE(""), "_modbus", "_tcp", MB_MDNS_PORT, serviceTxtData, 1));
    // add mac key string text item
    ESP_ERROR_CHECK(mdns_service_txt_item_set("_modbus", "_tcp", "mac", gen_mac_str(sta_mac, "\0", temp_str)));
    // add slave id key txt item
    ESP_ERROR_CHECK( mdns_service_txt_item_set("_modbus", "_tcp", "mb_id", gen_id_str("\0", temp_str)));
}
*/

void Modbus_TCP::start_mdns_service(const char* tag)
{
    char temp_str[32] = {0};
    uint8_t sta_mac[6] = {0};
    ESP_ERROR_CHECK(esp_read_mac(sta_mac, ESP_MAC_WIFI_STA));
    char* hostname = gen_host_name_str(MB_MDNS_INSTANCE(""), temp_str);
    //initialize mDNS
    ESP_ERROR_CHECK(mdns_init());
    //set mDNS hostname (required if you want to advertise services)
    ESP_ERROR_CHECK(mdns_hostname_set(hostname));
    ESP_LOGI(tag, "mdns hostname set to: [%s]", hostname);

    //set default mDNS instance name
    ESP_ERROR_CHECK(mdns_instance_name_set(MB_MDNS_INSTANCE("esp32_")));

    //structure with TXT records
    mdns_txt_item_t serviceTxtData[] = {
        {"board","esp32"}
    };

    // initialize service for Master
//	ESP_ERROR_CHECK(mdns_service_add(MB_MDNS_INSTANCE(""), "_modbus", "_tcp", MB_MDNS_PORT, serviceTxtData, 1));
    //initialize service
    ESP_ERROR_CHECK(mdns_service_add(hostname, "_modbus", "_tcp", MB_MDNS_PORT, serviceTxtData, 1));
    //add mac key string text item
    ESP_ERROR_CHECK(mdns_service_txt_item_set("_modbus", "_tcp", "mac", gen_mac_str(sta_mac, "\0", temp_str)));
    //add slave id key txt item
    ESP_ERROR_CHECK( mdns_service_txt_item_set("_modbus", "_tcp", "mb_id", gen_id_str("\0", temp_str)));
}

char* Modbus_TCP::master_get_slave_ip_str(mdns_ip_addr_t* address, mb_tcp_addr_type_t addr_type)
{
    mdns_ip_addr_t* a = address;
    char* slave_ip_str = NULL;

    while (a) {
        if ((a->addr.type == ESP_IPADDR_TYPE_V6) && (addr_type == MB_IPV6)) {
            if (-1 == asprintf(&slave_ip_str, IPV6STR, IPV62STR(a->addr.u_addr.ip6))) {
                abort();
            }
        } else if ((a->addr.type == ESP_IPADDR_TYPE_V4) && (addr_type == MB_IPV4)) {
            if (-1 == asprintf(&slave_ip_str, IPSTR, IP2STR(&(a->addr.u_addr.ip4)))) {
                abort();
            }
        }
        if (slave_ip_str) {
            break;
        }
        a = a->next;
    }
    return slave_ip_str;
}

esp_err_t Modbus_TCP::master_resolve_slave(const char* name, mdns_result_t* result, char** resolved_ip,
                                        mb_tcp_addr_type_t addr_type)
{
    if (!name || !result) {
        return ESP_ERR_INVALID_ARG;
    }
    mdns_result_t* r = result;
    int t;
    char* slave_ip = NULL;
    for (; r ; r = r->next) {
        if ((r->ip_protocol == MDNS_IP_PROTOCOL_V4) && (addr_type == MB_IPV6)) {
            continue;
        } else if ((r->ip_protocol == MDNS_IP_PROTOCOL_V6) && (addr_type == MB_IPV4)) {
            continue;
        }
        // Check host name for Modbus short address and
        // append it into slave ip address table
        if ((strcmp(r->instance_name, name) == 0) && (r->port == CONFIG_FMB_TCP_PORT_DEFAULT)) {
            printf("  PTR : %s\n", r->instance_name);
            if (r->txt_count) {
                printf("  TXT : [%u] ", r->txt_count);
                for ( t = 0; t < r->txt_count; t++) {
                    printf("%s=%s; ", r->txt[t].key, r->txt[t].value?r->txt[t].value:"NULL");
                }
                printf("\n");
            }
            slave_ip = master_get_slave_ip_str(r->addr, addr_type);
            if (slave_ip) {
                ESP_LOGI(MASTER_TAG, "Resolved slave %s[%s]:%u", r->hostname, slave_ip, r->port);
                *resolved_ip = slave_ip;
                return ESP_OK;
            }
        }
    }
    *resolved_ip = NULL;
    ESP_LOGD(MASTER_TAG, "Fail to resolve slave: %s", name);
    return ESP_ERR_NOT_FOUND;
}

int Modbus_TCP::master_create_slave_list(mdns_result_t* results, char** addr_table,
                                        mb_tcp_addr_type_t addr_type)
{
    if (!results) {
        return -1;
    }
    int i, addr, resolved = 0;
    const mb_parameter_descriptor_t* pdescr = &device_parameters[0];
    char** ip_table = addr_table;
    char slave_name[22] = {0};
    char* slave_ip = NULL;

    for (i = 0; (i < num_device_parameters && pdescr); i++, pdescr++) {
        addr = pdescr->mb_slave_addr;
        if (-1 == sprintf(slave_name, "mb_slave_tcp_%02X", addr)) {
            ESP_LOGI(MASTER_TAG, "Fail to create instance name for index: %d", addr);
            abort();
        }
        if (!ip_table[addr - 1]) {
            esp_err_t err = master_resolve_slave(slave_name, results, &slave_ip, addr_type);
            if (err != ESP_OK) {
                ESP_LOGE(MASTER_TAG, "Index: %d, sl_addr: %d, name:%s, failed to resolve!",
                                        i, addr, slave_name);
                // Set correspond index to NULL indicate host not resolved
                ip_table[addr - 1] = NULL;
                continue;
            }
            ip_table[addr - 1] = slave_ip; //slave_name;
            ESP_LOGI(MASTER_TAG, "Index: %d, sl_addr: %d, name:%s, resolve to IP: [%s]",
                                    i, addr, slave_name, slave_ip);
            resolved++;
        } else {
            ESP_LOGI(MASTER_TAG, "Index: %d, sl_addr: %d, name:%s, set to IP: [%s]",
                                    i, addr, slave_name, ip_table[addr - 1]);
            resolved++;
        }
    }
    return resolved;
}

void Modbus_TCP::master_destroy_slave_list(char** table)
{
    for (int i = 0; ((i < MB_DEVICE_COUNT) && table[i] != NULL); i++) {
        if (table[i]) {
            free(table[i]);
            table[i] = NULL;
        }
    }
}

int Modbus_TCP::master_query_slave_service(const char * service_name, const char * proto,
                                        mb_tcp_addr_type_t addr_type)
{
    ESP_LOGI(MASTER_TAG, "Query PTR: %s.%s.local", service_name, proto);

    mdns_result_t* results = NULL;
    int count = 0;

    esp_err_t err = mdns_query_ptr(service_name, proto, 3000, 20, &results);
    if(err){
        ESP_LOGE(MASTER_TAG, "Query Failed: %s", esp_err_to_name(err));
        return count;
    }
    if(!results){
        ESP_LOGW(MASTER_TAG, "No results found!");
        return count;
    }

    count = master_create_slave_list(results, (char**)slave_ip_address_table, addr_type); // ToDo: fix type

    mdns_query_results_free(results);
    return count;
}
#endif

// The function to get pointer to parameter storage (instance) according to parameter description table
void* Modbus_TCP::master_get_param_data(const mb_parameter_descriptor_t* param_descriptor)
{
    assert(param_descriptor != NULL);
    void* instance_ptr = NULL;
    if (param_descriptor->param_offset != 0) {
       switch(param_descriptor->mb_param_type)
       {
           case MB_PARAM_HOLDING:
               instance_ptr = ((void*)&holding_reg_params + param_descriptor->param_offset - 1);
               break;
           case MB_PARAM_INPUT:
               instance_ptr = ((void*)&input_reg_params + param_descriptor->param_offset - 1);
               break;
           case MB_PARAM_COIL:
               instance_ptr = ((void*)&coil_reg_params + param_descriptor->param_offset - 1);
               break;
           case MB_PARAM_DISCRETE:
               instance_ptr = ((void*)&discrete_reg_params + param_descriptor->param_offset - 1);
               break;
           default:
               instance_ptr = NULL;
               break;
       }
    } else {
        ESP_LOGE(MASTER_TAG, "Wrong parameter offset for CID #%d", param_descriptor->cid);
        assert(instance_ptr != NULL);
    }
    return instance_ptr;
}



// User operation function to read slave values and check alarm
void Modbus_TCP::master_operation_func(void *arg)
{
    esp_err_t err = ESP_OK;
    float value = 0;
    bool alarm_state = false;
    const mb_parameter_descriptor_t* param_descriptor = NULL;

    ESP_LOGI(MASTER_TAG, "Start modbus test...");

    for(uint16_t retry = 0; retry <= MASTER_MAX_RETRY && (!alarm_state); retry++) {
        // Read all found characteristics from slave(s)
        for (uint16_t cid = 0; (err != ESP_ERR_NOT_FOUND) && cid < MASTER_MAX_CIDS; cid++)
        {
            // Get data from parameters description table
            // and use this information to fill the characteristics description table
            // and having all required fields in just one table
            err = mbc_master_get_cid_info(cid, &param_descriptor);
            if ((err != ESP_ERR_NOT_FOUND) && (param_descriptor != NULL)) {
                void* temp_data_ptr = master_get_param_data(param_descriptor);
                assert(temp_data_ptr);
                uint8_t type = 0;
                err = mbc_master_get_parameter(cid, (char*)param_descriptor->param_key,
                                                    (uint8_t*)&value, &type);
                if (err == ESP_OK) {
                    *(float*)temp_data_ptr = value;
                    if ((param_descriptor->mb_param_type == MB_PARAM_HOLDING) ||
                        (param_descriptor->mb_param_type == MB_PARAM_INPUT)) {
                        ESP_LOGI(MASTER_TAG, "Characteristic #%d %s (%s) value = %f (0x%x) read successful.",
                                        param_descriptor->cid,
                                        (char*)param_descriptor->param_key,
                                        (char*)param_descriptor->param_units,
                                        value,
                                        *(uint32_t*)temp_data_ptr);
                        if (((value > param_descriptor->param_opts.max) ||
                            (value < param_descriptor->param_opts.min))) {
                                alarm_state = true;
                                break;
                        }
                    } else {
                        uint16_t state = *(uint16_t*)temp_data_ptr;
                        const char* rw_str = (state & param_descriptor->param_opts.opt1) ? "ON" : "OFF";
                        ESP_LOGI(MASTER_TAG, "Characteristic #%d %s (%s) value = %s (0x%x) read successful.",
                                        param_descriptor->cid,
                                        (char*)param_descriptor->param_key,
                                        (char*)param_descriptor->param_units,
                                        (const char*)rw_str,
                                        *(uint16_t*)temp_data_ptr);
                        if (state & param_descriptor->param_opts.opt1) {
                            alarm_state = true;
                            break;
                        }
                    }
                } else {
                    ESP_LOGE(MASTER_TAG, "Characteristic #%d (%s) read fail, err = %d (%s).",
                                        param_descriptor->cid,
                                        (char*)param_descriptor->param_key,
                                        (int)err,
                                        (char*)esp_err_to_name(err));
                }
                vTaskDelay(POLL_TIMEOUT_TICS); // timeout between polls
            }
        }
        vTaskDelay(UPDATE_CIDS_TIMEOUT_TICS);
    }

    if (alarm_state) {
        ESP_LOGI(MASTER_TAG, "Alarm triggered by cid #%d.",
                                        param_descriptor->cid);
    } else {
        ESP_LOGE(MASTER_TAG, "Alarm is not triggered after %d retries.",
                                        MASTER_MAX_RETRY);
    }
    ESP_LOGI(MASTER_TAG, "Destroy master...");
    vTaskDelay(100);
}





esp_err_t Modbus_TCP::init_services(const char *tag, mb_tcp_addr_type_t ip_addr_type)
{

// For Master
	esp_err_t result = nvs_flash_init();
    if (result == ESP_ERR_NVS_NO_FREE_PAGES || result == ESP_ERR_NVS_NEW_VERSION_FOUND) {
      ESP_ERROR_CHECK(nvs_flash_erase());
      result = nvs_flash_init();
    }
    ESP_RETURN_ON_FALSE((result == ESP_OK), ESP_ERR_INVALID_STATE,
    		tag,
                            "nvs_flash_init fail, returns(0x%x).",
                            (uint32_t)result);
    result = esp_netif_init();
    ESP_RETURN_ON_FALSE((result == ESP_OK), ESP_ERR_INVALID_STATE,
    		tag,
                            "esp_netif_init fail, returns(0x%x).",
                            (uint32_t)result);
    result = esp_event_loop_create_default();
    ESP_RETURN_ON_FALSE((result == ESP_OK), ESP_ERR_INVALID_STATE,
    		tag,
                            "esp_event_loop_create_default fail, returns(0x%x).",
                            (uint32_t)result);
#if CONFIG_MB_MDNS_IP_RESOLVER
    // Start mdns service and register device
    start_mdns_service(MASTER_TAG); // For master
//    start_mdns_service(SLAVE_TAG); // For slave
#endif
    // This helper function configures Wi-Fi or Ethernet, as selected in menuconfig.
    // Read "Establishing Wi-Fi or Ethernet Connection" section in
    // examples/protocols/README.md for more information about this function.
    result = Device::Hardware::Network::example_connect();
    ESP_RETURN_ON_FALSE((result == ESP_OK), ESP_ERR_INVALID_STATE,
    		tag,
                                "example_connect fail, returns(0x%x).",
                                (uint32_t)result);
#if CONFIG_EXAMPLE_CONNECT_WIFI
   result = esp_wifi_set_ps(WIFI_PS_NONE);
   ESP_RETURN_ON_FALSE((result == ESP_OK), ESP_ERR_INVALID_STATE,
                                   tag,
                                   "esp_wifi_set_ps fail, returns(0x%x).",
                                   (uint32_t)result);
#endif

#if CONFIG_MB_MDNS_IP_RESOLVER // next block for master
    int res = 0;
    for (int retry = 0; (res < num_device_parameters) && (retry < 10); retry++) {
        res = master_query_slave_service("_modbus", "_tcp", ip_addr_type);
    }
    if (res < num_device_parameters) {
        ESP_LOGE(MASTER_TAG, "Could not resolve one or more slave IP addresses, resolved: %d out of %d.", res, num_device_parameters );
        ESP_LOGE(MASTER_TAG, "Make sure you configured all slaves according to device parameter table and they alive in the network.");
        return ESP_ERR_NOT_FOUND;
    }
    mdns_free();
#elif CONFIG_MB_SLAVE_IP_FROM_STDIN
    int ip_cnt = master_get_slave_ip_stdin((char **)slave_ip_address_table);  // ToDo: Check usage, may be bug here
    if (ip_cnt) {
        ESP_LOGI(MASTER_TAG, "Configured %d IP addresse(s).", ip_cnt);
    } else {
        ESP_LOGE(MASTER_TAG, "Fail to get IP address from stdin. Continue.");
        return ESP_ERR_NOT_FOUND;
    }
#endif
    return ESP_OK;
}



esp_err_t Modbus_TCP::destroy_services(const char *tag)
{
    esp_err_t err = ESP_OK;
#if CONFIG_MB_MDNS_IP_RESOLVER
    master_destroy_slave_list((char**)slave_ip_address_table);
#endif
    err = Network::example_disconnect();
    ESP_RETURN_ON_FALSE((err == ESP_OK), ESP_ERR_INVALID_STATE,
    								tag,
                                   "example_disconnect fail, returns(0x%x).",
                                   (uint32_t)err);
    err = esp_event_loop_delete_default();
    ESP_RETURN_ON_FALSE((err == ESP_OK), ESP_ERR_INVALID_STATE,
    									tag,
                                       "esp_event_loop_delete_default fail, returns(0x%x).",
                                       (uint32_t)err);
    err = esp_netif_deinit();
    ESP_RETURN_ON_FALSE((err == ESP_OK || err == ESP_ERR_NOT_SUPPORTED), ESP_ERR_INVALID_STATE,
    									tag,
                                        "esp_netif_deinit fail, returns(0x%x).",
                                        (uint32_t)err);
    err = nvs_flash_deinit();
    ESP_RETURN_ON_FALSE((err == ESP_OK), ESP_ERR_INVALID_STATE,
    							tag,
                                "nvs_flash_deinit fail, returns(0x%x).",
                                (uint32_t)err);
#if CONFIG_MB_MDNS_IP_RESOLVER
    stop_mdns_service(); // For slave
#endif
    return err;
}


// Modbus master initialization
esp_err_t Modbus_TCP::master_init(mb_communication_info_t* comm_info)
{
    void* master_handler = NULL;

    esp_err_t err = mbc_master_init_tcp(&master_handler);
    ESP_RETURN_ON_FALSE((master_handler != NULL), ESP_ERR_INVALID_STATE,
                                MASTER_TAG,
                                "mb controller initialization fail.");
    ESP_RETURN_ON_FALSE((err == ESP_OK), ESP_ERR_INVALID_STATE,
                            MASTER_TAG,
                            "mb controller initialization fail, returns(0x%x).",
                            (uint32_t)err);

    err = mbc_master_setup((void*)comm_info);
    ESP_RETURN_ON_FALSE((err == ESP_OK), ESP_ERR_INVALID_STATE,
                            MASTER_TAG,
                            "mb controller setup fail, returns(0x%x).",
                            (uint32_t)err);

    err = mbc_master_set_descriptor(&device_parameters[0], num_device_parameters);
    ESP_RETURN_ON_FALSE((err == ESP_OK), ESP_ERR_INVALID_STATE,
                                MASTER_TAG,
                                "mb controller set descriptor fail, returns(0x%x).",
                                (uint32_t)err);
    ESP_LOGI(MASTER_TAG, "Modbus master stack initialized...");

    err = mbc_master_start();
    ESP_RETURN_ON_FALSE((err == ESP_OK), ESP_ERR_INVALID_STATE,
                            MASTER_TAG,
                            "mb controller start fail, returns(0x%x).",
                            (uint32_t)err);
    vTaskDelay(5);
    return err;
}

esp_err_t Modbus_TCP::master_destroy(void)
{
    esp_err_t err = mbc_master_destroy();
    ESP_RETURN_ON_FALSE((err == ESP_OK), ESP_ERR_INVALID_STATE,
                                MASTER_TAG,
                                "mbc_master_destroy fail, returns(0x%x).",
                                (uint32_t)err);
    ESP_LOGI(MASTER_TAG, "Modbus master stack destroy...");
    return err;
}

// master modbus tcp
void Modbus_TCP::run_tcp_master(void)
{
    mb_tcp_addr_type_t ip_addr_type;
#if !CONFIG_EXAMPLE_CONNECT_IPV6
    ip_addr_type = MB_IPV4;
#else
    ip_addr_type = MB_IPV6;
#endif
    ESP_ERROR_CHECK(init_services(MASTER_TAG, ip_addr_type));

    mb_communication_info_t comm_info = { 0 }; // Fix structure initialization
    comm_info.ip_port = MB_TCP_PORT;
    comm_info.ip_addr_type = ip_addr_type;
    comm_info.ip_mode = MB_MODE_TCP;
    comm_info.ip_addr = (void*)slave_ip_address_table;
    comm_info.ip_netif_ptr = (void*)Network::get_example_netif();

    ESP_ERROR_CHECK(master_init(&comm_info));
    vTaskDelay(50);

    master_operation_func(NULL);
    ESP_ERROR_CHECK(master_destroy());
    ESP_ERROR_CHECK(destroy_services(MASTER_TAG));
}



// File==================== tcp_slave.c



#if CONFIG_MB_MDNS_IP_RESOLVER


inline char* Modbus_TCP::gen_host_name_str(char* service_name, char* name)
{
    sprintf(name, "%s_%02X", service_name, MB_SLAVE_ADDR);
    return name;
}

void Modbus_TCP::stop_mdns_service(void)
{
    mdns_free();
}

#endif

// Set register values into known state
void Modbus_TCP::setup_reg_data(void)
{
    // Define initial state of parameters
    discrete_reg_params.discrete_input0 = 1;
    discrete_reg_params.discrete_input1 = 0;
    discrete_reg_params.discrete_input2 = 1;
    discrete_reg_params.discrete_input3 = 0;
    discrete_reg_params.discrete_input4 = 1;
    discrete_reg_params.discrete_input5 = 0;
    discrete_reg_params.discrete_input6 = 1;
    discrete_reg_params.discrete_input7 = 0;

    holding_reg_params.holding_data0 = 1.34;
    holding_reg_params.holding_data1 = 2.56;
    holding_reg_params.holding_data2 = 3.78;
    holding_reg_params.holding_data3 = 4.90;

    holding_reg_params.holding_data4 = 5.67;
    holding_reg_params.holding_data5 = 6.78;
    holding_reg_params.holding_data6 = 7.79;
    holding_reg_params.holding_data7 = 8.80;
    coil_reg_params.coils_port0 = 0x55;
    coil_reg_params.coils_port1 = 0xAA;

    input_reg_params.input_data0 = 1.12;
    input_reg_params.input_data1 = 2.34;
    input_reg_params.input_data2 = 3.56;
    input_reg_params.input_data3 = 4.78;
    input_reg_params.input_data4 = 1.12;
    input_reg_params.input_data5 = 2.34;
    input_reg_params.input_data6 = 3.56;
    input_reg_params.input_data7 = 4.78;
}

void Modbus_TCP::slave_operation_func(void *arg)
{
    mb_param_info_t reg_info; // keeps the Modbus registers access information

    ESP_LOGI(SLAVE_TAG, "Modbus slave stack initialized.");
    ESP_LOGI(SLAVE_TAG, "Start modbus test...");
    // The cycle below will be terminated when parameter holding_data0
    // incremented each access cycle reaches the CHAN_DATA_MAX_VAL value.
    for(;holding_reg_params.holding_data0 < MB_CHAN_DATA_MAX_VAL;) {
        // Check for read/write events of Modbus master for certain events
        mb_event_group_t event = mbc_slave_check_event(MB_READ_WRITE_MASK);
        const char* rw_str = (event & MB_READ_MASK) ? "READ" : "WRITE";
        // Filter events and process them accordingly
        if(event & (MB_EVENT_HOLDING_REG_WR | MB_EVENT_HOLDING_REG_RD)) {
            // Get parameter information from parameter queue
            ESP_ERROR_CHECK(mbc_slave_get_param_info(&reg_info, MB_PAR_INFO_GET_TOUT));
            ESP_LOGI(SLAVE_TAG, "HOLDING %s (%u us), ADDR:%u, TYPE:%u, INST_ADDR:0x%.4x, SIZE:%u",
                    rw_str,
                    (uint32_t)reg_info.time_stamp,
                    (uint32_t)reg_info.mb_offset,
                    (uint32_t)reg_info.type,
                    (uint32_t)reg_info.address,
                    (uint32_t)reg_info.size);
            if (reg_info.address == (uint8_t*)&holding_reg_params.holding_data0)
            {
                portENTER_CRITICAL((portMUX_TYPE*)&param_lock);
                holding_reg_params.holding_data0 += MB_CHAN_DATA_OFFSET;
                if (holding_reg_params.holding_data0 >= (MB_CHAN_DATA_MAX_VAL - MB_CHAN_DATA_OFFSET)) {
                    coil_reg_params.coils_port1 = 0xFF;
                }
                portEXIT_CRITICAL((portMUX_TYPE*)&param_lock);
            }
        } else if (event & MB_EVENT_INPUT_REG_RD) {
            ESP_ERROR_CHECK(mbc_slave_get_param_info(&reg_info, MB_PAR_INFO_GET_TOUT));
            ESP_LOGI(SLAVE_TAG, "INPUT READ (%u us), ADDR:%u, TYPE:%u, INST_ADDR:0x%.4x, SIZE:%u",
                    (uint32_t)reg_info.time_stamp,
                    (uint32_t)reg_info.mb_offset,
                    (uint32_t)reg_info.type,
                    (uint32_t)reg_info.address,
                    (uint32_t)reg_info.size);
        } else if (event & MB_EVENT_DISCRETE_RD) {
            ESP_ERROR_CHECK(mbc_slave_get_param_info(&reg_info, MB_PAR_INFO_GET_TOUT));
            ESP_LOGI(SLAVE_TAG, "DISCRETE READ (%u us): ADDR:%u, TYPE:%u, INST_ADDR:0x%.4x, SIZE:%u",
                                (uint32_t)reg_info.time_stamp,
                                (uint32_t)reg_info.mb_offset,
                                (uint32_t)reg_info.type,
                                (uint32_t)reg_info.address,
                                (uint32_t)reg_info.size);
        } else if (event & (MB_EVENT_COILS_RD | MB_EVENT_COILS_WR)) {
            ESP_ERROR_CHECK(mbc_slave_get_param_info(&reg_info, MB_PAR_INFO_GET_TOUT));
            ESP_LOGI(SLAVE_TAG, "COILS %s (%u us), ADDR:%u, TYPE:%u, INST_ADDR:0x%.4x, SIZE:%u",
                                rw_str,
                                (uint32_t)reg_info.time_stamp,
                                (uint32_t)reg_info.mb_offset,
                                (uint32_t)reg_info.type,
                                (uint32_t)reg_info.address,
                                (uint32_t)reg_info.size);
            if (coil_reg_params.coils_port1 == 0xFF) break;
        }
    }
    // Destroy of Modbus controller on alarm
    ESP_LOGI(SLAVE_TAG,"Modbus controller destroyed.");
    vTaskDelay(100);
}


// Modbus slave initialization
esp_err_t Modbus_TCP::slave_init(mb_communication_info_t* comm_info)
{
    mb_register_area_descriptor_t reg_area; // Modbus register area descriptor structure

    void* slave_handler = NULL;

    // Initialization of Modbus controller
    esp_err_t err = mbc_slave_init_tcp(&slave_handler);
    ESP_RETURN_ON_FALSE((err == ESP_OK && slave_handler != NULL), ESP_ERR_INVALID_STATE,
                                SLAVE_TAG,
                                "mb controller initialization fail.");

    comm_info->ip_addr = NULL; // Bind to any address
    comm_info->ip_netif_ptr = (void*)Network::get_example_netif();

    // Setup communication parameters and start stack
    err = mbc_slave_setup((void*)comm_info);
    ESP_RETURN_ON_FALSE((err == ESP_OK), ESP_ERR_INVALID_STATE,
                                        SLAVE_TAG,
                                        "mbc_slave_setup fail, returns(0x%x).",
                                        (uint32_t)err);

    // The code below initializes Modbus register area descriptors
    // for Modbus Holding Registers, Input Registers, Coils and Discrete Inputs
    // Initialization should be done for each supported Modbus register area according to register map.
    // When external master trying to access the register in the area that is not initialized
    // by mbc_slave_set_descriptor() API call then Modbus stack
    // will send exception response for this register area.
    reg_area.type = MB_PARAM_HOLDING; // Set type of register area
    reg_area.start_offset = MB_REG_HOLDING_START_AREA0; // Offset of register area in Modbus protocol
    reg_area.address = (void*)&holding_reg_params.holding_data0; // Set pointer to storage instance
    reg_area.size = sizeof(float) << 2; // Set the size of register storage instance
    err = mbc_slave_set_descriptor(reg_area);
    ESP_RETURN_ON_FALSE((err == ESP_OK), ESP_ERR_INVALID_STATE,
                                    SLAVE_TAG,
                                    "mbc_slave_set_descriptor fail, returns(0x%x).",
                                    (uint32_t)err);

    reg_area.type = MB_PARAM_HOLDING; // Set type of register area
    reg_area.start_offset = MB_REG_HOLDING_START_AREA1; // Offset of register area in Modbus protocol
    reg_area.address = (void*)&holding_reg_params.holding_data4; // Set pointer to storage instance
    reg_area.size = sizeof(float) << 2; // Set the size of register storage instance
    err = mbc_slave_set_descriptor(reg_area);
    ESP_RETURN_ON_FALSE((err == ESP_OK), ESP_ERR_INVALID_STATE,
                                    SLAVE_TAG,
                                    "mbc_slave_set_descriptor fail, returns(0x%x).",
                                    (uint32_t)err);

    // Initialization of Input Registers area
    reg_area.type = MB_PARAM_INPUT;
    reg_area.start_offset = MB_REG_INPUT_START_AREA0;
    reg_area.address = (void*)&input_reg_params.input_data0;
    reg_area.size = sizeof(float) << 2;
    err = mbc_slave_set_descriptor(reg_area);
    ESP_RETURN_ON_FALSE((err == ESP_OK), ESP_ERR_INVALID_STATE,
                                        SLAVE_TAG,
                                        "mbc_slave_set_descriptor fail, returns(0x%x).",
                                        (uint32_t)err);
    reg_area.type = MB_PARAM_INPUT;
    reg_area.start_offset = MB_REG_INPUT_START_AREA1;
    reg_area.address = (void*)&input_reg_params.input_data4;
    reg_area.size = sizeof(float) << 2;
    err = mbc_slave_set_descriptor(reg_area);
    ESP_RETURN_ON_FALSE((err == ESP_OK), ESP_ERR_INVALID_STATE,
                                        SLAVE_TAG,
                                        "mbc_slave_set_descriptor fail, returns(0x%x).",
                                        (uint32_t)err);

    // Initialization of Coils register area
    reg_area.type = MB_PARAM_COIL;
    reg_area.start_offset = MB_REG_COILS_START;
    reg_area.address = (void*)&coil_reg_params;
    reg_area.size = sizeof(coil_reg_params);
    err = mbc_slave_set_descriptor(reg_area);
    ESP_RETURN_ON_FALSE((err == ESP_OK), ESP_ERR_INVALID_STATE,
                                    SLAVE_TAG,
                                    "mbc_slave_set_descriptor fail, returns(0x%x).",
                                    (uint32_t)err);

    // Initialization of Discrete Inputs register area
    reg_area.type = MB_PARAM_DISCRETE;
    reg_area.start_offset = MB_REG_DISCRETE_INPUT_START;
    reg_area.address = (void*)&discrete_reg_params;
    reg_area.size = sizeof(discrete_reg_params);
    err = mbc_slave_set_descriptor(reg_area);
    ESP_RETURN_ON_FALSE((err == ESP_OK), ESP_ERR_INVALID_STATE,
                                    SLAVE_TAG,
                                    "mbc_slave_set_descriptor fail, returns(0x%x).",
                                    (uint32_t)err);

    // Set values into known state
    setup_reg_data();

    // Starts of modbus controller and stack
    err = mbc_slave_start();
    ESP_RETURN_ON_FALSE((err == ESP_OK), ESP_ERR_INVALID_STATE,
                                        SLAVE_TAG,
                                        "mbc_slave_start fail, returns(0x%x).",
                                        (uint32_t)err);
    vTaskDelay(5);
    return err;
}

esp_err_t Modbus_TCP::slave_destroy(void)
{
    esp_err_t err = mbc_slave_destroy();
    ESP_RETURN_ON_FALSE((err == ESP_OK), ESP_ERR_INVALID_STATE,
                                SLAVE_TAG,
                                "mbc_slave_destroy fail, returns(0x%x).",
                                (uint32_t)err);
    return err;
}

// An example application of Modbus slave. It is based on freemodbus stack.
// See deviceparams.h file for more information about assigned Modbus parameters.
// These parameters can be accessed from main application and also can be changed
// by external Modbus master host.
void Modbus_TCP::run_tcp_slave(void)
{
    ESP_ERROR_CHECK(init_services(SLAVE_TAG));

    // Set UART log level
    esp_log_level_set(SLAVE_TAG, ESP_LOG_INFO);

    mb_communication_info_t comm_info = (mb_communication_info_t){ 0 };

#if !CONFIG_EXAMPLE_CONNECT_IPV6
    comm_info.ip_addr_type = MB_IPV4;
#else
    comm_info.ip_addr_type = MB_IPV6;
#endif
    comm_info.ip_mode = MB_MODE_TCP;

    comm_info.ip_port = MB_TCP_PORT_NUMBER;
    ESP_ERROR_CHECK(slave_init(&comm_info));

    // The Modbus slave logic is located in this function (user handling of Modbus)
    slave_operation_func(NULL);

    ESP_ERROR_CHECK(slave_destroy());
    ESP_ERROR_CHECK(destroy_services(SLAVE_TAG));
}









} /* namespace Hardware */
} /* namespace Device */


#endif // CONFIG_ESP32_SYSTEM_HW_MODBUS_TCP
