# Inherit AOSP device configuration for passion.
$(call inherit-product, device/common/gps/gps_eu_supl.mk)

# Used by BusyBox
KERNEL_MODULES_DIR:=/lib/modules

# Enable Windows Media if supported by the board
WITH_WINDOWS_MEDIA:=true

# density in DPI of the LCD of this board. This is used to scale the UI
# appropriately. If this property is not defined, the default value is 160 dpi.
PRODUCT_PROPERTY_OVERRIDES += \
    ro.sf.lcd_density=120

# Add LDPI assets, in addition to MDPI
PRODUCT_LOCALES += ldpi mdpi

# Add device package overlay
DEVICE_PACKAGE_OVERLAYS += device/samsung/apollo-common/overlay

# Extra overlay for LDPI
PRODUCT_PACKAGE_OVERLAYS += vendor/cyanogen/overlay/ldpi

DISABLE_DEXPREOPT := false

PRODUCT_COPY_FILES += \
    device/samsung/apollo-common/bin/klogtail:system/xbin/klogtail

# HAL libs and other system binaries
PRODUCT_PACKAGES += \
    gralloc.apollo \
    gps.apollo \
    sensors.apollo \
    overlay.apollo \
    lights.apollo \
    libGLES_fimg \
    brcm_patchram_plus \
    screencap \
    dexpreopt \
    libstagefrighthw \
    libsecjpeg.so \
    libcamera.so \
    libsecgps.so 

# Samsung Specific tools
PRODUCT_PACKAGES += \
    SamsungServiceMode \
    G3Parts

PRODUCT_PACKAGES += \
    CMWallpapers \
    Pacman \
    Stk \
    Superuser

#Hardware OMX Codecs
PRODUCT_PACKAGES += \
    libSEC_OMX_Core.s5p6442 \
    libOMX.SEC.AVC.Decoder.s5p6442 \
    libOMX.SEC.M4V.Decoder.s5p6442 

PRODUCT_COPY_FILES += \
    device/samsung/apollo-common/sec_mm/sec_omx/sec_omx_core/secomxregistry:system/etc/secomxregistry

# Theme packages
PRODUCT_PACKAGES += \
    Androidian \
    Cyanbread 

# Bluetooth MAC Address
PRODUCT_PACKAGES += \
    bdaddr_read

PRODUCT_COPY_FILES += \
    device/samsung/apollo-common/etc/media_profiles.xml:system/etc/media_profiles.xml \
    device/samsung/apollo-common/etc/asound.conf:system/etc/asound.conf \
    device/samsung/apollo-common/etc/dhcpcd.conf:system/etc/dhcpcd.conf \
    device/samsung/apollo-common/etc/vold.fstab:system/etc/vold.fstab \
    device/samsung/apollo-common/etc/dbus.conf:system/etc/dbus.conf \
    device/samsung/apollo-common/etc/recovery.fstab:recovery/root/etc/recovery.fstab \
    device/samsung/apollo-common/etc/sysctl.conf:system/etc/sysctl.conf \
    device/samsung/apollo-common/etc/gps.conf:system/etc/gps.conf \
    device/samsung/apollo-common/etc/secgps.conf:system/etc/secgps.conf \
    device/samsung/apollo-common/etc/Credits-CM.html:system/etc/Credits-CM.html

PRODUCT_COPY_FILES += \
    device/samsung/apollo-common/lib/hw/copybit.apollo.so:system/lib/hw/copybit.apollo.so

# Live wallpaper packages
PRODUCT_PACKAGES += \
    LiveWallpapers \
    LiveWallpapersPicker \
    MagicSmokeWallpapers \
    VisualizationWallpapers \
    librs_jni

# The OpenGL ES API level that is natively supported by this device.
# This is a 16.16 fixed point number
PRODUCT_PROPERTY_OVERRIDES := \
    ro.opengles.version=65537

