var nfc = require('../dist/nfc');

console.log('libnfc version: ' + nfc.version)
console.log('available devices: ' + (nfc.devices.length ? '[' + nfc.devices.join(', ') + ']' : 'none'))

nfc.open().then(function (device) {
    console.log('device name: ' + device.name);
    console.log('device connstring: ' + device.connstring);

    process.on('exit', function () {
        device.close();
    });

    device.pollTarget(1000).then(function (target) {
        console.log('target: ' + target);
        console.log(' modulationType:', target.modulationType);
        console.log(' baudRate:', target.baudRate);
        console.log(' info:', target.info);
    }, function (reason) {
        console.log('poll: ' + reason);
    });
}, function (reason) {
    console.log('open: ' + reason);
});
