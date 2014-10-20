{
    'targets': [
        {
            'target_name': 'nfc',
            'sources': ['nfc.cc', 'nfc/context.cc', 'nfc/device.cc', 'nfc/util.cc'],
            'link_settings': {
                'libraries': ['-l nfc']
            }
        }
    ]
}
