"""Different spellings of one error bit must not silently replace its label."""

from pathlib import Path
import sys

sys.path.insert(0, str(Path(__file__).resolve().parents[1]))
import pytest
import voluptuous as vol

from components import jk_balancer, jk_bms, jk_bms_ble


@pytest.mark.parametrize("module", [jk_balancer, jk_bms, jk_bms_ble])
@pytest.mark.parametrize("alias", ["01", "0x1"])
def test_conflicting_aliases_are_rejected(module, alias):
    with pytest.raises(vol.Invalid, match="Conflicting labels"):
        module.error_overrides({"1": "First label", alias: "Different label"})


@pytest.mark.parametrize("module", [jk_balancer, jk_bms, jk_bms_ble])
def test_identical_aliases_and_suppressed_bits_remain_valid(module):
    assert module.error_overrides(
        {"1": "Custom label", "0x1": "Custom label", "2": ""}
    ) == {1: "Custom label", 2: ""}
