!!! note ""
    MetaData API lets you persist board and shield identity data (serial numbers, hardware versions, passwords, and a handful of generic extra slots) to flash, so it survives a reset or a firmware reflash.

    This is useful to identify a board at runtime, to check that a shield matches the board it is plugged into, or to store any other small piece of configuration that needs to outlive a power cycle.

!!! warning
    This is **not** secure storage: data is written in plain form to the NVS partition, with no secure element or read-protection involved. The password fields are only meant to gate casual spin/shield mismatches, not to protect a real secret.

### Include

MetaData API is part of Spin API: it is made available by including the `SpinAPI.h` header. From there, a `spin.metaData` object is available to interact with the API.
!!! note
    ```
    #include <SpinAPI.h>
    ```

## Fixed-length fields

Serial numbers, versions, and passwords are stored under a fixed-length field. Writing requires passing the exact expected size, and reading requires providing a buffer at least as large as the field.

| Field            | Length constant       | Length (bytes) |
| ---------------- | --------------------- | --------------- |
| Spin serial number   | `SPIN_SERIAL_LEN`     | 13 |
| Shield serial number | `SHIELD_SERIAL_LEN`   | 13 |
| Spin password        | `SPIN_PASSWORD_LEN`   | 10 |
| Shield password      | `SHIELD_PASSWORD_LEN` | 10 |

!!! example
    === "Serial numbers"
        ```cpp
        const char spin_serial[]   = "SPIN000000001"; // 13 chars
        const char shield_serial[] = "SHLD000000001"; // 13 chars

        spin.metaData.setSpinSerialNumber(spin_serial, sizeof(spin_serial) - 1);
        spin.metaData.setShieldSerialNumber(shield_serial, sizeof(shield_serial) - 1);

        char buffer[SPIN_SERIAL_LEN + 1];
        int8_t bytes_read = spin.metaData.getSpinSerialNumber(buffer, sizeof(buffer));
        if (bytes_read >= 0)
        {
            buffer[SPIN_SERIAL_LEN] = '\0'; // The stored value is not null-terminated
        }
        ```

    === "Passwords"
        ```cpp
        const char spin_password[]   = "SPINPASS01"; // 10 chars
        const char shield_password[] = "SHLDPASS01"; // 10 chars

        spin.metaData.setSpinPassword(spin_password, sizeof(spin_password) - 1);
        spin.metaData.setShieldPassword(shield_password, sizeof(shield_password) - 1);

        char buffer[SHIELD_PASSWORD_LEN + 1];
        int8_t bytes_read = spin.metaData.getShieldPassword(buffer, sizeof(buffer));
        if (bytes_read >= 0)
        {
            buffer[SHIELD_PASSWORD_LEN] = '\0';
        }
        ```

!!! note
    Stored values are raw ASCII bytes, not null-terminated. If fewer meaningful characters are needed than the field's fixed length, the caller must pad the buffer itself (e.g. with spaces or zeros) up to the full length.

## Hardware versions

Spin and shield hardware versions are stored as three separate `major`/`minor`/`rev` numbers rather than a buffer.

!!! example
    ```cpp
    spin.metaData.setSpinVersion(1, 2, 0);      // v1.2.0
    spin.metaData.setShieldVersion(2, 0, 1);    // v2.0.1

    uint8_t major, minor, rev;
    if (spin.metaData.getSpinVersion(&major, &minor, &rev) == 0)
    {
        // Use major, minor, rev
    }
    ```

## Generic extra slots

Beyond the predefined fields, `METADATA_EXTRA_COUNT` generic slots (indexed `0` to `METADATA_EXTRA_COUNT - 1`) are available to store any small, variable-length piece of data, up to `METADATA_EXTRA_MAX_LEN` bytes each.

!!! example
    ```cpp
    const char label[] = "MyLabel";
    spin.metaData.setExtraData(0, (const uint8_t*)label, strlen(label));

    uint8_t buffer[METADATA_EXTRA_MAX_LEN];
    int8_t bytes_read = spin.metaData.getExtraData(0, buffer, sizeof(buffer));
    if (bytes_read >= 0)
    {
        // buffer[0 .. bytes_read - 1] contains the stored data
    }
    ```

!!! note
    Unlike the fixed-length fields, the actual stored size of an extra slot is not known ahead of the read. The buffer passed to `getExtraData()` must therefore always be at least `METADATA_EXTRA_MAX_LEN` bytes, regardless of how much data was actually written to that slot.

## Error codes

All `set***()`/`get***()` functions return `0` (for setters) or the number of bytes read (for getters) on success, and a negative value on error:

| Return value | Meaning |
| ------------ | ------- |
| `-1` | Underlying storage error (nothing stored yet, corrupted data, or version mismatch) |
| `-2` | Provided buffer/size does not match what the field requires |
| `-3` | Extra slot `index` is out of range (`setExtraData()`/`getExtraData()` only) |

## Clearing all metadata

`clearAllMetaData()` erases every field owned by this class (serial numbers, versions, passwords, and all extra slots) without touching any other module's data in the shared NVS partition (e.g. ADC calibration or safety thresholds).

!!! example
    ```cpp
    spin.metaData.clearAllMetaData();
    ```

## API Reference

::: doxy.powerAPI.class
name: MetaDataAPI
