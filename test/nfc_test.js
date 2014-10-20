var nfc = require('../src/build/Release/nfc.node')
console.log('nfc', nfc)

var context = nfc.Context()
console.log('libnfc version: ' + context.version)
console.log('available devices: ' + (context.devices.length ? '[' + context.devices.join(', ') + ']' : 'none'))

var device = context.open()
console.log('device name: ' + device.name)
console.log('device connstring: ' + device.connstring)
device.close()

while (true) {
    var x = []
}
