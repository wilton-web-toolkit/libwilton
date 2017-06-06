/* 
 * File:   wilton_test.c
 * Author: alex
 *
 * Created on May 6, 2016, 9:44 PM
 */

#include "wilton/wilton.h"
#include "wilton/wiltoncall.h"

#include "stdio.h"
#include "stdlib.h"
#include "string.h"

void hello(void* ctx, wilton_Request* req) {
    (void) ctx;
    wilton_Request_send_response(req, "hello\n", 6);
}

void check_err(char* err) {
    if (NULL != err) {
        puts(err);
        exit(1);
    }
}

void test_server() {
    char* err;
    wilton_Server* server;
    const char* server_conf = "{\"tcpPort\": 8080}";

    wilton_HttpPath* path;
    err = wilton_HttpPath_create(&path, "GET", (int) strlen("GET"), "/", (int) strlen("/"), NULL, hello);
    check_err(err);

    err = wilton_Server_create(&server, server_conf, (int) strlen(server_conf), &path, 1);
    check_err(err);
    wilton_HttpPath_destroy(path);

    //getchar();

    err = wilton_Server_stop(server);
    check_err(err);
}

void test_duktape_fail() {
    const char* in = "{\"module\": \"tests/hello_duktapeX\", \"func\": \"hello\", \"args\": []}";
    char* out;
    int out_len;
    char* err = wiltoncall_runscript_duktape(in, strlen(in), &out, &out_len);
    puts(err);
    wilton_free(err);
}

void runScript(const char* in) {
    char* out;
    int out_len;
    char* err = wiltoncall_runscript_duktape(in, strlen(in), &out, &out_len);
    check_err(err);
    wilton_free(out);
}

const char* wilton_config() {
    return "{"
    "  \"defaultScriptEngine\": \"duktape\"," // optional, duktape is default
    "  \"requireJsDirPath\": \"../../wilton-requirejs\","
    "  \"requireJsConfig\": {"
    "    \"waitSeconds\": 0,"
    "    \"enforceDefine\": true,"
    "    \"nodeIdCompat\": true,"
    "    \"baseUrl\": \"../../modules\""
    "  }"
    "}";
}

void test_wiltonjs() {
    const char* config = wilton_config();
    wiltoncall_init(config, strlen(config));
    runScript("{\"module\": \"runWiltonTests\", \"func\": \"main\", \"args\": []}");
    runScript("{\"module\": \"runNodeTests\", \"func\": \"\", \"args\": []}");
    
}

void test_dyload() {
    const char* config = wilton_config();
    wiltoncall_init(config, strlen(config));
    const char* name = "dyload_shared_library";
    const char* data = "{\"path\": \"libwilton_test_module.so\"}";
    char* out = NULL;
    int out_len = 0;
    char* err = wiltoncall(name, strlen(name), data, strlen(data), &out, &out_len);
    if (NULL != err) {
        puts(err);
        wilton_free(err);
    }
    if (out_len > 0) {
        wilton_free(out);
    }
}

int main() {
//    test_server();
//    test_duktape_fail();
    test_wiltonjs();
//    test_dyload();

    return 0;
}

