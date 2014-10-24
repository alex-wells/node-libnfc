var nfc = require('../src/build/Release/nfc.node')
  , Q = require('q');


var context = new nfc.Context();


class Target {
    constructor(target) {
        this.target = target;
    }

    get modulationType() {
        return this.target.modulationType;
    }

    get baudRate() {
        return this.target.baudRate;
    }

    get info() {
        return this.target.info;
    }

    toString() {
        return '[Device: ' + this.target.modulationTypeString + ' ' + this.target.baudRateString + ']';
    }
}


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

    pollTarget(timeout, period=100) {
        var deferred = Q.defer()
          , promise = deferred.promise;
        if (timeout) {
            promise = promise.timeout(timeout);
        }
        var pollTarget = function () {
            if (!promise.isPending()) {
                return;
            }
            this.device.pollTarget(function (error, target) {
                if (error) {
                    deferred.reject(error);
                }
                else if (target || !timeout) {
                    deferred.resolve(target);
                }
                else {
                    setTimeout(pollTarget, period);
                }
            });
        }.bind(this);
        pollTarget();
        return promise.then(function (target) {
            return new Target(target);
        });
    }

    isPresent(target) {
        return this.device.isPresent(target.target);
    }

    toString() {
        return '[Device: ' + this.name + ']';
    }
}


class NFC {
    static get version() {
        return context.version;
    }

    static getDevices() {
        return Q.ninvoke(context, 'getDevices');
    }

    static open(connstring) {
        return Q.ninvoke(context, 'open', connstring).then(function (device) {
            return new Device(device);
        });
    }
}


module.exports = NFC;
