# spin.metaData implementation progress

Tracks progress of the `spin.metaData` feature (persist board/shield serial
numbers, versions, shield password, and 5 extra slots to flash via NVS).
Full design/rationale: see the plan this was generated from
(`based-on-the-owntech-cheerful-gizmo.md`).

This file is a scratch progress tracker, not user-facing documentation —
safe to delete once all commits below are checked off and the feature has
landed.

## Commits

- [x] **Commit 1** — flash driver: add `BOARD_METADATA` category to
  `nvs_category_t` + `nvs_storage_get_free_space()` diagnostic
  (`zephyr/modules/owntech_flash_driver/zephyr/public_api/nvs_storage.h`,
  `nvs_storage.c`)
- [ ] **Commit 2** — new `MetaDataAPI` class
  (`zephyr/modules/owntech_spin_api/zephyr/src/MetaDataAPI.h`, `.cpp`)
- [ ] **Commit 3** — wire `MetaDataAPI` into `SpinAPI` as `spin.metaData`
  (`SpinAPI.h`, `SpinAPI.cpp`, `zephyr/modules/owntech_spin_api/zephyr/CMakeLists.txt`)
- [ ] **Commit 4** — test harness `main.cpp` with serial command menu
  (`h`/`w`/`r`/`c`/`f`) (`src/main.cpp`)
- [ ] **Commit 5** — automated `pyserial` test script
  (`owntech/scripts/test_metadata_nvs.py`)

## Status log

(Each commit appends a short entry here when it lands: what was done,
files touched, any deviation from the plan.)

- **Commit 1 done**: added `BOARD_METADATA = 0x0400` to `nvs_category_t`
  and a new `nvs_storage_get_free_space()` diagnostic (wraps Zephyr's
  `nvs_calc_free_space(&fs)`) to
  `zephyr/modules/owntech_flash_driver/zephyr/public_api/nvs_storage.h`
  and `nvs_storage.c`. No deviation from plan.
