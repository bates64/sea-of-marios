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
        var size = data[0] | (data[1] << 8) | (data[2] << 16) | (data[3] << 24)
        var data = new Buffer(size)
        while (size--) {
            data[size] = mem.u8[vaddr + size]
        }
        sendPacket(COMMAND_READ_MEMORY, address, data)
        break
    }
    case COMMAND_WRITE_MEMORY: {
        console.log("Writing memory at vaddr " + address.hex())
        for (var i = 0; i < data.length; i++) {
            mem.u8[vaddr + i] = data[i]
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

socket.connect({ port: 65432 }, function () {
    console.log("Connected to server")
})

var command, address, dataSize, data, isBuffering = false
socket.on("data", function (buffer) {
    if (!isBuffering) {
        command = buffer[0]
        address = buffer[1] | (buffer[2] << 8) | (buffer[3] << 16) | (buffer[4] << 24)
        dataSize = buffer[5] | (buffer[6] << 8) | (buffer[7] << 16) | (buffer[8] << 24)
        data = sliceBuffer(buffer, 9)

        isBuffering = true
    } else {
        var buf = new Buffer(data.length + buffer.length)

        for (var i = 0; i < data.length; i++) {
            buf[i] = data[i]
        }

        for (var i = 0; i < buffer.length; i++) {
            buf[data.length + i] = buffer[i]
        }

        data = buf
    }

    if (data.length >= dataSize) {
        isBuffering = false
        handlePacket(command, address, data)
    }
})

socket.on("close", function () {
    console.log("Lost connection to server")
    console.log("Restart this script to reconnect")
})
