import storage
import usb_cdc, usb_hid
from pico_synth_sandbox import free_module

storage.remount("/", False, disable_concurrent_write_protection=True)
usb_hid.disable()
usb_cdc.enable(console=True, data=False)
free_module((storage, usb_hid, usb_cdc))