# Keyboard maps
PRODUCT_COPY_FILES += \
	device/samsung/apollo-common/usr/keychars/qwerty.kcm.bin:system/usr/keychars/qwerty.kcm.bin \
	device/samsung/apollo-common/usr/keychars/qwerty2.kcm.bin:system/usr/keychars/qwerty2.kcm.bin \
	device/samsung/apollo-common/usr/keychars/qt602240_ts_input.kcm.bin:system/usr/keychars/qt602240_ts_input.kcm.bin \
	device/samsung/apollo-common/usr/keychars/s3c-keypad.kcm.bin:system/usr/keychars/s3c-keypad.kcm.bin \
	device/samsung/apollo-common/usr/keychars/sec_jack.kcm.bin:system/usr/keychars/sec_jack.kcm.bin \
	device/samsung/apollo-common/usr/keylayout/qt602240_ts_input.kl:system/usr/keylayout/qt602240_ts_input.kl \
	device/samsung/apollo-common/usr/keylayout/s3c-keypad.kl:system/usr/keylayout/s3c-keypad.kl \
	device/samsung/apollo-common/usr/keylayout/sec_headset.kl:system/usr/keylayout/sec_headset.kl 

# wifi
PRODUCT_COPY_FILES += \
	device/samsung/apollo-common/etc/nvram_mfg.txt:system/etc/wifi/nvram_mfg.txt \
	device/samsung/apollo-common/etc/nvram_net.txt:system/etc/wifi/nvram_net.txt \
	device/samsung/apollo-common/etc/wifi.conf:system/etc/wifi/wifi.conf \
	device/samsung/apollo-common/etc/wpa_supplicant.conf:system/etc/wifi/wpa_supplicant.conf 

# ril
PRODUCT_COPY_FILES += \
	device/samsung/apollo-common/lib/libril.so:system/lib/libril.so \
	device/samsung/apollo-common/lib/libsec-ril.so:system/lib/libsec-ril.so \
	device/samsung/apollo-common/bin/rild:system/bin/rild

# camera
PRODUCT_COPY_FILES += \
	device/samsung/apollo-common/lib/libsamsungcamera.so:system/lib/libsamsungcamera.so \
        device/samsung/apollo-common/lib/libsecjpeg.so:system/lib/libsecjpeg.so

# bluetooth
PRODUCT_COPY_FILES += \
        device/samsung/apollo-common/bcm4329.hcd:system/vendor/firmware/bcm4329.hcd

# sensors
PRODUCT_COPY_FILES += \
	device/samsung/apollo-common/bin/akmd2:system/bin/akmd2

PRODUCT_PROPERTY_OVERRIDES += \
       ro.telephony.ril_class=samsung \
       ro.telephony.sends_barcount=1 \
       mobiledata.interfaces=pdp0,eth0,gprs,ppp0 \
       wifi.interface=eth0 \
       wifi.supplicant_scan_interval=20 \
       dalvik.vm.dexopt-flags=m=y \
       dalvik.vm.execution-mode=int:jit \
       dalvik.vm.heapsize=32m \
       persist.sys.purgeable_assets=0 \
       ro.compcache.default=0 \
       persist.sys.use_dithering=0 \
       persist.sys.use_16bpp_alpha=1 \
       persist.sys.led_trigger=none
#       persist.service.usb.setting=0 \
#       dev.sfbootcomplete=0 \
#       persist.sys.vold.switchexternal=1


# enable Google-specific location features,
# like NetworkLocationProvider and LocationCollector
PRODUCT_PROPERTY_OVERRIDES += \
        ro.com.google.locationfeatures=1 \
        ro.com.google.networklocation=1


# we have enough storage space to hold precise GC data
# PRODUCT_TAGS += dalvik.gc.type-precise

# Extended JNI checks
# The extended JNI checks will cause the system to run more slowly, but they can spot a variety of nasty bugs 
# before they have a chance to cause problems.
# Default=true for development builds, set by android buildsystem.
PRODUCT_PROPERTY_OVERRIDES += \
    ro.kernel.android.checkjni=0 \
    dalvik.vm.checkjni=false



