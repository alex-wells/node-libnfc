var nfc = require('../src/build/Release/nfc.node')
console.log('nfc', nfc)

var context = new nfc.Context()
console.log('libnfc version: ' + context.version)
console.log('available devices: ' + (context.devices.length ? '[' + context.devices.join(', ') + ']' : 'none'))

var device = context.open()
console.log('device name: ' + device.name)
console.log('device connstring: ' + device.connstring)

process.on('exit', function () {
    device.close()
})

while (true) {
    var target = device.pollTarget()
    if (!target) {
        continue
    }
    console.log('target: ' + target)
    console.log(' modulationType:', target.modulationType)
    console.log(' baudRate:', target.baudRate)
    console.log(' info:', target.info)
    while (device.isPresent(target)) {
        process.stdout.write('.')
    }
    process.stdout.write('\n')
    console.log('removed')
}
