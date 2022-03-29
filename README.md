## Important Notice

Swift Navigation has created a public RTKLIB branch to use the convbin program as a base for SBP (Swift Binary Protocol) 
  to RINEX converter (sbp2rinex).

The sbp2rinex executables for Windows, OSX and Linux are available here:
  http://downloads.swiftnav.com/tools/rinex_converter/

None of the Swift Navigation products use RTKLIB or it's derivatives for a real-time navigation.

RTKLIB post-processing results will likely differ from Swift Navigation receiver real-time output. Swift Navigation does not 
  provide support for the RTKLIB post-processing.


## Code Sign on Mac

Find Developer ID / Key ID / Issuer ID on LastPass and run these commands.
```
 export APP_BUNDLE_ID=com.swift-nav.RTKLIB
 export APPLE_DEVELOPER_ID="<Get Developer ID from LastPass, make sure to use quotes>"
 export APPLE_KEY_ID=<Get key id from LastPass>
 export APPLE_ISSUER_ID=<Get Issuer ID from LastPass>
```

Download the existing zip file from the release page. Unzip contents.
```
unzip swiftnav_rtklib-v2.3-macOS.zip -d swiftnav_rtklib-v2.3-macOS
```

Sign zip innards.
```
cd swiftnav_rtklib-v2.3-macOS
codesign -s "$APPLE_DEVELOPER_ID" \
  --options=runtime --force \
  --deep --timestamp --options=runtime *
7z a -tzip ../swiftnav_rtklib-v2.3-macOS.zip LICENSE.txt str2str sbp2rinex
cd ..
```

Sign zip file.
```
codesign -s "$APPLE_DEVELOPER_ID" --timestamp swiftnav_rtklib-v2.3-macOS.zip
```

Notarize zip file.
```
xcrun altool \
            --notarize-app \
            --file swiftnav_rtklib-v2.3-macOS.zip \
            --primary-bundle-id "$APP_BUNDLE_ID" \
            --apiKey "$APPLE_KEY_ID" \
            --apiIssuer "$APPLE_ISSUER_ID"
```

Verify notarization was approved. (Use RequestUUID returned from notarize-app step).
```
xcrun altool --verbose --notarization-info <RequestUUID> \
            --apiKey $APPLE_KEY_ID \
            --apiIssuer $APPLE_ISSUER_ID
```

Success!! Now reupload zip to Release page.


## Troubleshooting
During the "Sign zip innards" step if you see a message with this error "errSecInternalComponent" then run these commands and rerun this step.
```
security lock-keychain
security unlock-keychain
```