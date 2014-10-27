var nfc = require('../dist/nfc')
  , Q = require('q');

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
            return device.transceive([0]).then(function (receiveData) {
                console.log(' data:', receiveData);
                return target;
            });
        }).then(function (target) {
            var checkPresence = function () {
                return device.isPresent(target).then(function (isPresent) {
                    if (isPresent) {
                        process.stdout.write('.');
                        return Q.delay(100).then(checkPresence);
                    }
                });
            };
            return checkPresence();
        }).then(function () {
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
