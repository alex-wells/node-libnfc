var nfc = require('../src/build/Release/nfc.node');
console.log('nfc', nfc);

var context = new nfc.Context();
console.log('libnfc version: ' + context.version);
context.getDevices(function (error, devices) {
    if (error) {
        console.log('getDevices: ' + error);
        process.exit();
    }

    console.log('available devices: ' + (devices.length ? '[' + devices.join(', ') + ']' : 'none'));
    context.open(null, function (error, device) {
        if (error) {
            console.log('open: ' + error);
            process.exit();
        }

        console.log('device name: ' + device.name);
        console.log('device connstring: ' + device.connstring);

        process.on('exit', function () {
            device.close();
        });

        pollTarget = function () {
            device.pollTarget(function (error, target) {
                if (error) {
                    console.log('poll: ' + error);
                    process.exit();
                }

                if (!target) {
                    return pollTarget();
                }

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
            })
        };

        pollTarget();
    });
});
