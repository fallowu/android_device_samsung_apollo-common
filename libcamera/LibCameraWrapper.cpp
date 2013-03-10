/*
 * Copyright (C) 2011 The Android Open Source Project
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

// Adapted from JordanCameraWrapper

// We can't use CameraParameters constants because we're linking against
// Samsung's libcamera_client.so

#define LOG_TAG "LibCameraWrapper"
//#define LOG_NDEBUG 0

#include <cmath>
#include <dlfcn.h>
#include <fcntl.h>
#include <linux/videodev2.h>
#include <sys/ioctl.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <camera/Camera.h>
#include "LibCameraWrapper.h"

namespace android {

typedef sp<CameraHardwareInterface> (*OpenCamFunc)(int);
typedef void (*GetCamInfo)(int, struct CameraInfo*);

static void * g_libHandle = NULL;
static OpenCamFunc g_openCameraHardware = NULL;
static GetCamInfo g_getCameraInfo = NULL;

static const int CAMERA_CMD_SET_OBJECT_TRACKING_POSITION = 1103;
static const int CAMERA_CMD_SET_TOUCH_AF = 1105;

#ifndef FIRST_CAMERA_FACING
#define FIRST_CAMERA_FACING CAMERA_FACING_BACK
#endif
#ifndef FIRST_CAMERA_ORIENTATION
#define FIRST_CAMERA_ORIENTATION 90
#endif

static bool g_isRecording;

static const CameraInfo sCameraInfo[] = {
    {
        FIRST_CAMERA_FACING,
        FIRST_CAMERA_ORIENTATION,  /* orientation */
    }
};

static void ensureLibOpened()
{
    if (g_libHandle == NULL) {
        g_libHandle = ::dlopen("libsamsungcamera.so", RTLD_NOW);
        if (g_libHandle == NULL) {
            assert(0);
            LOGE("dlopen() error: %s\n", dlerror());
        } else {
            g_openCameraHardware = (OpenCamFunc) ::dlsym(g_libHandle, "openCameraHardware");
            assert(g_openCameraHardware != NULL);
        }
    }
}

extern "C" int HAL_getNumberOfCameras()
{
    return 1;
}

extern "C" void HAL_getCameraInfo(int cameraId, struct CameraInfo* cameraInfo)
{
    ensureLibOpened();
    memcpy(cameraInfo, &sCameraInfo[cameraId], sizeof(CameraInfo));
}

extern "C" sp<CameraHardwareInterface> HAL_openCameraHardware(int cameraId)
{
    LOGV("openCameraHardware: call createInstance");
    ensureLibOpened();
    return LibCameraWrapper::createInstance(cameraId);
}

wp<CameraHardwareInterface> LibCameraWrapper::singleton[2] = { 0 };

sp<CameraHardwareInterface> LibCameraWrapper::createInstance(int cameraId)
{
    LOGV("%s :", __func__);
    if (singleton[cameraId] != NULL) {
        sp<CameraHardwareInterface> hardware = singleton[cameraId].promote();
        if (hardware != NULL) {
            return hardware;
        }
    }

    ensureLibOpened();

    sp<CameraHardwareInterface> hardware(new LibCameraWrapper(cameraId));
    singleton[cameraId] = hardware;
    return hardware;
}

LibCameraWrapper::LibCameraWrapper(int cameraId) :
    mLibInterface(g_openCameraHardware(cameraId)),
    mCameraId(cameraId),
    mVideoMode(false),
    mContinuousAf(false),
    mFixFocus(false),
    mTouchFocus(false)
{
    LOGV("%s :", __func__);
}

LibCameraWrapper::~LibCameraWrapper()
{
    LOGV("%s :", __func__);
}

sp<IMemoryHeap>
LibCameraWrapper::getPreviewHeap() const
{
    LOGV("%s :", __func__);
    return mLibInterface->getPreviewHeap();
}

sp<IMemoryHeap>
LibCameraWrapper::getRawHeap() const
{
    LOGV("%s :", __func__);
    return mLibInterface->getRawHeap();
}

void
LibCameraWrapper::setCallbacks(notify_callback notify_cb,
                                  data_callback data_cb,
                                  data_callback_timestamp data_cb_timestamp,
                                  void* user)
{
    LOGV("%s :", __func__);
    mLibInterface->setCallbacks(notify_cb, data_cb, data_cb_timestamp, user);
}

