# TINs
Simple proxy server for the sensor network

## Build, Test, Run
```
git submodule update --init --recursive
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
./device path/to/config.json deviceID
./proxy path/to/config.json
```
```
curl -X GET 127.0.0.1:4000/device0/info -H "X-Test: asdf" --verbose
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
