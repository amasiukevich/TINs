-- tutorial: https://mika-s.github.io/wireshark/lua/dissector/2017/11/04/creating-a-wireshark-dissector-in-lua-1.html

-- definitions concerning message type
local packet_type_mask = 0xC0
local msg_data = 1
local msg_ack = 2
local msg_error = 3

-- definitions of error codes
local counter_mask = 0x3FFF
local e_bad_source = 1
local e_connection_broken = 2

aaa_protocol = Proto("AAA", "AAA Protocol")

-- how do you deal with data that is not aligned with bytes?
packet_type = ProtoField.uint8( "type", "packet type",nil, base.dec, packet_type_mask)
number = ProtoField.uint16("new", "packet number",nil,  base.dec, counter_mask)


aaa_protocol.fields = {packet_type, number}

local function get_msg_type(code)
    code = code / (2^6)       -- shift right by 6 so that you get int representation of codes (lua 5.2 does not support bitwise operations :C)
    local msg_type = "UNKNOWN"
    if code == msg_data then msg_type = "DATA"
    elseif code == msg_ack then msg_type = "ACK"
    elseif code == msg_error then msg_type = "ERROR" end
    return msg_type
end


local function get_error_codes(num)
    num = num * 4
    num = num / 4       -- obliterate first two bits, lua 5.2 does not support bitwise operations
    local err_code = "UNKNOWN"
    if num == e_bad_source then err_code= "E_SOURCE"
        elseif num == e_connection_broken then err_code = "E_CONNECTION"
    end
    return err_code
end

function aaa_protocol.dissector(buffer, pinfo, tree)
    length = buffer:len()
    if(length == 0) then return end

    pinfo.cols.protocol = aaa_protocol.name
    local subtree = tree:add(aaa_protocol, buffer(), "AAA protocol data")
    local type = buffer(0,1):uint8()
    local typename = get_msg_type(type)
    subtree:add(packet_type, buffer(0,1)):append_text(" (" .. typename .. ") ")

    local counter = buffer(0,2):uint16()
    if(typename == "ERROR") then
        local error_code = get_error_codes(counter)
        subtree:add(number, buffer(0,2)):append_text(" (" .. error_code .. ")" )
    else
        subtree:add(number, buffer(0,2))
    end
    if (typename ~= "ACK" and length > 2) then
        subtree:add(buffer(2, length-2), buffer(2, length-2):string()):append_text(" (payload) ")
    end
end

local udp_port = DissectorTable.get("udp.port")
udp_port:add(5555, aaa_protocol)