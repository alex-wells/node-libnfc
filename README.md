node-libnfc
===========

This is a simple Node.js wrapper around `libnfc` for accessing near-field communication (NFC) devices.


Example
-------

The library can be used as follows:

```
#!javascript
var nfc = require('node-libnfc')

// Show libnfc version.
console.log('nfc version: ' + nfc.version)

// Open NFC device.
var device = nfc.open()

// Make sure device get closed on exit.
process.on('exit', function () {
    device.close()
})

// Show device name.
console.log('got device: ' + device.name)

// Try to select passive target, waiting at most 1000 ms.
device.selectPassiveTarget(1000).then(function (target) {
    // Show target ID.
    console.log('got target, ID: ' + target.id)
}, function (reason) {
    console.log('failed to get target: ' + reason)
})
```