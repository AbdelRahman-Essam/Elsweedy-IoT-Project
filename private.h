/*
 *            Errors Log
 * *********************************
 * Problem: ISR Comes out multiple times:
 * Sol:     created a delay of 50ms between two counts, Counts outside the interrupt
 * *********************************
 * Problem: if code stuck it wont count
 * Sol:     make array of readings and compare their millis
 * Sol++:   call the count func in the stuck loop
 * *********************************
 * Problem: ISR wdt
 * Cause:   the empty loop in the client handle
 * sol:     make delay inside to give time for isr to be executed
 * *********************************
 * Problem: 
 * 
 * 
 */
#define LENGTH(x) (strlen(x) + 1)   // length of char string

#define htmlConfigurationString "<!DOCTYPE html>"\
  "<html lang=\"en\">"\
  "<head>"\
  "<meta charset=\"UTF-8\">"\
  "<meta http-equiv=\"X-UA-Compatible\" content=\"IE=edge\">"\
  "<meta name=\"viewport\" content=\"width=device-width, initial-scale=1.0\">"\
  "<title>Home page</title>"\
  "<style>"\
  "*{margin: 0px;}"\
  "body {"\
  "background-color: rgb(34, 45, 57);"\
  "font-family: 'Calibri'}"\
  "form {"\
  "position: absolute;"\
  "top: 50%;"\
  "left: 50%;"\
  "transform: translateX(-50%) translateY(-50%);"\
  "text-align: center;"\
  "border-radius: 25px;}"\
  "input[type=\"text\"],"\
  "input[type=\"password\"],"\
  "textarea {"\
  "border: 3px solid #0AD666;"\
  "size: 100;"\
  "margin: 10px auto;"\
  "display: block;"\
  "padding: 5px;}"\
  "input[type=\"submit\"] {"\
  "border: 0;"\
  "background-color: #0AD666;"\
  "color: white;"\
  "padding: 5px 50px;"\
  "margin: 0px auto 10px auto;"\
  "font-size: 15px;"\
  "border-radius: 5px;}"\
  ".NavBar {"\
  "background-color: #0AD666;"\
  "padding: 20px 10px;}"\
  ".NavBar h1 {"\
  "color: rgb(34, 45, 57);"\
  "font-size: 25px;}"\
  "h1 {"\
  "color: #0AD666;"\
  "font-size: 25px;}"\
  "#footer {"\
  "width: 100%;"\
  "position: fixed;"\
  "bottom: 0;"\
  "padding: 10px;"\
  "background-color: white;"\
  "color: rgb(34, 45, 57);"\
  "text-align: center;}"\
  "</style>"\
  "</head>"\
  "<body>"\
  "<div class=\"NavBar\"><h1>Elswedey Electric</h1></div>"\
  "<form>"\
  "<h1>Wifi Configuration</h1>"\
  "<input size=30 type=\"text\" name=\"ssid\" placeholder=\"SSID\" required>"\
  "<input size=30 type=\"password\" name=\"pass\" placeholder=\"Password\" required>"\
  "<input type=\"submit\" value=\"Send\">"\
  "</form>"\
  "<section id=\"footer\">Powered By EME IH</section>"\
  "</body></html>"

const char * root_ca=\
"-----BEGIN CERTIFICATE-----\n" \
"MIIDujCCAqKgAwIBAgILBAAAAAABD4Ym5g0wDQYJKoZIhvcNAQEFBQAwTDEgMB4G\n" \
"A1UECxMXR2xvYmFsU2lnbiBSb290IENBIC0gUjIxEzARBgNVBAoTCkdsb2JhbFNp\n" \
"Z24xEzARBgNVBAMTCkdsb2JhbFNpZ24wHhcNMDYxMjE1MDgwMDAwWhcNMjExMjE1\n" \
"MDgwMDAwWjBMMSAwHgYDVQQLExdHbG9iYWxTaWduIFJvb3QgQ0EgLSBSMjETMBEG\n" \
"A1UEChMKR2xvYmFsU2lnbjETMBEGA1UEAxMKR2xvYmFsU2lnbjCCASIwDQYJKoZI\n" \
"hvcNAQEBBQADggEPADCCAQoCggEBAKbPJA6+Lm8omUVCxKs+IVSbC9N/hHD6ErPL\n" \
"v4dfxn+G07IwXNb9rfF73OX4YJYJkhD10FPe+3t+c4isUoh7SqbKSaZeqKeMWhG8\n" \
"eoLrvozps6yWJQeXSpkqBy+0Hne/ig+1AnwblrjFuTosvNYSuetZfeLQBoZfXklq\n" \
"tTleiDTsvHgMCJiEbKjNS7SgfQx5TfC4LcshytVsW33hoCmEofnTlEnLJGKRILzd\n" \
"C9XZzPnqJworc5HGnRusyMvo4KD0L5CLTfuwNhv2GXqF4G3yYROIXJ/gkwpRl4pa\n" \
"zq+r1feqCapgvdzZX99yqWATXgAByUr6P6TqBwMhAo6CygPCm48CAwEAAaOBnDCB\n" \
"mTAOBgNVHQ8BAf8EBAMCAQYwDwYDVR0TAQH/BAUwAwEB/zAdBgNVHQ4EFgQUm+IH\n" \
"V2ccHsBqBt5ZtJot39wZhi4wNgYDVR0fBC8wLTAroCmgJ4YlaHR0cDovL2NybC5n\n" \
"bG9iYWxzaWduLm5ldC9yb290LXIyLmNybDAfBgNVHSMEGDAWgBSb4gdXZxwewGoG\n" \
"3lm0mi3f3BmGLjANBgkqhkiG9w0BAQUFAAOCAQEAmYFThxxol4aR7OBKuEQLq4Gs\n" \
"J0/WwbgcQ3izDJr86iw8bmEbTUsp9Z8FHSbBuOmDAGJFtqkIk7mpM0sYmsL4h4hO\n" \
"291xNBrBVNpGP+DTKqttVCL1OmLNIG+6KYnX3ZHu01yiPqFbQfXf5WRDLenVOavS\n" \
"ot+3i9DAgBkcRcAtjOj4LaR0VknFBbVPFd5uRHg5h6h+u/N5GJG79G+dwfCMNYxd\n" \
"AfvDbbnvRG15RjF+Cv6pgsH/76tuIMRQyV+dTZsXjAzlAcmgQWpzU/qlULRuJQ/7\n" \
"TBj0/VLZjmmx6BEP3ojY+x1J96relc8geMJgEtslQIxq/H5COEBkEveegeGTLg==\n" \
"-----END CERTIFICATE-----\n";



String header;

String ssid;
String password;

int SensorValue = 0;

bool exit_but = false;
int count =0;
int ResetFlag = 0;

uint32_t now_millis=0;
uint32_t prev_millis=0;
uint32_t reset_prev_millis = 0;














