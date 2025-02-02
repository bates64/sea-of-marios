// https://shygoo.net/pj64d/apidoc.php

var socket = new Socket()

var COMMAND_READ_MEMORY = 0
var COMMAND_WRITE_MEMORY = 1

function sendPacket(command, address, data) {
    console.log("Sending " + command)

    const header = new Buffer(1 + 4 + 4)
    header.write[0] = command
    header.writeUInt32BE(address, 1)
    header.writeUInt32BE(data.length, 5)

    const buffer = Buffer.concat([header, data], 1 + 4 + 4 + data.length)

    socket.write(buffer)
}

function handlePacket(command, address, data) {
    console.log("Recieved " + command)

    switch (command) {
    case COMMAND_READ_MEMORY: {
        console.log("Reading memory at vaddr " + address.hex())
        var size = data[3] | (data[2] << 8) | (data[1] << 16) | (data[0] << 24)
        console.log("Memory size: " + size)
        var data = new Buffer(size)
        for (var i = 0; i < size; i++) {
            data[i] = mem.u8[address + i]
        }
        sendPacket(COMMAND_READ_MEMORY, address, data)
        break
    }
    case COMMAND_WRITE_MEMORY: {
        console.log("Writing memory at vaddr " + address.hex())
        for (var i = 0; i < data.length; i++) {
            mem.u8[address + i] = data[i]
        }
        break
    }
    default:
        console.log("Unknown command");
    }
}

// Buffer.prototype.slice ponyfill
function sliceBuffer(buffer, offset) {
    var buf = new Buffer(buffer.length - offset)

    for (var i = offset; i < buffer.length; i++) {
        buf[i - offset] = buffer[i]
    }

    return buf
}


console.log("Connecting...")

socket.connect(65432, "127.0.0.1", function () {
    console.log("Connected to server")
})

var command, address, dataSize, data
socket.on("data", function (buffer) {
    command = buffer[0]
    address = buffer[4] | (buffer[3] << 8) | (buffer[2] << 16) | (buffer[1] << 24)
    dataSize = buffer[8] | (buffer[7] << 8) | (buffer[6] << 16) | (buffer[5] << 24)
    data = sliceBuffer(buffer, 9)
    handlePacket(command, address, data)
})

socket.on("close", function () {
    console.log("Lost connection to server")
    console.log("Restart this script to reconnect")
})
