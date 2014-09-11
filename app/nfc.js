var nfc = require('../src/build/Release/nfc.node')
  , _ = require('lodash')
  , Q = require('q')


function wrap (object, wrapper) {
    return _.extend(object, _.mapValues(wrapper, function (value, key) {
        return _.partial(value, _.bind(object[key], object))
    }))
}


function deviceSelectPassiveTarget (fn, timeout) {
    var device = this
      , timer = null

    if (timeout) {
        timer = setTimeout(function () {
            device.abortCommand()
        }, timeout)
    }

    var deferred = Q.defer()
    fn(function (target) {
        if (timer) {
            clearTimeout(timer)
        }
        if (target) {
            deferred.resolve(target)
        }
        else {
            deferred.reject('no target')
        }
    })
    return deferred.promise
}


function nfcOpen (fn, connstring) {
    var device = fn(connstring)
    if (!device.available) {
        throw new Error("no such device")
    }
    return wrap(device, {
        selectPassiveTarget: deviceSelectPassiveTarget,
    })
}


module.exports = wrap(nfc, {
    open: nfcOpen,
})
