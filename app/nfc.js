var nfc = require('../src/build/Release/nfc.node')
  , Q = require('q');


var context = new nfc.Context();


class Device {
    constructor(device) {
        this.device = device;
    }

    get name() {
        return this.device.name;
    }

    get connstring() {
        return this.device.connstring;
    }

    close() {
        return this.device.close();
    }

    setIdle() {
        return this.device.setIdle();
    }

    pollTarget(timeout) {
        var deferred = Q.defer()
          , promise = deferred.promise;
        if (timeout) {
            promise = promise.timeout(timeout);
        }
        var pollTarget = function () {
            this.device.pollTarget(function (error, target) {
                if (error) {
                    deferred.reject(error);
                }
                else if (target) {
                    deferred.resolve(target);
                }
                else if (promise.isPending()) {
                    setTimeout(pollTarget, 100);
                }
            });
            // this.device.close();
        }.bind(this);
        pollTarget();
        return promise;
    }

    isPresent(target) {
        return this.device.isPresent(target);
    }

    toString() {
        return '[Device: ' + this.name + ']';
    }
}


class NFC {
    static get version() {
        return context.version;
    }

    static get devices() {
        return context.devices;
    }

    static open(connstring) {
        return Q.ninvoke(context, 'open', connstring).then(function (device) {
            return new Device(device);
        });
    }
}


module.exports = NFC;
