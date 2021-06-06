# TINs
Simple proxy server for the sensor network

## Build, Test, Run
```
git submodule update --init_logger --recursive
```
```
mkdir build
cd build
cmake ..
make
```
```
ctest --output-on-failure
```
```
./device config/global.json device0
./proxy config/global.json
```
```
curl -X GET 127.0.0.1:4000/device0/info --verbose
curl -X GET 127.0.0.1:4000/device0/file --verbose
curl -X GET 127.0.0.1:4000/device0/file/test1.txt --verbose
curl -X DELETE 127.0.0.1:4000/device0/file/test1.txt --verbose
curl -X PUT 127.0.0.1:4000/device0/file/newfile.txt -H "X-Data: This is a new file." --verbose
```

## Building and using Wireshark plugin
(for Ubuntu)
1. Download Wireshark

```sudo apt-get install wireshark```

2. Move LUA file to directory containing LUA plugins

```
 sudo cp /path/to/aaa-protocol/src/wireshark/tin-aaa.lua /usr/lib/x86_64-linux-gnu/wireshark/plugins/tin-aaa.lua
```

3. Start Wireshark 
```
sudo wireshark
```

4. Open Help -> About Wireshark -> Plugins and you should see "tin-aaa.lua" plugin on the list