void
LibCameraWrapper::enableMsgType(int32_t msgType)
{
    LOGV("%s :", __func__);
    mLibInterface->enableMsgType(msgType);
}

void
LibCameraWrapper::disableMsgType(int32_t msgType)
{
    LOGV("%s :", __func__);
    mLibInterface->disableMsgType(msgType);
}

bool
LibCameraWrapper::msgTypeEnabled(int32_t msgType)
{
    LOGV("%s :", __func__);
    return mLibInterface->msgTypeEnabled(msgType);
}

status_t
LibCameraWrapper::startPreview()
{
    LOGV("%s :", __func__);
    status_t ret = mLibInterface->startPreview();

    if (mFixFocus) {
        LOGV("Fix focus mode");
        // We need to switch the focus mode once after switching from video or the camera won't work.
        // Note: If the previous mode was macro, then it actually doesn't matter since the bug doesn't affect that case.
        CameraParameters pars = mLibInterface->getParameters();
        const char *prevFocusMode = pars.get("focus-mode");
        pars.set("focus-mode", "macro");
        mLibInterface->setParameters(pars);
        pars.set("focus-mode", prevFocusMode);
        mLibInterface->setParameters(pars);
        mFixFocus = false;
    }

    return ret;
}

bool
LibCameraWrapper::useOverlay()
{
    LOGV("%s :", __func__);
    return mLibInterface->useOverlay();
}

status_t
LibCameraWrapper::setOverlay(const sp<Overlay> &overlay)
{
    LOGV("%s :", __func__);
    return mLibInterface->setOverlay(overlay);
}

void
LibCameraWrapper::stopPreview()
{
    LOGV("%s :", __func__);
    mLibInterface->stopPreview();
}

bool
LibCameraWrapper::previewEnabled()
{
    LOGV("%s :", __func__);
    return mLibInterface->previewEnabled();
}

status_t
LibCameraWrapper::startRecording()
{
    LOGV("%s :", __func__);
    g_isRecording = true;
    return mLibInterface->startRecording();
}

void
LibCameraWrapper::stopRecording()
{
    LOGV("%s :", __func__);
    g_isRecording = false;
    mLibInterface->stopRecording();
}

bool
LibCameraWrapper::recordingEnabled()
{
    LOGV("%s :", __func__);
    return mLibInterface->recordingEnabled();
}

void
LibCameraWrapper::releaseRecordingFrame(const sp<IMemory>& mem)
{
    LOGV("%s :", __func__);
    return mLibInterface->releaseRecordingFrame(mem);
}

status_t
LibCameraWrapper::autoFocus()
{
    LOGV("%s :", __func__);
    return mLibInterface->autoFocus();
}

status_t
LibCameraWrapper::cancelAutoFocus()
{
    LOGV("%s :", __func__);
    return mLibInterface->cancelAutoFocus();
}

status_t
LibCameraWrapper::takePicture()
{
    LOGV("%s :", __func__);
    return mLibInterface->takePicture();
}

status_t
LibCameraWrapper::cancelPicture()
{
    LOGV("%s :", __func__);
    return mLibInterface->cancelPicture();
}

status_t
LibCameraWrapper::setParameters(const CameraParameters& params)
{
    LOGV("%s :", __func__);

    CameraParameters pars(params.flatten());

    /*
     * getInt returns -1 if the value isn't present and 0 on parse failure,
     * so if it's larger than 0, we can be sure the value was parsed properly
     */
    mVideoMode = pars.getInt("cam-mode") > 0;
    pars.remove("cam-mode");

//    if (mVideoMode) {
//
//    }
    return mLibInterface->setParameters(params);
}

CameraParameters
LibCameraWrapper::getParameters() const
{
    LOGV("%s :", __func__);
    CameraParameters ret = mLibInterface->getParameters();
    
    // This is for detecting if we're in camcorder mode or not
    ret.set("cam-mode", mVideoMode ? "1" : "0");
    return ret;
}

status_t
LibCameraWrapper::sendCommand(int32_t cmd, int32_t arg1, int32_t arg2)
{
    LOGV("%s :", __func__);
    return mLibInterface->sendCommand(cmd, arg1, arg2);
}

void
LibCameraWrapper::release()
{
    LOGV("%s :", __func__);
    mLibInterface->release();
}

status_t
LibCameraWrapper::dump(int fd, const Vector<String16>& args) const
{
    LOGV("%s :", __func__);
    return mLibInterface->dump(fd, args);
}

}; //namespace android
