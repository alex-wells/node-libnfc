var nfc = require('../dist/nfc');

console.log('libnfc version: ' + nfc.version);
nfc.getDevices().then(function(devices) {
    console.log('available devices: ' + (devices.length ? '[' + devices.join(', ') + ']' : 'none'));
    return nfc.open();
}).then(function (device) {
    console.log('device: ' + device);
    console.log(' name: ', device.name);
    console.log(' connstring: ', device.connstring);

    process.on('exit', function () {
        device.close();
    });

    function pollTarget() {
        device.pollTarget(2500).then(function (target) {
            console.log('target: ' + target);
            console.log(' modulationType:', target.modulationType);
            console.log(' baudRate:', target.baudRate);
            console.log(' info:', target.info);
            while (device.isPresent(target)) {
                process.stdout.write('.');
            }
            process.stdout.write('\n');
            console.log('removed');
            pollTarget();
        }, function (reason) {
            console.log('poll: ' + reason);
        }).done();
    }

    pollTarget();
}, function (reason) {
    console.log('open: ' + reason);
}).done();
