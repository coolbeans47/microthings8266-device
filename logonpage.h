#ifndef logonpage_h
#define logonpage_h

const char LOGON_PAGE[] = 
"<!DOCTYPE html>" 
"<html lang=""en"">" 
"<head>"
"<meta charset=""UTF-8"">"
"<meta name=""viewport"" content=""width=device-width, initial-scale=1.0"">" 
"<meta http-equiv=""X-UA-Compatible"" content=""ie=edge"">" 
"<title>Microthings8266 Setup</title>" 
"</head>"
"<style>"
"form { border: solid 1px darkblue; width: 300px; background-color: rgb(188, 184, 211); padding: 20px; margin: auto; border-radius: 5px;} "
"label { display: block; font-weight: bold; } "
"div { margin-bottom: 10px; } "
"button { padding: 10px; width: 100%;} "
"</style>"
"<body>"
"<form action='/save/'>"
"<div><label for='ssid'>Wifi SSID</label><input type='text' size='30' name='ssid'/></div>"
"<div><label for='password'>Wifi Password</label><input size='30' type='password' name='password'/></div>"
"<button type='Submit'>Save</button>"
"</form>"
"</body></html>"
;



#endif
