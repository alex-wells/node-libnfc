node-libnfc
===========

This is a simple Node.js wrapper around `libnfc` for accessing near-field communication (NFC) devices.


Example
-------

The library can be used as follows:

```
#!javascript
var nfc = require('node-libnfc');

// Show libnfc version.
console.log('libnfc version: ' + nfc.version);

// Open NFC device.
nfc.open().then(function (device) {
    // Make sure device gets closed on exit.
    process.on('exit', function () {
        device.close();
    });

    // Show device name.
    console.log('got device: ' + device.name);

    // Try to poll for target, waiting at most 1000 ms.
    device.pollTarget(1000).then(function (target) {
        // Show target info.
        console.log('got target:', target.info);
    }, function (reason) {
        console.log('failed to get target: ' + reason);
    });
}, function (reason) {
    console.log('failed to open device: ' + reason);
});
```
