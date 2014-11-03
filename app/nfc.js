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
        var promise;
        var pollTarget = device => {
            return Q.ninvoke(device, 'pollTarget').then(target => {
                if (target) {
                    return new Target(target);
                }
                if (!timeout) {
                    return null;
                }
                return Q.delay(period).then(() => {
                    if (promise.isPending()) {
                        return pollTarget(device);
                    }
                });
            });
        };
        promise = pollTarget(this.device);
        if (timeout) {
            promise = promise.timeout(timeout);
        }
        return promise;
    }

    isPresent(target) {
        return Q.ninvoke(this.device, 'isPresent', target.target);
    }

    transceive(transmit, receiveCapacity=4096) {
        return Q.ninvoke(this.device, 'transceive', transmit, receiveCapacity);
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
        return Q.ninvoke(context, 'open', connstring).then(device => new Device(device));
    }
}


module.exports = NFC;
